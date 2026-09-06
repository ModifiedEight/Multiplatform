#include <gui/screens/VillagerTradeScreen.hpp>
#include <Minecraft.hpp>
#include <entity/Player.hpp>
#include <entity/Villager.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/entity/ItemRenderer.hpp>
#include <gui/buttons/Button.hpp>

static const int32_t GW = 176;
static const int32_t GH = 166;

VillagerTradeScreen::VillagerTradeScreen(Player* p, Villager* v)
	: player(p), villager(v), selectedTrade(0), scrollOffset(0), guiX(0), guiY(0), lastSourceSlot(-1) {
}

VillagerTradeScreen::~VillagerTradeScreen() {
}

void VillagerTradeScreen::init() {
	this->buttons.clear();
	this->guiX = (this->width - GW) / 2;
	this->guiY = (this->height - GH) / 2;
}

void VillagerTradeScreen::tick() {
	if (!this->villager || this->villager->isDead) {
		this->minecraft->setScreen(0);
		return;
	}
	if (this->player && this->player->distanceTo(this->villager) > 6.5f) {
		this->minecraft->setScreen(0);
	}
}

void VillagerTradeScreen::removed() {
	if (this->player && this->player->inventory && !this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
		bool isCreative = this->player->inventory->field_20 != 0;
		if (!isCreative) {
			if (this->lastSourceSlot >= 0 && this->lastSourceSlot < this->player->inventory->getContainerSize() &&
			    !this->player->inventory->getItem(this->lastSourceSlot)) {
				this->player->inventory->setItem(this->lastSourceSlot, &this->placedInputItem);
			} else if (!this->player->inventory->add(&this->placedInputItem)) {
				this->player->drop(&this->placedInputItem, 0);
			}
			this->player->inventory->setContainerChanged();
		}
		this->placedInputItem = ItemInstance();
	}
}

bool_t VillagerTradeScreen::renderGameBehind() { return 1; }
bool_t VillagerTradeScreen::isInGameScreen() { return 0; }

static bool doTrade(Player* player, Villager* villager, int32_t idx, ItemInstance* placedSlot, int32_t sourceSlot) {
	if (!player || !player->inventory || !villager) return false;
	if (idx < 0 || idx >= villager->tradeCount) return false;
	VillagerTrade& tr = villager->trades[idx];

	bool isCreative = player->inventory->field_20 != 0;
	if (isCreative) {
		ItemInstance out(tr.outputId, tr.outputCount, tr.outputMeta);
		int32_t targetSlot = (sourceSlot >= 0 && sourceSlot < 9) ? sourceSlot : player->inventory->selectedSlot;
		if (targetSlot >= 0 && targetSlot < 9) {
			player->inventory->setItem(targetSlot, &out);
		} else {
			player->inventory->add(&out);
		}
		player->inventory->setContainerChanged();
		if (placedSlot) *placedSlot = ItemInstance();
		return true;
	}

	if (placedSlot && !placedSlot->isNull() && placedSlot->getId() == tr.inputId &&
	    (tr.inputMeta == 0 || placedSlot->getAuxValue() == tr.inputMeta) &&
	    placedSlot->count >= tr.inputCount) {
		placedSlot->count -= tr.inputCount;
		if (placedSlot->count <= 0) *placedSlot = ItemInstance();

		ItemInstance out(tr.outputId, tr.outputCount, tr.outputMeta);
		if (sourceSlot >= 0 && sourceSlot < player->inventory->getContainerSize() &&
		    !player->inventory->getItem(sourceSlot)) {
			player->inventory->setItem(sourceSlot, &out);
		} else if (!player->inventory->add(&out)) {
			player->drop(&out, 0);
		}
		player->inventory->setContainerChanged();
		return true;
	}

	int32_t have = 0;
	int32_t sz = player->inventory->getContainerSize();
	for (int32_t i = 0; i < sz; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		if (s && !s->isNull() && s->getId() == tr.inputId &&
		    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta))
			have += s->count;
	}
	if (have < tr.inputCount) return false;

	int32_t rem = tr.inputCount;
	int32_t firstEmptySlot = -1;
	for (int32_t i = 0; i < sz && rem > 0; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		if (s && !s->isNull() && s->getId() == tr.inputId &&
		    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta)) {
			int32_t take = s->count < rem ? s->count : rem;
			s->count -= take;
			rem -= take;
			if (s->count <= 0) {
				player->inventory->setItem(i, 0);
				if (firstEmptySlot < 0) firstEmptySlot = i;
			}
		}
	}

	ItemInstance out(tr.outputId, tr.outputCount, tr.outputMeta);
	if (!player->inventory->add(&out)) {
		player->drop(&out, 0);
	}
	if (placedSlot) *placedSlot = ItemInstance();
	player->inventory->setContainerChanged();
	return true;
}

