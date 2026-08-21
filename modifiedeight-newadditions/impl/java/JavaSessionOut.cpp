#include <java/JavaSession.hpp>
#include <java/JavaLog.hpp>
#include <java/JavaIdMap.hpp>
#include <java/JavaY.hpp>

#include <Minecraft.hpp>
#include <entity/LocalPlayer.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <level/MultiPlayerLevel.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/SignTileEntity.hpp>

#include <network/Packet.hpp>
#include <network/packet/AnimatePacket.hpp>
#include <network/packet/ContainerSetSlotPacket.hpp>
#include <network/packet/DropItemPacket.hpp>
#include <network/packet/EntityDataPacket.hpp>
#include <network/packet/InteractPacket.hpp>
#include <network/packet/MessagePacket.hpp>
#include <network/packet/MovePlayerPacket.hpp>
#include <network/packet/PlayerActionPacket.hpp>
#include <network/packet/PlayerEquipmentPacket.hpp>
#include <network/packet/RemoveBlockPacket.hpp>
#include <network/packet/RespawnPacket.hpp>
#include <network/packet/UseItemPacket.hpp>

#include <math.h>

/*
 * Everything m8 wants to say to a server goes through RakNetInstance::send.
 * When a Java session owns the level there is no RakNet peer on the other end,
 * so this is where MCPE packets are turned into protocol 47 ones.
 *
 * The contract is deliberately "swallow everything": any MCPE packet we do not
 * translate must still be consumed, because letting it reach the dead RakNet
 * peer is at best wasted work and at worst a crash (ClientSideNetworkHandler
 * happily fires ReadyPacket and RequestChunkPackets at level-generated time).
 */

// m8 lays its inventory container out as 0..8 linked hotbar handles, 9..17 the
// hotbar proper, then 18..44 the backpack. Java counts 9..35 backpack then
// 36..44 hotbar. Returns -1 for a slot with no Java equivalent.
static int32_t javaSlotForM8Container(int32_t m8Index) {
	if(m8Index >= 9 && m8Index <= 17) {
		return 36 + (m8Index - 9);
	}
	if(m8Index >= 18 && m8Index <= 44) {
		return 9 + (m8Index - 18);
	}
	return -1;
}

static void javaWriteStack(JavaByteBuf* out, const ItemInstance& item) {
	if(!item.isValid || item.count <= 0) {
		out->writeShort(-1);
		return;
	}
	int32_t id = 0;
	if(item.itemClass) {
		id = item.itemClass->itemID;
	} else if(item.tileClass) {
		id = item.tileClass->blockID;
	}
	int32_t javaId = JavaIdMap::m8ItemToJava(id);
	if(javaId <= 0) {
		out->writeShort(-1);
		return;
	}
	out->writeShort((int16_t)javaId);
	out->writeByte((uint8_t)item.count);
	out->writeShort((int16_t)item.metadata);
	out->writeByte(0); // no NBT
}

/*
 * Creative content is client authored.  m8 fills its hotbar from its own palette
 * screen, and a Java server only ever learns what is in a creative slot from a
 * Creative Inventory Action - its own creative window starts empty.  This is the
 * one push that makes the two agree, sent once the level goes live and then per
 * slot as the player edits it (through ContainerSetSlotPacket above).
 */
void JavaSession::sendCreativeSlot(int32_t javaSlot, const ItemInstance* item) {
	if(this->phase != JavaSession::PLAY || this->gamemode != 1) {
		return;
	}
	JavaByteBuf out;
	this->beginPacket(&out, 0x10); // Creative Inventory Action
	out.writeShort((int16_t)javaSlot);
	if(item) {
		javaWriteStack(&out, *item);
	} else {
		out.writeShort(-1);
	}
	this->send(out);
}

