#include <rendering/TextureAtlas.hpp>
#include <NinecraftApp.hpp>
#include <_AssetFile.hpp>
#include <json/reader.h>
#include <cstdio>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <stb_image.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t* g_terrainAtlasPixels;
extern int g_terrainAtlasWidth;
extern int g_terrainAtlasHeight;
extern uint8_t* g_itemsAtlasPixels;
extern int g_itemsAtlasWidth;
extern int g_itemsAtlasHeight;

uint8_t* g_terrainAtlasPixels = nullptr;
int g_terrainAtlasWidth = 1024;
int g_terrainAtlasHeight = 1024;
uint8_t* g_itemsAtlasPixels = nullptr;
int g_itemsAtlasWidth = 512;
int g_itemsAtlasHeight = 512;

static uint8_t* load_and_resize_png(struct NinecraftApp* mc, const std::string& path, int target_w, int target_h) {
	int w, h, channels;
	uint8_t* pixels = nullptr;
	AssetFile file = mc->platform()->readAssetFile(path);
	if (file.bytes && file.length > 0) {
		pixels = stbi_load_from_memory(file.bytes, file.length, &w, &h, &channels, STBI_rgb_alpha);
		delete[] file.bytes;
	}
	if (!pixels) {
		return nullptr;
	}
	if (w == target_w && h == target_h) {
		return pixels;
	}
	uint8_t* resized = (uint8_t*)malloc(target_w * target_h * 4);
	for (int y = 0; y < target_h; ++y) {
		for (int x = 0; x < target_w; ++x) {
			int src_x = x * w / target_w;
			int src_y = y * h / target_h;
			int dst_idx = (y * target_w + x) * 4;
			int src_idx = (src_y * w + src_x) * 4;
			resized[dst_idx + 0] = pixels[src_idx + 0];
			resized[dst_idx + 1] = pixels[src_idx + 1];
			resized[dst_idx + 2] = pixels[src_idx + 2];
			resized[dst_idx + 3] = pixels[src_idx + 3];
		}
	}
	stbi_image_free(pixels);
	return resized;
}

static void paste_texture(uint8_t* atlas_pixels, int atlas_w, const uint8_t* tex_pixels, int grid_x, int grid_y, int tex_w, int tex_h) {
	if (!atlas_pixels || !tex_pixels) return;
	for (int y = 0; y < tex_h; ++y) {
		for (int x = 0; x < tex_w; ++x) {
			int dst_x = grid_x + x;
			int dst_y = grid_y + y;
			int dst_idx = (dst_y * atlas_w + dst_x) * 4;
			int src_idx = (y * tex_w + x) * 4;
			atlas_pixels[dst_idx + 0] = tex_pixels[src_idx + 0];
			atlas_pixels[dst_idx + 1] = tex_pixels[src_idx + 1];
			atlas_pixels[dst_idx + 2] = tex_pixels[src_idx + 2];
			atlas_pixels[dst_idx + 3] = tex_pixels[src_idx + 3];
		}
	}
}

TextureAtlas::TextureAtlas(const std::string& s){
	this->path = s;
}

TextureUVCoordinateSet TextureAtlas::_parseJSON(const Json::Value& v){
	float v17 = (v[2].asFloat() - v[0].asFloat()) * 0.002f;
	float v18 = (v[3].asFloat() - v[1].asFloat()) * 0.002f;

	TextureUVCoordinateSet t;
	t.width = v[4].asFloat();
	t.height = v[5].asFloat();
	t.minX = v[0].asFloat() + v17;
	t.maxX = v[2].asFloat() - v17;
	t.minY = v[1].asFloat() + v18;
	t.maxY = v[3].asFloat() - v18;
	return t;
}

TextureAtlasTextureItem* TextureAtlas::getTextureItem(const std::string& s){
	return &this->field_4[s];
}

