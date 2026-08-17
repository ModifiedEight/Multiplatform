#pragma once
#include <_types.h>
#include <string>
#include <map>
#include <utility>
#include <BitStream.h>

struct RegionFileInfo {
	FILE* fileRaw = nullptr;
	int32_t* locTable = nullptr;
	std::map<int32_t, int32_t> stdMap;
	std::string path;
};

struct RegionFile
{
	std::string folderPath;
	int8_t* bytes4096_2;
	std::map<std::pair<int32_t, int32_t>, RegionFileInfo*> regions;

	RegionFile(const std::string&);
	void close();
	bool_t open();
	bool_t readChunk(int32_t, int32_t, RakNet::BitStream**);
	bool_t write(RegionFileInfo* rInfo, int32_t, const uint8_t* data, int32_t dataSize);
	bool_t writeChunk(int32_t, int32_t, RakNet::BitStream&);
	RegionFileInfo* getRegion(int32_t chunkX, int32_t chunkZ);

	virtual ~RegionFile();
};
