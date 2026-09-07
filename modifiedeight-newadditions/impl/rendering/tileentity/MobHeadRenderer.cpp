#include <level/Level.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/Textures.hpp>
#include <rendering/TileEntityRenderDispatcher.hpp>
#include <rendering/entity/EntityRenderer.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <tile/entity/MobHeadTileEntity.hpp>
#include <unigl.h>

#include <rendering/Tesselator.hpp>
#include <NinecraftApp.hpp>

MobHeadRenderer *MobHeadRenderer::instance = nullptr;

static const char *_headTextures[] = {
    "mob/char.png",           "mob/creeper.png",   "mob/zombie.png",
    "mob/skeleton.png",       "mob/spider.png",    "mob/pigzombie.png",
    "mob/slime.png",          "mob/cow.png",       "mob/pig.png",
    "mob/sheep_0.png",        "mob/chicken.png",   "mob/villager.png",
    "mob/ocelot.png",         "mob/polarbear.png", "mob/big_sea_turtle.png",
    "mob/char.png",           "mob/wolf.png",      "mob/fox.png"};

const char *MobHeadRenderer::getTexturePath(int32_t headType) {
  if (headType < 0 || headType >= 18)
    return _headTextures[0];
  return _headTextures[headType];
}

void MobHeadRenderer::render2DFace(Textures* textures, int32_t headType, float x, float y, float size, float alpha) {
  if (!textures) return;
  if (headType < 0 || headType >= 18) headType = 0;
  textures->loadAndBindTexture(getTexturePath(headType));

  struct FaceData {
    float u0, v0, u1, v1;
    float hat_u0, hat_v0, hat_u1, hat_v1;
    bool hasHat;
    float aspectW, aspectH;
  };

  static const FaceData uvs[18] = {
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 40.0f/64.0f, 8.0f/32.0f, 48.0f/64.0f, 16.0f/32.0f, true,  1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 40.0f/64.0f, 12.0f/32.0f, 48.0f/64.0f, 20.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 40.0f/64.0f, 8.0f/32.0f, 48.0f/64.0f, 16.0f/32.0f, true,  1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 6.0f/64.0f,  6.0f/32.0f, 14.0f/64.0f, 14.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 8.0f/64.0f,  8.0f/64.0f, 14.0f/64.0f, 14.0f/64.0f, 6.0f/64.0f, 38.0f/64.0f, 12.0f/64.0f, 44.0f/64.0f, true,  1.0f, 1.0f },
    { 3.0f/64.0f,  3.0f/32.0f,  7.0f/64.0f,  9.0f/32.0f, 0, 0, 0, 0, false, 0.67f, 1.0f },
    { 8.0f/64.0f,  8.0f/64.0f, 16.0f/64.0f, 18.0f/64.0f, 0, 0, 0, 0, false, 0.8f, 1.0f },
    { 5.0f/64.0f,  5.0f/32.0f, 10.0f/64.0f,  9.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 0.8f },
    { 7.0f/128.0f, 7.0f/64.0f, 14.0f/128.0f, 14.0f/64.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 9.0f/128.0f, 6.0f/64.0f, 15.0f/128.0f, 11.0f/64.0f, 0, 0, 0, 0, false, 1.0f, 0.83f },
    { 8.0f/64.0f,  8.0f/32.0f, 16.0f/64.0f, 16.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 4.0f/64.0f,  4.0f/32.0f, 10.0f/64.0f, 10.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 1.0f },
    { 7.0f/48.0f, 11.0f/32.0f, 15.0f/48.0f, 17.0f/32.0f, 0, 0, 0, 0, false, 1.0f, 0.75f },
  };

  const FaceData& f = uvs[headType];
  float drawW = size * f.aspectW;
  float drawH = size * f.aspectH;
  float drawX = x + (size - drawW) * 0.5f;
  float drawY = y + (size - drawH) * 0.5f;

  GLboolean blendWas = glIsEnabled(GL_BLEND);
  GLboolean alphaTestWas = glIsEnabled(GL_ALPHA_TEST);

  glEnable(GL_TEXTURE_2D);
  if (!blendWas) glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (!alphaTestWas) glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1f);
  glColor4f(1.0f, 1.0f, 1.0f, alpha);

  Tesselator& t = Tesselator::instance;
  t.begin(4);
  t.colorABGR(((uint32_t)(alpha * 255.0f) << 24) | 0x00FFFFFF);
  t.vertexUV(drawX,         drawY + drawH, 0.0f, f.u0, f.v1);
  t.vertexUV(drawX + drawW, drawY + drawH, 0.0f, f.u1, f.v1);
  t.vertexUV(drawX + drawW, drawY,         0.0f, f.u1, f.v0);
  t.vertexUV(drawX,         drawY,         0.0f, f.u0, f.v0);
  t.draw(1);

  if (f.hasHat) {
    t.begin(4);
    t.colorABGR(((uint32_t)(alpha * 255.0f) << 24) | 0x00FFFFFF);
    t.vertexUV(drawX,         drawY + drawH, 0.0f, f.hat_u0, f.hat_v1);
    t.vertexUV(drawX + drawW, drawY + drawH, 0.0f, f.hat_u1, f.hat_v1);
    t.vertexUV(drawX + drawW, drawY,         0.0f, f.hat_u1, f.hat_v0);
    t.vertexUV(drawX,         drawY,         0.0f, f.hat_u0, f.hat_v0);
    t.draw(1);
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  if (!blendWas) glDisable(GL_BLEND);
}

