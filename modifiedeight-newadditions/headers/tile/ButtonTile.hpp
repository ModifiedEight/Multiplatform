#pragma once
#include <tile/Tile.hpp>
#include <string>

class ButtonTile : public Tile {
public:
    std::string textureName;
    int32_t textureAux;
    TextureUVCoordinateSet textureUV;
    int32_t delayTicks;

    ButtonTile(int32_t id, const std::string& texName, int32_t texAux, const Material* mat, int32_t delay = 20);
    virtual ~ButtonTile();
    virtual bool_t isCubeShaped() override;
    virtual bool_t isSolidRender() override;
    virtual AABB* getAABB(Level* level, int32_t x, int32_t y, int32_t z) override;
    virtual void updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) override;
    virtual void updateDefaultShape() override;
    virtual int32_t getRenderShape() override;
    virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z) override;
    virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side) override;
    virtual int32_t getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz, Mob* mob, int32_t meta) override;
    virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId) override;
    virtual void entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity) override;
    virtual bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) override;
    virtual void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* rand) override;
    virtual void onRemove(Level* level, int32_t x, int32_t y, int32_t z) override;
    virtual bool_t isSignalSource() override;
    virtual int32_t getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) override;
    virtual int32_t getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) override;
    virtual TextureUVCoordinateSet* getTexture(int32_t side, int32_t data) override;
    virtual TextureUVCoordinateSet* getTexture(int32_t side) override;
    virtual TextureUVCoordinateSet* getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) override;
    virtual TextureUVCoordinateSet* getCarriedTexture(int32_t side, int32_t data) override;
};
