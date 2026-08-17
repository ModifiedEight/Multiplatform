#include <tile/DirtTile.hpp>
#include <level/LevelSource.hpp>
#include <tile/material/Material.hpp>

DirtTile::DirtTile(int32_t id, std::string n, Material* m)
	: Tile(id, n, m) {
	this->field_5C = 255;
}
DirtTile::~DirtTile() {
}

int32_t DirtTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (level && (level->getMaterial(x, y + 1, z) == Material::water ||
	              level->getMaterial(x, y, z) == Material::water)) {
		return 0x88B890;
	}
	return 0xFFFFFF;
}
