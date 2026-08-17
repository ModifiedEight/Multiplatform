#include <level/storage/RegionFile.hpp>
#include <BitStream.h>
#include <string.h>
#include <zlib.h>

RegionFile::RegionFile(const std::string& a2) {
	this->folderPath = a2;
	this->bytes4096_2 = new int8_t[4096];
	memset(this->bytes4096_2, 0, 4096);
}

void RegionFile::close() {
	for(auto&& pair : this->regions) {
		RegionFileInfo* rInfo = pair.second;
		if(rInfo) {
			if(rInfo->fileRaw) {
				fclose(rInfo->fileRaw);
			}
			if(rInfo->locTable) {
				delete[] rInfo->locTable;
			}
			delete rInfo;
		}
	}
	this->regions.clear();
}

bool_t RegionFile::open() {
	return 1;
}

RegionFileInfo* RegionFile::getRegion(int32_t chunkX, int32_t chunkZ) {
	int32_t rx = chunkX >> 5;
	int32_t rz = chunkZ >> 5;
	std::pair<int32_t, int32_t> key = {rx, rz};
	auto it = this->regions.find(key);
	if (it != this->regions.end()) {
		return it->second;
	}

	RegionFileInfo* rInfo = new RegionFileInfo();
	rInfo->locTable = new int32_t[1024];
	memset(rInfo->locTable, 0, 4096);
	
	if (rx == 0 && rz == 0) {
		rInfo->path = this->folderPath + "/chunks.dat";
	} else {
		rInfo->path = this->folderPath + "/chunks_r." + std::to_string(rx) + "." + std::to_string(rz) + ".dat";
	}

	rInfo->fileRaw = fopen(rInfo->path.c_str(), "r+b");
	if (rInfo->fileRaw) {
		fread(rInfo->locTable, 4, 1024, rInfo->fileRaw);
		rInfo->stdMap[0] = 0;
		for (int v3 = 0; v3 < 1024; ++v3) {
			int32_t v4 = rInfo->locTable[v3];
			if (v4) {
				int32_t v5 = v4 >> 8;
				int32_t v6 = (uint8_t)v4;
				for (int i = 0; i < v6; ++i) {
					rInfo->stdMap[i + v5] = 0;
				}
			}
		}
	} else {
		rInfo->fileRaw = fopen(rInfo->path.c_str(), "w+b");
		if (rInfo->fileRaw) {
			fwrite(rInfo->locTable, 4, 1024, rInfo->fileRaw);
			rInfo->stdMap[0] = 0;
			fclose(rInfo->fileRaw);
			rInfo->fileRaw = fopen(rInfo->path.c_str(), "r+b");
		}
	}

	this->regions[key] = rInfo;
	return rInfo;
}

bool_t RegionFile::readChunk(int32_t chunkX, int32_t chunkZ, RakNet::BitStream** a4) {
	RegionFileInfo* rInfo = this->getRegion(chunkX, chunkZ);
	if (!rInfo || !rInfo->fileRaw) return 0;

	int32_t cx = chunkX & 31;
	int32_t cz = chunkZ & 31;
	int32_t result = rInfo->locTable[32 * cz + cx];
	if (result) {
		fseek(rInfo->fileRaw, (result >> 8) << 12, 0);
		int32_t n = 0;
		fread(&n, 4, 1, rInfo->fileRaw);
		n -= 4;
		if (n > 0) {
			uint8_t* v8 = new uint8_t[n];
			fread(v8, 1, n, rInfo->fileRaw);

			int32_t rx = chunkX >> 5;
			int32_t rz = chunkZ >> 5;
			if (rx == 0 && rz == 0) {
				*a4 = new RakNet::BitStream(v8, n, 0);
			} else {
				uLongf uncompressedSize = 82176;
				uint8_t* uncompressedBuffer = new uint8_t[uncompressedSize];
				int decompressResult = uncompress(uncompressedBuffer, &uncompressedSize, v8, n);
				delete[] v8;
				if (decompressResult == Z_OK) {
					*a4 = new RakNet::BitStream(uncompressedBuffer, uncompressedSize, 0);
				} else {
					delete[] uncompressedBuffer;
					return 0;
				}
			}
			return 1;
		}
	}
	return 0;
}

