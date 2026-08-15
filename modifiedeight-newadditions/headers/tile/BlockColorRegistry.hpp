#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <string>
#include <cstdio>

class BlockColorRegistry {
public:
    static inline std::unordered_map<uint64_t, uint32_t>& getMap() {
        static std::unordered_map<uint64_t, uint32_t> s_colors;
        return s_colors;
    }

    static inline std::mutex& getMutex() {
        static std::mutex s_mutex;
        return s_mutex;
    }

    static void setBlockColor(int32_t x, int32_t y, int32_t z, uint32_t color) {
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        getMap()[key] = color;
    }

    static uint32_t getBlockColor(int32_t x, int32_t y, int32_t z) {
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return 0xFFFFFFFF;
    }

    static bool hasBlockColor(int32_t x, int32_t y, int32_t z) {
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        return map.find(key) != map.end();
    }

    static void clearBlockColor(int32_t x, int32_t y, int32_t z) {
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        getMap().erase(key);
    }

    static void clear() {
        std::lock_guard<std::mutex> lock(getMutex());
        getMap().clear();
    }

    static void save(const std::string& dir) {
        if (dir.empty()) return;
        std::string filePath = dir + "/block_colors.dat";
        FILE* f = fopen(filePath.c_str(), "wb");
        if (!f) return;

        std::lock_guard<std::mutex> lock(getMutex());
        auto& map = getMap();
        uint32_t count = (uint32_t)map.size();
        fwrite(&count, sizeof(uint32_t), 1, f);

        for (auto& pair : map) {
            int32_t x, y, z;
            unpackPosKey(pair.first, x, y, z);
            uint32_t color = pair.second;
            fwrite(&x, sizeof(int32_t), 1, f);
            fwrite(&y, sizeof(int32_t), 1, f);
            fwrite(&z, sizeof(int32_t), 1, f);
            fwrite(&color, sizeof(uint32_t), 1, f);
        }
        fclose(f);
    }

    static void load(const std::string& dir) {
        clear();
        if (dir.empty()) return;
        std::string filePath = dir + "/block_colors.dat";
        FILE* f = fopen(filePath.c_str(), "rb");
        if (!f) return;

        uint32_t count = 0;
        if (fread(&count, sizeof(uint32_t), 1, f) == 1) {
            std::lock_guard<std::mutex> lock(getMutex());
            auto& map = getMap();
            for (uint32_t i = 0; i < count; ++i) {
                int32_t x = 0, y = 0, z = 0;
                uint32_t color = 0;
                if (fread(&x, sizeof(int32_t), 1, f) == 1 &&
                    fread(&y, sizeof(int32_t), 1, f) == 1 &&
                    fread(&z, sizeof(int32_t), 1, f) == 1 &&
                    fread(&color, sizeof(uint32_t), 1, f) == 1) {
                    uint64_t key = getPosKey(x, y, z);
                    map[key] = color;
                }
            }
        }
        fclose(f);
    }

private:
    static inline uint64_t getPosKey(int32_t x, int32_t y, int32_t z) {
        return ((uint64_t)(x & 0x3FFFFFF) << 38) | ((uint64_t)(y & 0xFFF) << 26) | (uint64_t)(z & 0x3FFFFFF);
    }

    static inline void unpackPosKey(uint64_t key, int32_t& x, int32_t& y, int32_t& z) {
        int64_t rawX = (int64_t)(key >> 38) & 0x3FFFFFF;
        if (rawX & 0x2000000) rawX |= ~0x3FFFFFFLL;
        x = (int32_t)rawX;

        int64_t rawY = (int64_t)(key >> 26) & 0xFFF;
        if (rawY & 0x800) rawY |= ~0xFFFLL;
        y = (int32_t)rawY;

        int64_t rawZ = (int64_t)key & 0x3FFFFFF;
        if (rawZ & 0x2000000) rawZ |= ~0x3FFFFFFLL;
        z = (int32_t)rawZ;
    }
};