void VillagerTradeScreen::buttonClicked(Button* btn) {
}

void VillagerTradeScreen::mouseClicked(int32_t mx, int32_t my, int32_t btn) {
	int32_t closeX = this->guiX + GW - 21;
	int32_t closeY = this->guiY + 5;
	if (mx >= closeX - 2 && mx <= closeX + 18 && my >= closeY - 2 && my <= closeY + 18) {
		this->minecraft->setScreen(0);
		return;
	}

	if (mx < this->guiX || mx > this->guiX + GW || my < this->guiY || my > this->guiY + GH) {
		this->minecraft->setScreen(0);
		return;
	}

	if (this->villager) {
		int32_t cnt = this->villager->tradeCount;
		if (cnt <= 0) { this->villager->initTrades(); cnt = this->villager->tradeCount; }

		int32_t prevX = this->guiX + 22;
		int32_t prevY = this->guiY + 18;
		if (mx >= prevX - 3 && mx <= prevX + 14 && my >= prevY - 3 && my <= prevY + 18) {
			if (this->selectedTrade > 0) {
				if (this->player && this->player->inventory && !this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
					if (this->player->inventory->field_20 == 0) {
						if (this->lastSourceSlot >= 0 && this->lastSourceSlot < this->player->inventory->getContainerSize() &&
						    !this->player->inventory->getItem(this->lastSourceSlot)) {
							this->player->inventory->setItem(this->lastSourceSlot, &this->placedInputItem);
						} else {
							this->player->inventory->add(&this->placedInputItem);
						}
					}
					this->placedInputItem = ItemInstance();
				}
				this->selectedTrade--;
				if (this->minecraft->level)
					this->minecraft->level->playSound(this->villager, "mob.villager.haggle", 1.0f, 1.0f);
			}
			return;
		}

		int32_t nextX = this->guiX + 138;
		int32_t nextY = this->guiY + 18;
		if (mx >= nextX - 3 && mx <= nextX + 14 && my >= nextY - 3 && my <= nextY + 18) {
			if (this->selectedTrade < cnt - 1) {
				if (this->player && this->player->inventory && !this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
					if (this->player->inventory->field_20 == 0) {
						if (this->lastSourceSlot >= 0 && this->lastSourceSlot < this->player->inventory->getContainerSize() &&
						    !this->player->inventory->getItem(this->lastSourceSlot)) {
							this->player->inventory->setItem(this->lastSourceSlot, &this->placedInputItem);
						} else {
							this->player->inventory->add(&this->placedInputItem);
						}
					}
					this->placedInputItem = ItemInstance();
				}
				this->selectedTrade++;
				if (this->minecraft->level)
					this->minecraft->level->playSound(this->villager, "mob.villager.haggle", 1.0f, 1.0f);
			}
			return;
		}

		VillagerTrade& tr = this->villager->trades[this->selectedTrade];

		int32_t inSlotX = this->guiX + 36;
		int32_t inSlotY = this->guiY + 53;
		if (mx >= inSlotX && mx <= inSlotX + 18 && my >= inSlotY && my <= inSlotY + 18) {
			if (this->player && this->player->inventory && !this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
				if (this->player->inventory->field_20 == 0) {
					if (this->lastSourceSlot >= 0 && this->lastSourceSlot < this->player->inventory->getContainerSize() &&
					    !this->player->inventory->getItem(this->lastSourceSlot)) {
						this->player->inventory->setItem(this->lastSourceSlot, &this->placedInputItem);
					} else {
						this->player->inventory->add(&this->placedInputItem);
					}
					this->player->inventory->setContainerChanged();
				}
				this->placedInputItem = ItemInstance();
			}
			return;
		}

		int32_t arrowX = this->guiX + 80;
		int32_t arrowY = this->guiY + 50;
		int32_t outX = this->guiX + 116;
		int32_t outY = this->guiY + 49;
		if ((mx >= arrowX && mx <= arrowX + 34 && my >= arrowY && my <= arrowY + 28) ||
		    (mx >= outX && mx <= outX + 32 && my >= outY && my <= outY + 32)) {
			bool ok = doTrade(this->player, this->villager, this->selectedTrade, &this->placedInputItem, this->lastSourceSlot);
			if (this->minecraft->level)
				this->minecraft->level->playSound(this->villager, ok ? "mob.villager.yes" : "mob.villager.no", 1.0f, 1.0f);
			return;
		}

		if (this->player && this->player->inventory) {
			bool isCreative = this->player->inventory->field_20 != 0;
			int32_t clickedSlot = -1;

			for (int32_t col = 0; col < 9; ++col) {
				int32_t sx = this->guiX + 8 + col * 18;
				int32_t sy = this->guiY + 142;
				if (mx >= sx && mx <= sx + 18 && my >= sy && my <= sy + 18) {
					clickedSlot = col;
					break;
				}
			}

			if (clickedSlot < 0 && !isCreative) {
				int32_t sz = this->player->inventory->getContainerSize();
				for (int32_t row = 0; row < 3; ++row) {
					for (int32_t col = 0; col < 9; ++col) {
						int32_t sx = this->guiX + 8 + col * 18;
						int32_t sy = this->guiY + 84 + row * 18;
						if (mx >= sx && mx <= sx + 18 && my >= sy && my <= sy + 18) {
							int32_t idx = 9 + row * 9 + col;
							if (idx < sz) clickedSlot = idx;
							break;
						}
					}
					if (clickedSlot >= 0) break;
				}
			}

			if (clickedSlot >= 0) {
				ItemInstance* s = this->player->inventory->getItem(clickedSlot);
				if (s && !s->isNull() && s->getId() == tr.inputId &&
				    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta)) {
					this->lastSourceSlot = clickedSlot;
					if (isCreative) {
						this->placedInputItem = ItemInstance(tr.inputId, tr.inputCount, tr.inputMeta);
						this->player->inventory->setItem(clickedSlot, 0);
						this->player->inventory->setContainerChanged();
					} else {
						int32_t needed = tr.inputCount - (this->placedInputItem.isNull() ? 0 : this->placedInputItem.count);
						if (needed > 0) {
							int32_t moveCount = s->count < needed ? s->count : needed;
							if (this->placedInputItem.isNull()) {
								this->placedInputItem = ItemInstance(s->getId(), moveCount, s->getAuxValue());
							} else {
								this->placedInputItem.count += moveCount;
							}
							s->count -= moveCount;
							if (s->count <= 0) {
								this->player->inventory->setItem(clickedSlot, 0);
							}
							this->player->inventory->setContainerChanged();
						}
					}
					return;
				}
			}
		}
	}
	Screen::mouseClicked(mx, my, btn);
}

