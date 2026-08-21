#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>
#include <cstdio>

struct BlockFaceColors {
    uint32_t colors[6];
    BlockFaceColors() {
        for (int i = 0; i < 6; i++) {
            colors[i] = 0xFFFFFFFF;
        }
    }
    bool isEmpty() const {
        for (int i = 0; i < 6; i++) {
            if (colors[i] != 0xFFFFFFFF) return false;
        }
        return true;
    }
};

class BlockColorRegistry {
public:
    static inline std::unordered_map<uint64_t, BlockFaceColors>& getMap() {
        static std::unordered_map<uint64_t, BlockFaceColors> s_colors;
        return s_colors;
    }

    static inline std::mutex& getMutex() {
        static std::mutex s_mutex;
        return s_mutex;
    }

    static inline std::atomic<size_t>& getCount() {
        static std::atomic<size_t> s_count(0);
        return s_count;
    }

    static void setBlockFaceColor(int32_t x, int32_t y, int32_t z, int32_t face, uint32_t color) {
        if (face < 0 || face > 5) return;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        getMap()[key].colors[face] = color;
        getCount().store(getMap().size(), std::memory_order_relaxed);
    }

    static void setBlockColor(int32_t x, int32_t y, int32_t z, uint32_t color) {
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        for (int i = 0; i < 6; i++) {
            getMap()[key].colors[i] = color;
        }
        getCount().store(getMap().size(), std::memory_order_relaxed);
    }

    static uint32_t getBlockFaceColor(int32_t x, int32_t y, int32_t z, int32_t face) {
        if (getCount().load(std::memory_order_relaxed) == 0 || face < 0 || face > 5) return 0xFFFFFFFF;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second.colors[face];
        }
        return 0xFFFFFFFF;
    }

    static bool hasBlockFaceColor(int32_t x, int32_t y, int32_t z, int32_t face) {
        return getBlockFaceColor(x, y, z, face) != 0xFFFFFFFF;
    }

    static uint32_t getBlockColor(int32_t x, int32_t y, int32_t z) {
        if (getCount().load(std::memory_order_relaxed) == 0) return 0xFFFFFFFF;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        auto it = map.find(key);
        if (it != map.end()) {
            for (int i = 0; i < 6; i++) {
                if (it->second.colors[i] != 0xFFFFFFFF) return it->second.colors[i];
            }
        }
        return 0xFFFFFFFF;
    }

    static bool hasBlockColor(int32_t x, int32_t y, int32_t z) {
        if (getCount().load(std::memory_order_relaxed) == 0) return false;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        auto it = map.find(key);
        if (it != map.end()) {
            return !it->second.isEmpty();
        }
        return false;
    }

    static void clearBlockFaceColor(int32_t x, int32_t y, int32_t z, int32_t face) {
        if (getCount().load(std::memory_order_relaxed) == 0 || face < 0 || face > 5) return;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        auto& map = getMap();
        auto it = map.find(key);
        if (it != map.end()) {
            it->second.colors[face] = 0xFFFFFFFF;
            if (it->second.isEmpty()) {
                map.erase(it);
            }
            getCount().store(map.size(), std::memory_order_relaxed);
        }
    }

    static void clearBlockColor(int32_t x, int32_t y, int32_t z) {
        if (getCount().load(std::memory_order_relaxed) == 0) return;
        std::lock_guard<std::mutex> lock(getMutex());
        uint64_t key = getPosKey(x, y, z);
        getMap().erase(key);
        getCount().store(getMap().size(), std::memory_order_relaxed);
    }

    static void clear() {
        std::lock_guard<std::mutex> lock(getMutex());
        getMap().clear();
        getCount().store(0, std::memory_order_relaxed);
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
            fwrite(&x, sizeof(int32_t), 1, f);
            fwrite(&y, sizeof(int32_t), 1, f);
            fwrite(&z, sizeof(int32_t), 1, f);
            fwrite(pair.second.colors, sizeof(uint32_t), 6, f);
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
                BlockFaceColors bfc;
                if (fread(&x, sizeof(int32_t), 1, f) == 1 &&
                    fread(&y, sizeof(int32_t), 1, f) == 1 &&
                    fread(&z, sizeof(int32_t), 1, f) == 1 &&
                    fread(bfc.colors, sizeof(uint32_t), 6, f) == 6) {
                    uint64_t key = getPosKey(x, y, z);
                    map[key] = bfc;
                }
            }
            getCount().store(map.size(), std::memory_order_relaxed);
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