MobHeadRenderer::MobHeadRenderer() : TileEntityRenderer() {
  MobHeadRenderer::instance = this;

  headParts[0]._init(64, 32);
  headParts[0].texOffs(0, 0);
  headParts[0].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[0].texOffs(32, 0);
  headParts[0].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8, 0.5f);
  headParts[1]._init(64, 32);
  headParts[1].texOffs(0, 0);
  headParts[1].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[2]._init(64, 32);
  headParts[2].texOffs(0, 0);
  headParts[2].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[3]._init(64, 32);
  headParts[3].texOffs(0, 0);
  headParts[3].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[4]._init(64, 32);
  headParts[4].texOffs(32, 4);
  headParts[4].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[5]._init(64, 32);
  headParts[5].texOffs(0, 0);
  headParts[5].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[5].texOffs(32, 0);
  headParts[5].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8, 0.5f);
  headParts[6]._init(64, 32);
  headParts[6].texOffs(0, 0);
  headParts[6].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[6].texOffs(0, 16);
  headParts[6].addBox(-3.0f, -3.0f, -3.0f, 6, 6, 6);
  headParts[7]._init(64, 32);
  headParts[7].texOffs(0, 0);
  headParts[7].addBox(-4.0f, -4.0f, -3.0f, 8, 8, 6);
  headParts[7].texOffs(22, 0);
  headParts[7].addBox(-5.0f, -5.0f, -1.0f, 1, 3, 1);
  headParts[7].texOffs(22, 0);
  headParts[7].addBox(4.0f, -5.0f, -1.0f, 1, 3, 1);
  headParts[8]._init(64, 32);
  headParts[8].texOffs(0, 0);
  headParts[8].addBox(-4.0f, -4.0f, -4.0f, 8, 8, 8);
  headParts[8].texOffs(16, 16);
  headParts[8].addBox(-2.0f, 0.0f, -5.0f, 4, 3, 1);
  headParts[9]._init(64, 64);
  headParts[9].texOffs(0, 0);
  headParts[9].addBox(-3.0f, -3.0f, -4.0f, 6, 6, 8);
  headParts[9].texOffs(0, 32);
  headParts[9].addBox(-3.0f, -3.0f, -2.0f, 6, 6, 6, 0.6f);
  headParts[10]._init(64, 32);
  headParts[10].texOffs(0, 0);
  headParts[10].addBox(-2.0f, -3.0f, -1.5f, 4, 6, 3);
  headParts[10].texOffs(14, 0);
  headParts[10].addBox(-2.0f, -1.0f, -3.5f, 4, 2, 2);
  headParts[10].texOffs(14, 4);
  headParts[10].addBox(-1.0f, 1.0f, -2.5f, 2, 2, 2);
  headParts[11]._init(64, 64);
  headParts[11].texOffs(0, 0);
  headParts[11].addBox(-4.0f, -5.0f, -4.0f, 8, 10, 8);
  headParts[11].texOffs(24, 0);
  headParts[11].addBox(-1.0f, 1.5f, -6.0f, 2, 4, 2);
  headParts[12]._init(64, 32);
  headParts[12].texOffs(0, 0);
  headParts[12].addBox(-2.5f, -2.0f, -2.5f, 5, 4, 5);
  headParts[12].texOffs(1, 24);
  headParts[12].addBox(-1.5f, 0.0f, -3.5f, 3, 2, 1);
  headParts[12].texOffs(1, 17);
  headParts[12].addBox(-2.0f, -3.0f, -1.0f, 1, 1, 2);
  headParts[12].texOffs(1, 17);
  headParts[12].addBox(1.0f, -3.0f, -1.0f, 1, 1, 2);
  headParts[13]._init(128, 64);
  headParts[13].texOffs(0, 0);
  headParts[13].addBox(-3.5f, -3.5f, -3.5f, 7, 7, 7);
  headParts[13].texOffs(0, 44);
  headParts[13].addBox(-2.5f, 0.5f, -6.5f, 5, 3, 3);
  headParts[13].texOffs(26, 0);
  headParts[13].addBox(-4.5f, -4.5f, -1.5f, 2, 2, 1);
  headParts[13].texOffs(26, 0);
  headParts[13].addBox(2.5f, -4.5f, -1.5f, 2, 2, 1);
  headParts[14]._init(128, 64);
  headParts[14].texOffs(3, 0);
  headParts[14].addBox(-3.0f, -2.5f, -3.0f, 6, 5, 6);
  headParts[15]._init(128, 128);
  headParts[15].texOffs(0, 0);
  headParts[15].addBox(-4.0f, -5.0f, -4.0f, 8, 10, 8);
  headParts[15].texOffs(24, 0);
  headParts[15].addBox(-1.0f, 1.5f, -6.0f, 2, 4, 2);
  headParts[16]._init(64, 32);
  headParts[16].texOffs(0, 0);
  headParts[16].addBox(-3.0f, -3.0f, -2.0f, 6, 6, 4);
  headParts[16].texOffs(16, 14);
  headParts[16].addBox(-3.0f, -5.0f, 0.0f, 2, 2, 1);
  headParts[16].texOffs(16, 14);
  headParts[16].addBox(1.0f, -5.0f, 0.0f, 2, 2, 1);
  headParts[16].texOffs(0, 10);
  headParts[16].addBox(-1.5f, 0.0f, -5.0f, 3, 3, 4);
  headParts[17]._init(48, 32);
  headParts[17].texOffs(1, 5);
  headParts[17].addBox(-4.0f, -3.0f, -3.0f, 8, 6, 6);
  headParts[17].texOffs(8, 1);
  headParts[17].addBox(-4.0f, -5.0f, -2.0f, 2, 2, 1);
  headParts[17].texOffs(15, 1);
  headParts[17].addBox(2.0f, -5.0f, -2.0f, 2, 2, 1);
  headParts[17].texOffs(6, 18);
  headParts[17].addBox(-2.0f, 1.0f, -6.0f, 4, 2, 3);
}