void JavaSession::pushCreativeInventory() {
	if(this->gamemode != 1 || !this->player || !this->player->inventory) {
		return;
	}
	Inventory* inv = this->player->inventory;
	int32_t size = (int32_t)inv->items.size();
	int32_t pushed = 0;
	for(int32_t i = 9; i < size && i <= 44; ++i) {
		ItemInstance* item = inv->getItem(i);
		if(!item || !item->isValid || item->count <= 0) {
			continue;
		}
		int32_t javaSlot = javaSlotForM8Container(i);
		if(javaSlot < 0) {
			continue;
		}
		this->sendCreativeSlot(javaSlot, item);
		++pushed;
	}
	if(pushed) {
		javaLog("pushed %d creative inventory slot(s) to the server", pushed);
	}
}

/*
 * m8 only announces its held item when the *item* changes (LocalPlayer::tick
 * compares ids), so switching between two slots holding the same thing would
 * leave the server pointing at the old one.  Comparing the slot itself once a
 * tick covers every way the selection can move: hotbar tap, palette pick, or
 * scroll wheel.
 */
void JavaSession::syncHeldSlot() {
	if(!this->player || !this->player->inventory) {
		return;
	}
	int32_t slot = this->player->inventory->selectedSlot;
	if(slot < 0 || slot > 8 || slot == this->selectedSlot) {
		return;
	}
	this->sendHeldSlot(slot);
}

void JavaSession::sendChatLine(const std::string& text) {
	if(this->phase != JavaSession::PLAY || text.empty()) {
		return;
	}
	// 1.8 refuses anything over 100 characters and disconnects the sender.
	std::string line = text;
	if(line.size() > 100) {
		line.resize(100);
	}
	JavaByteBuf out;
	this->beginPacket(&out, 0x01);
	out.writeString(line);
	this->send(out);
	this->flushOut();
}

/*
 * 1.8 carries sign lines as chat components, and C12PacketUpdateSign hands each
 * one straight to IChatComponent.Serializer.jsonToComponent, which answers null
 * for an empty string and takes the server's packet thread down with it on the
 * next getUnformattedText().  So every line is written, blank ones included, as
 * a real object.  The server strips formatting codes itself, but the quotes and
 * backslashes have to survive the trip as text rather than as JSON syntax.
 */
static void javaWriteSignLine(JavaByteBuf* out, const std::string& text) {
	std::string json = "{\"text\":\"";
	for(size_t i = 0; i < text.size(); ++i) {
		unsigned char c = (unsigned char)text[i];
		if(c == '"' || c == '\\') {
			json += '\\';
			json += (char)c;
		} else if(c == '\n' || c == '\r' || c == '\t') {
			json += ' ';
		} else if(c >= 0x20) {
			json += (char)c;
		}
	}
	json += "\"}";
	out->writeString(json);
}

/*
 * The server only accepts this while the sign is still editable and still owned
 * by us, which is the window it opens with Sign Editor Open right after we place
 * one - the same moment m8 puts its TextEditScreen up.  Pressing Done is the one
 * and only chance to say what the board reads.
 */
void JavaSession::sendSignText(int32_t mx, int32_t my, int32_t mz) {
	if(!this->level) {
		return;
	}
	int32_t jy = JavaY::toJava(my);
	if(jy < 0 || jy > 255) {
		return;
	}
	TileEntity* te = this->level->getTileEntity(mx, my, mz);
	if(!te || te->type != 4) {
		return;
	}
	SignTileEntity* sign = (SignTileEntity*)te;

	JavaByteBuf out;
	this->beginPacket(&out, 0x12); // Update Sign
	out.writePosition(mx, jy, mz);
	for(int32_t i = 0; i < 4; ++i) {
		javaWriteSignLine(&out, sign->textLines[i]);
	}
	this->send(out);
	javaDebug("out sign %d %d %d [%s|%s|%s|%s]", mx, jy, mz,
	          sign->textLines[0].c_str(), sign->textLines[1].c_str(),
	          sign->textLines[2].c_str(), sign->textLines[3].c_str());
}

