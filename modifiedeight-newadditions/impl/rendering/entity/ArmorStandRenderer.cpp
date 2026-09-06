#include <entity/ArmorStand.hpp>
#include <item/ArmorItem.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/ArmorStandRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <tile/MobHeadTile.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <unigl.h>

static std::string _armorStandTextures[] = {
    "armor/cloth_1.png",   "armor/cloth_2.png",   "armor/chain_1.png",
    "armor/chain_2.png",   "armor/iron_1.png",    "armor/iron_2.png",
    "armor/diamond_1.png", "armor/diamond_2.png", "armor/gold_1.png",
    "armor/gold_2.png"};

static HumanoidModel *_armorStandArmor1 = 0;
static HumanoidModel *_armorStandArmor2 = 0;

ArmorStandRenderer::ArmorStandRenderer(Model *model, float shadow)
    : MobRenderer(model, shadow) {
  if (!_armorStandArmor1)
    _armorStandArmor1 = new HumanoidModel(1.0f, 0.0f);
  if (!_armorStandArmor2)
    _armorStandArmor2 = new HumanoidModel(0.5f, 0.0f);
}

ArmorStandRenderer::~ArmorStandRenderer() {}

static void setPartVisible(ModelPart &part, bool visible) {
  part.field_1D = visible ? 1 : 0;
  part.field_4C = visible ? 0 : 1;
}

void ArmorStandRenderer::render(Entity *entity, float x, float y, float z,
                                float rot, float a6) {
  ArmorStand *stand = (ArmorStand *)entity;
  if (!stand)
    return;

  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(180.0f - stand->yaw, 0.0f, 1.0f, 0.0f);
  glScalef(-1.0f, -1.0f, 1.0f);
  glTranslatef(0.0f, -1.5f, 0.0f);

  stand->setupLighting(this->isFancy(), a6);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1f);

  this->bindTexture("mob/armorstand.png");

  if (this->model) {
    this->model->render(entity, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
  }

  for (int i = 0; i < 4; ++i) {
    ItemInstance *piece = &stand->armorItems[i];
    if (piece && !piece->isNull() && piece->count > 0) {
      if (i == 0 && MobHeadTile::isHeadBlock(piece->getId())) {
        int htype = MobHeadTile::getHeadType(piece->getId());
        if (MobHeadRenderer::instance) {
          glPushMatrix();
          glTranslatef(0.0f, 0.0f, 0.0f);
          MobHeadRenderer::instance->renderHead(htype, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
          glPopMatrix();
        }
      } else if (ItemInstance::isArmorItem(piece)) {
        ArmorItem *armor = (ArmorItem *)piece->itemClass;
        if (armor) {
          int texIdx = 2 * armor->field_50 + (i == 2);
          if (texIdx >= 0 && texIdx < 10) {
            this->bindTexture(_armorStandTextures[texIdx]);
          }
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          HumanoidModel *hmodel =
              (i == 2) ? _armorStandArmor2 : _armorStandArmor1;
          if (hmodel) {
            setPartVisible(hmodel->headModel, i == 0);
            setPartVisible(hmodel->bodyModel, i == 1 || i == 2);
            setPartVisible(hmodel->rightArmModel, i == 1);
            setPartVisible(hmodel->leftArmModel, i == 1);
            setPartVisible(hmodel->rightLegModel, i == 2 || i == 3);
            setPartVisible(hmodel->leftLegModel, i == 2 || i == 3);

            hmodel->headModel.xRotAngle = 0.0f;
            hmodel->headModel.yRotAngle = 0.0f;
            hmodel->headModel.zRotAngle = 0.0f;

            hmodel->bodyModel.xRotAngle = 0.0f;
            hmodel->bodyModel.yRotAngle = 0.0f;
            hmodel->bodyModel.zRotAngle = 0.0f;

            hmodel->rightArmModel.xRotAngle = 0.0f;
            hmodel->rightArmModel.yRotAngle = 0.0f;
            hmodel->rightArmModel.zRotAngle = 0.0f;

            hmodel->leftArmModel.xRotAngle = 0.0f;
            hmodel->leftArmModel.yRotAngle = 0.0f;
            hmodel->leftArmModel.zRotAngle = 0.0f;

            hmodel->rightLegModel.xRotAngle = 0.0f;
            hmodel->rightLegModel.yRotAngle = 0.0f;
            hmodel->rightLegModel.zRotAngle = 0.0f;

            hmodel->leftLegModel.xRotAngle = 0.0f;
            hmodel->leftLegModel.yRotAngle = 0.0f;
            hmodel->leftLegModel.zRotAngle = 0.0f;

            glDisable(GL_CULL_FACE);
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.1f);
            hmodel->render(stand, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
            glEnable(GL_CULL_FACE);
          }
        }
      }
    }
  }

  glPopMatrix();
}