MobHeadRenderer::~MobHeadRenderer() {}

void MobHeadRenderer::renderGuiHead(Textures* textures, int32_t headType, float x, float y, float scale, float alpha) {
  if (headType < 0 || headType >= 18)
    headType = 0;
  if (!instance)
    return;
  glPushMatrix();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_RESCALE_NORMAL);
  glTranslatef(x + 8.0f * scale, y + 11.5f * scale, 10.0f);
  glScalef(12.0f * scale, 12.0f * scale, 12.0f * scale);
  glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
  glColor4f(1.0f, 1.0f, 1.0f, alpha);
  instance->renderHead(headType, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_RESCALE_NORMAL);
  glPopMatrix();
}

void MobHeadRenderer::renderHead(int32_t headType, float x, float y, float z,
                                 float rotAngle, float scale) {
  if (headType < 0 || headType >= 18)
    headType = 0;
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(180.0f - rotAngle, 0.0f, 1.0f, 0.0f);
  glScalef(-1.0f, -1.0f, 1.0f);
  const char *tex = getTexturePath(headType);
  if (this->dispatcher && this->dispatcher->textures) {
    this->dispatcher->textures->loadAndBindTexture(tex);
  } else if (EntityRenderer::entityRenderDispatcher &&
             EntityRenderer::entityRenderDispatcher->textures) {
    EntityRenderer::entityRenderDispatcher->textures->loadAndBindTexture(tex);
  } else if (NinecraftApp::instance && NinecraftApp::instance->texturesPtr) {
    NinecraftApp::instance->texturesPtr->loadAndBindTexture(tex);
  }
  GLboolean lightingWas = glIsEnabled(GL_LIGHTING);
  GLboolean cullWas = glIsEnabled(GL_CULL_FACE);
  GLboolean blendWas = glIsEnabled(GL_BLEND);
  GLboolean alphaTestWas = glIsEnabled(GL_ALPHA_TEST);
  GLboolean depthTestWas = glIsEnabled(GL_DEPTH_TEST);
  GLboolean textureWas = glIsEnabled(GL_TEXTURE_2D);

  if (!textureWas) glEnable(GL_TEXTURE_2D);
  if (!blendWas) glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (!alphaTestWas) glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1f);
  if (cullWas) glDisable(GL_CULL_FACE);

  headParts[headType].render(scale);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  if (cullWas) glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
  if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  if (!blendWas) glDisable(GL_BLEND);
  if (!depthTestWas) glDisable(GL_DEPTH_TEST);
  if (!textureWas) glDisable(GL_TEXTURE_2D);
  if (lightingWas) glEnable(GL_LIGHTING);
  else glDisable(GL_LIGHTING);

  glPopMatrix();
}

