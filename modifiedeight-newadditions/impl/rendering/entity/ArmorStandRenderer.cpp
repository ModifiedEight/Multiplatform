#include <entity/ArmorStand.hpp>
#include <item/ArmorItem.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/ArmorStandRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
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

  this->bindTexture("mob/armorstand.png");

  if (this->model) {
    this->model->render(entity, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
  }

  for (int i = 0; i < 4; ++i) {
    ItemInstance *piece = &stand->armorItems[i];
    if (piece && !piece->isNull() && piece->count > 0 &&
        ItemInstance::isArmorItem(piece)) {
      ArmorItem *armor = (ArmorItem *)piece->itemClass;
      if (armor) {
        int texIdx = 2 * armor->field_50 + (i == 2);
        if (texIdx >= 0 && texIdx < 10) {
          this->bindTexture(_armorStandTextures[texIdx]);
        }
        HumanoidModel *hmodel =
            (i == 2) ? _armorStandArmor2 : _armorStandArmor1;
        if (hmodel) {
          hmodel->headModel.field_1D = (i == 0);
          hmodel->bodyModel.field_1D = (i == 1 || i == 2);
          hmodel->rightArmModel.field_1D = (i == 1);
          hmodel->leftArmModel.field_1D = (i == 1);
          hmodel->rightLegModel.field_1D = (i == 2 || i == 3);
          hmodel->leftLegModel.field_1D = (i == 2 || i == 3);
          hmodel->render(stand, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
        }
      }
    }
  }

  glPopMatrix();
}
