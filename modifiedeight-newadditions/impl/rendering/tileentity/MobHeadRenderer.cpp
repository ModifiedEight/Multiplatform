#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <tile/entity/MobHeadTileEntity.hpp>
#include <rendering/TileEntityRenderDispatcher.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/EntityRenderer.hpp>
#include <rendering/Textures.hpp>
#include <level/Level.hpp>
#include <unigl.h>

MobHeadRenderer* MobHeadRenderer::instance = nullptr;

static const char* _headTextures[] = {
	"mob/char.png",
	"mob/creeper.png",
	"mob/zombie.png",
	"mob/skeleton.png",
	"mob/spider.png",
	"mob/pigzombie.png",
	"mob/slime.png",
	"mob/cow.png",
	"mob/pig.png",
	"mob/sheep_0.png",
	"mob/chicken.png",
	"mob/villager.png",
	"mob/ocelot.png",
	"mob/polarbear.png",
	"mob/big_sea_turtle.png",
	"mob/zombie.png",
	"mob/wolf.png",
	"mob/fox.png"
};

const char* MobHeadRenderer::getTexturePath(int32_t headType) {
	if (headType < 0 || headType >= 18) return _headTextures[0];
	return _headTextures[headType];
}

MobHeadRenderer::MobHeadRenderer()
	: TileEntityRenderer() {
	MobHeadRenderer::instance = this;

	headParts[0]._init(64, 32); headParts[0].texOffs(0, 0); headParts[0].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[1]._init(64, 32); headParts[1].texOffs(0, 0); headParts[1].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[2]._init(64, 32); headParts[2].texOffs(0, 0); headParts[2].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[3]._init(64, 32); headParts[3].texOffs(0, 0); headParts[3].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[4]._init(64, 32); headParts[4].texOffs(32, 4); headParts[4].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[5]._init(64, 32); headParts[5].texOffs(0, 0); headParts[5].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[6]._init(64, 32); headParts[6].texOffs(0, 0); headParts[6].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8); headParts[6].texOffs(0, 16); headParts[6].addBox(-3.0f, -3.0f, -3.0f, 6, 6, 6);
	headParts[7]._init(64, 32); headParts[7].texOffs(0, 0); headParts[7].addBox(-4.0f, -4.0f, -3.0f, 8, 8, 6); headParts[7].texOffs(22, 0); headParts[7].addBox(-5.0f, -5.0f, -1.0f, 1, 3, 1); headParts[7].texOffs(22, 0); headParts[7].addBox(4.0f, -5.0f, -1.0f, 1, 3, 1);
	headParts[8]._init(64, 32); headParts[8].texOffs(0, 0); headParts[8].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8); headParts[8].texOffs(16, 16); headParts[8].addBox(-2.0f, 0.0f, -5.0f, 4, 3, 1);
	headParts[9]._init(64, 64); headParts[9].texOffs(0, 0); headParts[9].addBox(-3.0f, -3.0f, -4.0f, 6, 6, 8); headParts[9].texOffs(0, 32); headParts[9].addBox(-3.0f, -3.0f, -2.0f, 6, 6, 6, 0.6f);
	headParts[10]._init(64, 32); headParts[10].texOffs(0, 0); headParts[10].addBox(-2.0f, -3.0f, -1.5f, 4, 6, 3); headParts[10].texOffs(14, 0); headParts[10].addBox(-2.0f, -1.0f, -3.5f, 4, 2, 2); headParts[10].texOffs(14, 4); headParts[10].addBox(-1.0f, 1.0f, -2.5f, 2, 2, 2);
	headParts[11]._init(64, 64); headParts[11].texOffs(0, 0); headParts[11].addBox(-4.0f, -5.0f, -4.0f, 8, 10, 8); headParts[11].texOffs(24, 0); headParts[11].addBox(-1.0f, 1.5f, -6.0f, 2, 4, 2);
	headParts[12]._init(64, 32); headParts[12].texOffs(0, 0); headParts[12].addBox(-2.5f, -2.0f, -2.5f, 5, 4, 5); headParts[12].texOffs(1, 24); headParts[12].addBox(-1.5f, 0.0f, -3.5f, 3, 2, 1); headParts[12].texOffs(1, 17); headParts[12].addBox(-2.0f, -3.0f, -1.0f, 1, 1, 2); headParts[12].texOffs(1, 17); headParts[12].addBox(1.0f, -3.0f, -1.0f, 1, 1, 2);
	headParts[13]._init(128, 64); headParts[13].texOffs(0, 0); headParts[13].addBox(-3.5f, -3.5f, -3.5f, 7, 7, 7); headParts[13].texOffs(0, 44); headParts[13].addBox(-2.5f, 0.5f, -6.5f, 5, 3, 3); headParts[13].texOffs(26, 0); headParts[13].addBox(-4.5f, -4.5f, -1.5f, 2, 2, 1); headParts[13].texOffs(26, 0); headParts[13].addBox(2.5f, -4.5f, -1.5f, 2, 2, 1);
	headParts[14]._init(128, 64); headParts[14].texOffs(3, 0); headParts[14].addBox(-3.0f, -2.5f, -3.0f, 6, 5, 6);
	headParts[15]._init(64, 32); headParts[15].texOffs(0, 0); headParts[15].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
	headParts[16]._init(64, 32); headParts[16].texOffs(0, 0); headParts[16].addBox(-3.0f, -3.0f, -2.0f, 6, 6, 4); headParts[16].texOffs(16, 14); headParts[16].addBox(-3.0f, -5.0f, 0.0f, 2, 2, 1); headParts[16].texOffs(16, 14); headParts[16].addBox(1.0f, -5.0f, 0.0f, 2, 2, 1); headParts[16].texOffs(0, 10); headParts[16].addBox(-1.5f, 0.0f, -5.0f, 3, 3, 4);
	headParts[17]._init(48, 32); headParts[17].texOffs(1, 5); headParts[17].addBox(-4.0f, -3.0f, -3.0f, 8, 6, 6); headParts[17].texOffs(8, 1); headParts[17].addBox(-4.0f, -5.0f, -2.0f, 2, 2, 1); headParts[17].texOffs(15, 1); headParts[17].addBox(2.0f, -5.0f, -2.0f, 2, 2, 1); headParts[17].texOffs(6, 18); headParts[17].addBox(-2.0f, 1.0f, -6.0f, 4, 2, 3);
}