void VillagerTradeScreen::keyPressed(int32_t key) {
	if (key == 27 || key == 6) this->minecraft->setScreen(0);
}

bool_t VillagerTradeScreen::backPressed(Minecraft* mc, bool_t b) {
	if (mc) mc->setScreen(0);
	return 1;
}

static void renderSlotItem(Font* font, Textures* textures, const ItemInstance* inst, float x, float y) {
	if (!inst || inst->isNull() || inst->count <= 0) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	ItemRenderer::renderGuiItemNew(textures, (ItemInstance*)inst, 0, x, y, 1.0f, 1.0f, 1.0f);
	if (inst->count > 1) {
		char buf[16];
		snprintf(buf, sizeof(buf), "%d", inst->count);
		int32_t w = font->width(buf);
		font->drawShadow(buf, (int32_t)(x + 17.0f - (float)w), (int32_t)(y + 9.0f), 0xFFFFFFFF);
	}
}

void VillagerTradeScreen::render(int32_t mx, int32_t my, float f) {
	this->renderBackground(0);

	this->minecraft->texturesPtr->loadAndBindTexture("gui/container/villager.png");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	this->blit((float)this->guiX, (float)this->guiY, 0, 0, 176.0f, 166.0f, 176, 166);

	int32_t closeX = this->guiX + GW - 21;
	int32_t closeY = this->guiY + 5;
	bool closeHov = (mx >= closeX && mx <= closeX + 16 && my >= closeY && my <= closeY + 16);

	this->minecraft->texturesPtr->loadAndBindTexture("gui/gui.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (closeHov) {
		glColor4f(0.75f, 0.75f, 0.75f, 0.85f);
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	}
	float u0 = 236.0f * 0.00390625f;
	float v0 = 64.0f * 0.00390625f;
	float u1 = (236.0f + 18.0f) * 0.00390625f;
	float v1 = (64.0f + 18.0f) * 0.00390625f;
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.vertexUV((float)closeX, (float)(closeY + 16), 0.0f, u0, v1);
	t.vertexUV((float)(closeX + 16), (float)(closeY + 16), 0.0f, u1, v1);
	t.vertexUV((float)(closeX + 16), (float)closeY, 0.0f, u1, v0);
	t.vertexUV((float)closeX, (float)closeY, 0.0f, u0, v0);
	t.draw(1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	int32_t tw = this->font->width("✕");
	this->font->drawShadow("✕", closeX + (16 - tw + 1) / 2, closeY + 4, closeHov ? 0xFFFFA0 : 0xFFFFFF);

	this->font->draw("Villager", this->guiX + 8, this->guiY + 6, 0x404040);
	this->font->draw("Inventory", this->guiX + 8, this->guiY + 72, 0x404040);

	if (this->villager) {
		if (this->villager->tradeCount <= 0) this->villager->initTrades();
		int32_t cnt = this->villager->tradeCount;
		if (this->selectedTrade >= cnt) this->selectedTrade = cnt - 1;
		if (this->selectedTrade < 0) this->selectedTrade = 0;

		char buf[16];
		snprintf(buf, sizeof(buf), "%d / %d", this->selectedTrade + 1, cnt);
		int32_t lw = this->font->width(buf);
		this->font->draw(buf, this->guiX + (GW - lw) / 2, this->guiY + 6, 0x404040);

		VillagerTrade& tr = this->villager->trades[this->selectedTrade];

		int32_t prevX = this->guiX + 22;
		int32_t prevY = this->guiY + 18;
		bool prevHov = (mx >= prevX && mx <= prevX + 11 && my >= prevY && my <= prevY + 15);
		int32_t prevU = (this->selectedTrade > 0) ? (prevHov ? 188 : 176) : 200;

		int32_t nextX = this->guiX + 138;
		int32_t nextY = this->guiY + 18;
		bool nextHov = (mx >= nextX && mx <= nextX + 11 && my >= nextY && my <= nextY + 15);
		int32_t nextU = (this->selectedTrade < cnt - 1) ? (nextHov ? 188 : 176) : 200;

		this->minecraft->texturesPtr->loadAndBindTexture("gui/container/villager.png");
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		this->blit((float)prevX, (float)prevY, prevU, 21, 11.0f, 15.0f, 11, 15);
		this->blit((float)nextX, (float)nextY, nextU, 0, 11.0f, 15.0f, 11, 15);

		ItemInstance inInst(tr.inputId, tr.inputCount, tr.inputMeta);
		ItemInstance outInst(tr.outputId, tr.outputCount, tr.outputMeta);

		float topInX = (float)(this->guiX + 37);
		float topInY = (float)(this->guiY + 18);
		renderSlotItem(this->font, this->minecraft->texturesPtr, &inInst, topInX, topInY);

		float topOutX = (float)(this->guiX + 110);
		float topOutY = (float)(this->guiY + 18);
		renderSlotItem(this->font, this->minecraft->texturesPtr, &outInst, topOutX, topOutY);

		int32_t have = (!this->placedInputItem.isNull() && this->placedInputItem.getId() == tr.inputId &&
		                (tr.inputMeta == 0 || this->placedInputItem.getAuxValue() == tr.inputMeta)) ? this->placedInputItem.count : 0;
		bool isCreative = (this->player && this->player->inventory && this->player->inventory->field_20 != 0);
		if (isCreative) {
			have = tr.inputCount;
		} else if (this->player && this->player->inventory) {
			int32_t sz = this->player->inventory->getContainerSize();
			for (int32_t i = 0; i < sz; ++i) {
				ItemInstance* s = this->player->inventory->getItem(i);
				if (s && !s->isNull() && s->getId() == tr.inputId &&
				    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta))
					have += s->count;
			}
		}

		if (!this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
			float slotInX = (float)(this->guiX + 37);
			float slotInY = (float)(this->guiY + 54);
			renderSlotItem(this->font, this->minecraft->texturesPtr, &this->placedInputItem, slotInX, slotInY);
		}

		if (have < tr.inputCount) {
			this->minecraft->texturesPtr->loadAndBindTexture("gui/container/villager.png");
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			this->blit((float)(this->guiX + 83), (float)(this->guiY + 53), 212, 0, 28.0f, 21.0f, 28, 21);
		} else {
			float slotOutX = (float)(this->guiX + 121);
			float slotOutY = (float)(this->guiY + 54);
			renderSlotItem(this->font, this->minecraft->texturesPtr, &outInst, slotOutX, slotOutY);
		}
	}

	if (this->player && this->player->inventory) {
		bool isCreative = this->player->inventory->field_20 != 0;
		if (!isCreative) {
			int32_t sz = this->player->inventory->getContainerSize();
			for (int32_t row = 0; row < 3; ++row) {
				for (int32_t col = 0; col < 9; ++col) {
					int32_t idx = 9 + row * 9 + col;
					if (idx >= sz) continue;
					ItemInstance* s = this->player->inventory->getItem(idx);
					if (!s || s->isNull()) continue;
					float sx = (float)(this->guiX + 8 + col * 18 + 1);
					float sy = (float)(this->guiY + 84 + row * 18 + 1);
					renderSlotItem(this->font, this->minecraft->texturesPtr, s, sx, sy);
				}
			}
		}
		for (int32_t col = 0; col < 9; ++col) {
			ItemInstance* s = this->player->inventory->getItem(col);
			if (!s || s->isNull()) continue;
			float sx = (float)(this->guiX + 8 + col * 18 + 1);
			float sy = (float)(this->guiY + 143);
			renderSlotItem(this->font, this->minecraft->texturesPtr, s, sx, sy);
		}
	}

	Screen::render(mx, my, f);
}
