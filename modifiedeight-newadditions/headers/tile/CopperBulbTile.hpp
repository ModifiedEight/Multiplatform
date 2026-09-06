#pragma once
#include <tile/Tile.hpp>

class CopperBulbTile : public Tile {
public:
    CopperBulbTile(int32_t id, bool_t isOn);
    virtual ~CopperBulbTile();
    virtual void onPlace(Level* level, int32_t x, int32_t y, int32_t z);
    virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId);
    virtual int32_t getResource(int32_t meta, Random* random);
    virtual int32_t getResourceCount(Random* random);
private:
    bool_t isOn;
};