bool_t JavaSession::interceptSend(Packet* pk) {
	if(!this->isActive()) {
		return 0;
	}
	if(!pk) {
		return 1;
	}
	if(this->phase != JavaSession::PLAY || !this->levelLive) {
		return 1; // still joining - nothing we send now would be understood
	}

	// ---- movement -----------------------------------------------------------
	/*
	 * Movement never comes through here: LocalPlayer::tick hands a Java session
	 * to JavaSession::playerTick instead, which reports every tick the way a Java
	 * client does.  This is only the backstop for anything else that reaches for
	 * sendPosition(), and dropping it on the floor is the right answer twice
	 * over.  MovePlayerPacket's y is posY - ridingHeight, and posY lags the
	 * collision box by ySize for several ticks after a step up (Entity::move
	 * raises the box immediately and lets the camera catch up), so relayed
	 * verbatim it claims a position inside the slab or stair just climbed onto -
	 * NetHandlerPlayServer sees a box that collides where the previous one did
	 * not and answers with a teleport back, which was the shove on the first tick
	 * of every step.  playerTick reads boundingBox.minY, the value Java actually
	 * means.
	 */
	if(dynamic_cast<MovePlayerPacket*>(pk)) {
		return 1;
	}

	// ---- breaking blocks ----------------------------------------------------
	if(RemoveBlockPacket* p = dynamic_cast<RemoveBlockPacket*>(pk)) {
		int32_t x = p->x;
		int32_t y = (int32_t)p->y;
		int32_t z = p->z;
		int32_t face = 1;
		if(this->minecraft && this->minecraft->selectedObject.hitType == 2
		   && this->minecraft->selectedObject.field_4 == x
		   && this->minecraft->selectedObject.field_8 == y
		   && this->minecraft->selectedObject.field_C == z) {
			face = this->minecraft->selectedObject.field_10;
		}
		int32_t jy = JavaY::toJava(y);
		// m8 has already taken the block out locally, so announce both ends of
		// the dig at once. If the server disagrees it answers with a Block
		// Change and the world heals itself.
		this->sendDigging(0, x, jy, z, face);
		this->sendDigging(2, x, jy, z, face);
		javaDebug("out dig %d %d %d face=%d", x, jy, z, face);
		return 1;
	}

	// ---- using / placing ---------------------------------------------------
	if(UseItemPacket* p = dynamic_cast<UseItemPacket*>(pk)) {
		JavaByteBuf out;
		this->beginPacket(&out, 0x08); // Player Block Placement
		if(p->face == 255) {
			// "used in air": 1.8 signals that with an impossible position and
			// a direction of -1.
			out.writePosition(-1, -1, -1);
			out.writeByte(0xFF);
			this->writeHeldStack(&out);
			out.writeByte(0xFF);
			out.writeByte(0xFF);
			out.writeByte(0xFF);
		} else {
			int32_t jy = JavaY::toJava(p->y);
			if(jy < 0 || jy > 255) {
				return 1;
			}
			out.writePosition(p->x, jy, p->z);
			out.writeByte((uint8_t)(p->face & 0xFF));
			this->writeHeldStack(&out);
			int32_t cx = (int32_t)(p->faceX * 16.0f);
			int32_t cy = (int32_t)(p->faceY * 16.0f);
			int32_t cz = (int32_t)(p->faceZ * 16.0f);
			out.writeByte((uint8_t)(cx & 0xFF));
			out.writeByte((uint8_t)(cy & 0xFF));
			out.writeByte((uint8_t)(cz & 0xFF));
		}
		this->send(out);
		javaDebug("out place %d %d %d face=%d cursor=%.2f,%.2f,%.2f held=%d",
		          p->x, (int32_t)JavaY::toJava(p->y), p->z, p->face,
		          p->faceX, p->faceY, p->faceZ, this->selectedSlot);
		return 1;
	}

	// ---- sign text ----------------------------------------------------------
	/*
	 * TextEditScreen is the only thing in 0.8.1 that sends an EntityDataPacket,
	 * and it sends exactly one: SignTileEntity::getUpdatePacket, when the player
	 * closes the board.  The tag it carries is the same text the tile entity
	 * still holds, so read it from there rather than unpicking the NBT.
	 */
	if(EntityDataPacket* p = dynamic_cast<EntityDataPacket*>(pk)) {
		this->sendSignText(p->x, p->y, p->z);
		return 1;
	}

	// ---- releasing a charged item (bow) -------------------------------------
	if(PlayerActionPacket* p = dynamic_cast<PlayerActionPacket*>(pk)) {
		if(p->action == 5) {
			this->sendDigging(5, 0, 0, 0, 255);
		}
		return 1;
	}

	// ---- arm swing ----------------------------------------------------------
	if(AnimatePacket* p = dynamic_cast<AnimatePacket*>(pk)) {
		if(p->action == 1) {
			JavaByteBuf out;
			this->beginPacket(&out, 0x0A); // Animation - no payload
			this->send(out);
		}
		return 1;
	}

	// ---- hitting / right-clicking an entity --------------------------------
	if(InteractPacket* p = dynamic_cast<InteractPacket*>(pk)) {
		JavaByteBuf out;
		this->beginPacket(&out, 0x02); // Use Entity
		out.writeVarInt(p->type);      // the *target* id, despite the name
		out.writeVarInt(p->action == 2 ? 1 : 0); // 1 = ATTACK, 0 = INTERACT
		this->send(out);
		return 1;
	}

	// ---- chat ---------------------------------------------------------------
	if(MessagePacket* p = dynamic_cast<MessagePacket*>(pk)) {
		this->sendChatLine(std::string(p->message.C_String()));
		return 1;
	}

	// ---- respawn ------------------------------------------------------------
	if(dynamic_cast<RespawnPacket*>(pk)) {
		JavaByteBuf out;
		this->beginPacket(&out, 0x16); // Client Status
		out.writeVarInt(0);            // perform respawn
		this->send(out);
		return 1;
	}

	// ---- dropping the held stack -------------------------------------------
	if(dynamic_cast<DropItemPacket*>(pk)) {
		this->sendDigging(3, 0, 0, 0, 255); // DROP_ALL_ITEMS
		return 1;
	}

	// ---- switching hotbar slot ---------------------------------------------
	if(PlayerEquipmentPacket* p = dynamic_cast<PlayerEquipmentPacket*>(pk)) {
		// m8 reports the container index it linked; turn it back into 0..8.
		int32_t slot = p->slot;
		if(slot >= 9 && slot <= 17) {
			slot -= 9;
		}
		if(slot >= 0 && slot <= 8 && slot != this->selectedSlot) {
			javaDebug("out held slot %d (m8 index %d)", slot, p->slot);
			this->sendHeldSlot(slot);
		}
		return 1;
	}

	// ---- creative inventory edits ------------------------------------------
	if(ContainerSetSlotPacket* p = dynamic_cast<ContainerSetSlotPacket*>(pk)) {
		if(this->gamemode != 1) {
			return 1; // only creative mode may set slots directly
		}
		int32_t javaSlot;
		if(p->field_D == 120) {
			if(p->field_E < 0 || p->field_E > 3) {
				return 1;
			}
			javaSlot = 5 + p->field_E; // helmet first, same order as m8
		} else if(p->field_D == 0) {
			/*
			 * Container 0 is the player's own inventory menu, and its slot
			 * numbers are menu indices, not container indices: InventoryMenu
			 * enumerates container slots 9.. as menu slots 0.., which is why
			 * ServerSideNetworkHandler stores into `field_E + 9`.
			 */
			javaSlot = javaSlotForM8Container(p->field_E + 9);
			if(javaSlot < 0) {
				return 1;
			}
		} else {
			return 1;
		}
		JavaByteBuf out;
		this->beginPacket(&out, 0x10); // Creative Inventory Action
		out.writeShort((int16_t)javaSlot);
		javaWriteStack(&out, p->field_10);
		this->send(out);
		return 1;
	}

	// Everything else is MCPE-only bookkeeping the Java server neither needs
	// nor understands (chunk requests, ready handshakes, entity links, health
	// echoes). Consume it silently.
	return 1;
}
