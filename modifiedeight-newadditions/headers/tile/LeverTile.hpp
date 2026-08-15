#pragma once
#include <tile/Tile.hpp>

class LeverTile : public Tile {
public:
    LeverTile(int32_t id);
    virtual ~LeverTile();
    virtual bool_t isCubeShaped();
    virtual bool_t isSolidRender();
    virtual AABB* getAABB(Level* level, int32_t x, int32_t y, int32_t z);
    virtual void updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z);
    virtual void updateDefaultShape();
    virtual int32_t getRenderShape();
    virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z);
    virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z, int32_t side);
    virtual int32_t getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz, Mob* mob, int32_t meta);
    virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId);
    virtual bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player);
    virtual void onRemove(Level* level, int32_t x, int32_t y, int32_t z);
    virtual bool_t isSignalSource();
    virtual int32_t getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side);
    virtual int32_t getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side);
};
