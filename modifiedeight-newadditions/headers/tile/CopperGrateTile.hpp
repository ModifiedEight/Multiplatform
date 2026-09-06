#pragma once
#include <tile/Tile.hpp>

class CopperGrateTile : public Tile {
public:
    CopperGrateTile(int32_t id);
    virtual ~CopperGrateTile();
    virtual bool_t isSolidRender();
    virtual bool_t isCubeShaped();
    virtual int32_t getRenderLayer();
    virtual bool_t blocksLight();
};
