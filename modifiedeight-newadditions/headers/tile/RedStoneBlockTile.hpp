#pragma once
#include <tile/Tile.hpp>

struct RedStoneBlockTile : public Tile {
  RedStoneBlockTile(int32_t id);
  virtual ~RedStoneBlockTile();

  virtual bool_t isSignalSource() override;
  virtual int32_t getSignal(LevelSource *, int32_t, int32_t, int32_t,
                            int32_t) override;
  virtual int32_t getDirectSignal(Level *, int32_t, int32_t, int32_t,
                                  int32_t) override;
};