MobHeadRenderer::~MobHeadRenderer() {
}

void MobHeadRenderer::renderHead(int32_t headType, float x, float y, float z, float rotAngle, float scale) {
	if (headType < 0 || headType >= 18) headType = 0;
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(180.0f - rotAngle, 0.0f, 1.0f, 0.0f);
	glScalef(-1.0f, -1.0f, 1.0f);
	const char* tex = getTexturePath(headType);
	if (this->dispatcher && this->dispatcher->textures) {
		this->dispatcher->textures->loadAndBindTexture(tex);
	} else if (EntityRenderer::entityRenderDispatcher && EntityRenderer::entityRenderDispatcher->textures) {
		EntityRenderer::entityRenderDispatcher->textures->loadAndBindTexture(tex);
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glDisable(GL_CULL_FACE);
	headParts[headType].render(scale);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}

void MobHeadRenderer::render(TileEntity* te, float x, float y, float z, float a6, bool_t graphics) {
	MobHeadTileEntity* head = (MobHeadTileEntity*)te;
	if (!head) return;
	te->setupLighting(graphics, a6);
	int meta = 1;
	if (head->level) {
		meta = head->level->getData(head->posX, head->posY, head->posZ);
	}
	float rot = 0.0f;
	float px = x + 0.5f;
	float py = y + 0.25f;
	float pz = z + 0.5f;
	if (meta == 1 || meta == 0) {
		rot = (float)head->rotation * 22.5f;
	} else if (meta == 2) {
		// South wall - head faces north (toward player who placed from south)
		rot = 180.0f;
		pz += 0.25f;
		py += 0.25f;
	} else if (meta == 3) {
		// North wall - head faces south (toward player who placed from north)
		rot = 0.0f;
		pz -= 0.25f;
		py += 0.25f;
	} else if (meta == 4) {
		// East wall - head faces west (toward player who placed from east)
		rot = 90.0f;
		px += 0.25f;
		py += 0.25f;
	} else if (meta == 5) {
		// West wall - head faces east (toward player who placed from west)
		rot = 270.0f;
		px -= 0.25f;
		py += 0.25f;
	} else {
		rot = (float)(meta & 15) * 22.5f;
	}
	this->renderHead(head->headType, px, py, pz, rot, 0.0625f);
}