bool_t RegionFile::write(RegionFileInfo* rInfo, int32_t a2, const uint8_t* data, int32_t dataSize) {
	fseek(rInfo->fileRaw, a2 << 12, 0);
	int32_t v6 = dataSize + 4;
	fwrite(&v6, 4, 1, rInfo->fileRaw);
	fwrite(data, 1, dataSize, rInfo->fileRaw);
	return 1;
}

bool_t RegionFile::writeChunk(int32_t chunkX, int32_t chunkZ, RakNet::BitStream& a4) {
	RegionFileInfo* rInfo = this->getRegion(chunkX, chunkZ);
	if (!rInfo || !rInfo->fileRaw) return 0;

	int32_t cx = chunkX & 31;
	int32_t cz = chunkZ & 31;
	int32_t regionIndex = cx + 32 * cz;
	int32_t locTableEntry = rInfo->locTable[regionIndex];

	int32_t rx = chunkX >> 5;
	int32_t rz = chunkZ >> 5;

	const uint8_t* dataToWrite = (const uint8_t*)a4.GetData();
	int32_t sizeToWrite = a4.GetNumberOfBytesUsed();
	uint8_t* compressedBuffer = nullptr;

	if (rx != 0 || rz != 0) {
		uLongf maxCompressedSize = compressBound(sizeToWrite);
		compressedBuffer = new uint8_t[maxCompressedSize];
		uLongf compressedSize = maxCompressedSize;
		int compressResult = compress(compressedBuffer, &compressedSize, dataToWrite, sizeToWrite);
		if (compressResult == Z_OK) {
			dataToWrite = compressedBuffer;
			sizeToWrite = compressedSize;
		} else {
			delete[] compressedBuffer;
			return 0;
		}
	}

	int32_t v8 = ((int32_t)(sizeToWrite + 4) >> 12) + 1;
	if (v8 <= 256) {
		int32_t firstByteOfLocTableEntry = (uint8_t)locTableEntry;
		int32_t v10 = locTableEntry >> 8;
		if (v10) {
			if (firstByteOfLocTableEntry == v8) {
				this->write(rInfo, v10, dataToWrite, sizeToWrite);
				if (compressedBuffer) delete[] compressedBuffer;
				return 1;
			}
		}
		int32_t v13 = 1;
		for (int v11 = 0; v11 < firstByteOfLocTableEntry; ++v11) {
			rInfo->stdMap[v11 + v10] = v13;
		}
		int32_t v14 = 0;
		int32_t v16 = 0;
		while (1) {
			int32_t v17 = v16 + v14;
			auto&& p = rInfo->stdMap.find(v17);
			if (p == rInfo->stdMap.end()) {
				break;
			}
			if (rInfo->stdMap[v17]) {
				if (++v14 >= v8) {
					goto LABEL_25;
				}
			} else {
				v16 += v14 + 1;
				v14 = 0;
			}
		}
		{
			fseek(rInfo->fileRaw, 0, 2);
			int32_t i = v8 - v14;
			for (int32_t k = 0; k < i; ++k) {
				fwrite(this->bytes4096_2, 4, 1024, rInfo->fileRaw);
				rInfo->stdMap[k + v16] = 1;
			}
		}
	LABEL_25:
		for (int32_t v24 = 0; v24 < v8; ++v24) {
			rInfo->stdMap[v24 + v16] = 0;
		}
		rInfo->locTable[regionIndex] = v8 | (v16 << 8);
		this->write(rInfo, v16, dataToWrite, sizeToWrite);
		fseek(rInfo->fileRaw, regionIndex * 4, 0);
		fwrite(&rInfo->locTable[regionIndex], 4, 1, rInfo->fileRaw);
		if (compressedBuffer) delete[] compressedBuffer;
		return 1;
	}
	return 0;
}

RegionFile::~RegionFile() {
	this->close();
	if (this->bytes4096_2) delete[] this->bytes4096_2;
}