void TextureAtlas::load(struct NinecraftApp* mc) {
	bool is_terrain = (this->path.find("terrain.meta") != std::string::npos);
	int atlas_w = is_terrain ? g_terrainAtlasWidth : g_itemsAtlasWidth;
	int atlas_h = is_terrain ? g_terrainAtlasHeight : g_itemsAtlasHeight;
	uint8_t* atlas_pixels = (uint8_t*)malloc(atlas_w * atlas_h * 4);
	memset(atlas_pixels, 0, atlas_w * atlas_h * 4);

	if (is_terrain) {
		if (g_terrainAtlasPixels) {
			free(g_terrainAtlasPixels);
		}
		g_terrainAtlasPixels = atlas_pixels;
	} else {
		if (g_itemsAtlasPixels) {
			free(g_itemsAtlasPixels);
		}
		g_itemsAtlasPixels = atlas_pixels;
	}

	Json::Reader reader;
	Json::Value root;
	AssetFile file = mc->platform()->readAssetFile(this->path);
	std::string data;
	if (file.bytes && file.length > 0) {
		data.assign((char_t*)file.bytes, file.length);
		delete[] file.bytes;
	}

	if (reader.parse(data, root, 1)) {
		int slot_idx = 0;
		int slots_per_row = atlas_w / 16;

		Json::Value::iterator v27 = root.begin();
		Json::Value::iterator v28 = root.end();

		while (v27 != v28) {
			Json::Value v32(*v27);
			std::string name = v32["name"].asString();
			std::vector<TextureUVCoordinateSet> v29;

			std::string base_path = is_terrain ? "textures/blocks/" : "textures/items/";
			std::string main_img_path = base_path + name + ".png";
			uint8_t* main_pixels = load_and_resize_png(mc, main_img_path, 16, 16);
			if (!main_pixels) {
				main_img_path = base_path + name + "_0.png";
				main_pixels = load_and_resize_png(mc, main_img_path, 16, 16);
			}

			int main_x = (slot_idx % slots_per_row) * 16;
			int main_y = (slot_idx / slots_per_row) * 16;
			slot_idx++;

			if (main_pixels) {
				paste_texture(atlas_pixels, atlas_w, main_pixels, main_x, main_y, 16, 16);
				free(main_pixels);
			}

			float main_min_x = main_x / (float)atlas_w;
			float main_max_x = (main_x + 16) / (float)atlas_w;
			float main_min_y = main_y / (float)atlas_h;
			float main_max_y = (main_y + 16) / (float)atlas_h;
			float main_inset_x = (main_max_x - main_min_x) * 0.002f;
			float main_inset_y = (main_max_y - main_min_y) * 0.002f;

			TextureUVCoordinateSet main_uv;
			main_uv.width = atlas_w;
			main_uv.height = atlas_h;
			main_uv.minX = main_min_x + main_inset_x;
			main_uv.maxX = main_max_x - main_inset_x;
			main_uv.minY = main_min_y + main_inset_y;
			main_uv.maxY = main_max_y - main_inset_y;

			if (v32.isMember("additonal_textures") && v32["additonal_textures"].isArray()) {
				Json::Value v6 = v32["additonal_textures"];
				int add_count = v6.size();
				for (int i = 0; i < add_count; ++i) {
					char num_buf[16];
					sprintf(num_buf, "%d", i + 1);
					std::string add_img_path = base_path + name + "_" + num_buf + ".png";
					uint8_t* add_pixels = load_and_resize_png(mc, add_img_path, 16, 16);
					if (!add_pixels) {
						sprintf(num_buf, "%d", i);
						add_img_path = base_path + name + "_" + num_buf + ".png";
						add_pixels = load_and_resize_png(mc, add_img_path, 16, 16);
					}

					int add_x = (slot_idx % slots_per_row) * 16;
					int add_y = (slot_idx / slots_per_row) * 16;
					slot_idx++;

					if (add_pixels) {
						paste_texture(atlas_pixels, atlas_w, add_pixels, add_x, add_y, 16, 16);
						free(add_pixels);
					}

					float add_min_x = add_x / (float)atlas_w;
					float add_max_x = (add_x + 16) / (float)atlas_w;
					float add_min_y = add_y / (float)atlas_h;
					float add_max_y = (add_y + 16) / (float)atlas_h;
					float add_inset_x = (add_max_x - add_min_x) * 0.002f;
					float add_inset_y = (add_max_y - add_min_y) * 0.002f;

					TextureUVCoordinateSet add_uv;
					add_uv.width = atlas_w;
					add_uv.height = atlas_h;
					add_uv.minX = add_min_x + add_inset_x;
					add_uv.maxX = add_max_x - add_inset_x;
					add_uv.minY = add_min_y + add_inset_y;
					add_uv.maxY = add_max_y - add_inset_y;

					v29.emplace_back(add_uv);
				}
			}

			this->field_4[name] = TextureAtlasTextureItem(name, main_uv, v29);
			++v27;
		}
	}
}