void MobHeadRenderer::render(TileEntity *te, float x, float y, float z,
                             float a6, bool_t graphics) {
  MobHeadTileEntity *head = (MobHeadTileEntity *)te;
  if (!head)
    return;
  te->setupLighting(graphics, a6);
  int meta = 1;
  if (head->level) {
    meta = head->level->getData(head->posX, head->posY, head->posZ);
  }

  static const float headBottomY[18] = {
    0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f,
    0.1875f, 0.1875f, 0.3125f, 0.125f, 0.21875f, 0.15625f, 0.25f, 0.1875f, 0.1875f
  };

  static const float headBackDepth[18] = {
    0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.1875f, 0.25f,
    0.25f, 0.09375f, 0.25f, 0.15625f, 0.21875f, 0.1875f, 0.25f, 0.125f, 0.1875f
  };

  int ht = head->headType;
  if (ht < 0 || ht >= 18) ht = 0;

  float rot = 0.0f;
  float px = x + 0.5f;
  float py = y + headBottomY[ht];
  float pz = z + 0.5f;

  if (meta == 1 || meta == 0) {
    rot = (float)head->rotation * 22.5f;
  } else if (meta == 2) {
    rot = 180.0f;
    pz = z + 1.0f - headBackDepth[ht];
    py = y + 0.25f;
  } else if (meta == 3) {
    rot = 0.0f;
    pz = z + headBackDepth[ht];
    py = y + 0.25f;
  } else if (meta == 4) {
    rot = 90.0f;
    px = x + 1.0f - headBackDepth[ht];
    py = y + 0.25f;
  } else if (meta == 5) {
    rot = 270.0f;
    px = x + headBackDepth[ht];
    py = y + 0.25f;
  } else {
    rot = (float)(meta & 15) * 22.5f;
  }
  this->renderHead(head->headType, px, py, pz, rot, 0.0625f);
}
