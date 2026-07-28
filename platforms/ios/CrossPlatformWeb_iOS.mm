#ifdef MCPE_IOS
#import <Foundation/Foundation.h>
#include <util/CrossPlatformWeb.hpp>
#include <zlib.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

static std::string encodeUrlSpaces(const std::string& url) {
    std::string out = url;
    size_t pos = 0;
    while((pos = out.find(" ", pos)) != std::string::npos) {
        out.replace(pos, 1, "%20");
        pos += 3;
    }
    return out;
}

static void mkdirp(const std::string& path) {
    if(path.empty()) return;
    std::string cur;
    cur.reserve(path.size());
    for(char c : path) {
        cur += c;
        if(c == '/' && !cur.empty()) {
            mkdir(cur.c_str(), 0755);
        }
    }
    mkdir(cur.c_str(), 0755);
}

static uint16_t readLE16(const unsigned char* p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t readLE32(const unsigned char* p) {
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

static bool sanitizeZipEntryName(const std::string& raw, std::string& out) {
    out.clear();
    if(raw.empty()) return false;

    std::string path = raw;
    std::replace(path.begin(), path.end(), '\\', '/');
    while(!path.empty() && path.front() == '/') {
        path.erase(path.begin());
    }
    if(path.size() >= 2 && path[1] == ':') {
        return false;
    }

    size_t start = 0;
    while(start <= path.size()) {
        size_t slash = path.find('/', start);
        std::string part = path.substr(start, slash == std::string::npos ? std::string::npos : slash - start);
        start = (slash == std::string::npos) ? path.size() + 1 : slash + 1;
        if(part.empty() || part == ".") {
            continue;
        }
        if(part == "..") {
            return false;
        }
        if(!out.empty()) out += '/';
        out += part;
    }
    return !out.empty();
}

static bool copyStoredEntry(FILE* zip, uint64_t dataOffset, uint64_t size, FILE* out) {
    if(fseeko(zip, (off_t)dataOffset, SEEK_SET) != 0) return false;
    std::vector<unsigned char> buffer(32768);
    uint64_t remaining = size;
    while(remaining > 0) {
        size_t want = (size_t)std::min<uint64_t>(remaining, buffer.size());
        size_t got = fread(buffer.data(), 1, want, zip);
        if(got == 0) return false;
        if(fwrite(buffer.data(), 1, got, out) != got) return false;
        remaining -= got;
    }
    return true;
}

static bool inflateEntry(FILE* zip, uint64_t dataOffset, uint64_t compressedSize, FILE* out) {
    if(fseeko(zip, (off_t)dataOffset, SEEK_SET) != 0) return false;

    z_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    if(inflateInit2(&strm, -MAX_WBITS) != Z_OK) {
        return false;
    }

    std::vector<unsigned char> input(32768);
    std::vector<unsigned char> output(32768);
    uint64_t remaining = compressedSize;
    int ret = Z_OK;

    while(remaining > 0) {
        size_t want = (size_t)std::min<uint64_t>(remaining, input.size());
        size_t got = fread(input.data(), 1, want, zip);
        if(got == 0) {
            inflateEnd(&strm);
            return false;
        }
        remaining -= got;

        strm.next_in = input.data();
        strm.avail_in = (unsigned int)got;

        while(strm.avail_in > 0) {
            strm.next_out = output.data();
            strm.avail_out = (unsigned int)output.size();
            ret = inflate(&strm, Z_NO_FLUSH);
            if(ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&strm);
                return false;
            }

            size_t produced = output.size() - strm.avail_out;
            if(produced > 0 && fwrite(output.data(), 1, produced, out) != produced) {
                inflateEnd(&strm);
                return false;
            }

            if(ret == Z_STREAM_END) {
                inflateEnd(&strm);
                return true;
            }
        }
    }

    inflateEnd(&strm);
    return false;
}

static bool extractZipFile(const std::string& zipFile, const std::string& outDir) {
    FILE* zip = fopen(zipFile.c_str(), "rb");
    if(!zip) return false;

    if(fseeko(zip, 0, SEEK_END) != 0) {
        fclose(zip);
        return false;
    }
    off_t fileSize = ftello(zip);
    if(fileSize < 0) {
        fclose(zip);
        return false;
    }

    const off_t eocdSearchFloor = (fileSize > (off_t)(0xFFFF + 22)) ? fileSize - (0xFFFF + 22) : 0;
    unsigned char eocd[22];
    off_t eocdOffset = -1;
    for(off_t pos = fileSize - 22; pos >= eocdSearchFloor; --pos) {
        if(fseeko(zip, pos, SEEK_SET) != 0) break;
        if(fread(eocd, 1, sizeof(eocd), zip) != sizeof(eocd)) break;
        if(readLE32(eocd) == 0x06054b50) {
            eocdOffset = pos;
            break;
        }
        if(pos == 0) break;
    }
    if(eocdOffset < 0) {
        fclose(zip);
        return false;
    }

    uint16_t totalEntries = readLE16(eocd + 10);
    uint32_t centralDirectoryOffset = readLE32(eocd + 16);
    if(fseeko(zip, (off_t)centralDirectoryOffset, SEEK_SET) != 0) {
        fclose(zip);
        return false;
    }

    mkdirp(outDir);

    for(uint16_t i = 0; i < totalEntries; ++i) {
        unsigned char cdh[46];
        if(fread(cdh, 1, sizeof(cdh), zip) != sizeof(cdh)) {
            fclose(zip);
            return false;
        }
        if(readLE32(cdh) != 0x02014b50) {
            fclose(zip);
            return false;
        }

        uint16_t flags = readLE16(cdh + 8);
        uint16_t method = readLE16(cdh + 10);
        uint32_t compressedSize = readLE32(cdh + 20);
        uint32_t uncompressedSize = readLE32(cdh + 24);
        uint16_t nameLen = readLE16(cdh + 28);
        uint16_t extraLen = readLE16(cdh + 30);
        uint16_t commentLen = readLE16(cdh + 32);
        uint32_t localHeaderOffset = readLE32(cdh + 42);

        std::string rawName(nameLen, '\0');
        if(nameLen > 0 && fread(&rawName[0], 1, nameLen, zip) != nameLen) {
            fclose(zip);
            return false;
        }
        if(fseeko(zip, extraLen + commentLen, SEEK_CUR) != 0) {
            fclose(zip);
            return false;
        }

        off_t nextCentralDirEntry = ftello(zip);
        if(nextCentralDirEntry < 0) {
            fclose(zip);
            return false;
        }

        std::string safeName;
        bool isDirectory = !rawName.empty() && rawName.back() == '/';
        if(!sanitizeZipEntryName(rawName, safeName)) {
            if(isDirectory) {
                continue;
            }
            fclose(zip);
            return false;
        }

        std::string outPath = outDir + "/" + safeName;
        if(isDirectory) {
            mkdirp(outPath);
            continue;
        }

        if(fseeko(zip, (off_t)localHeaderOffset, SEEK_SET) != 0) {
            fclose(zip);
            return false;
        }
        unsigned char lfh[30];
        if(fread(lfh, 1, sizeof(lfh), zip) != sizeof(lfh)) {
            fclose(zip);
            return false;
        }
        if(readLE32(lfh) != 0x04034b50) {
            fclose(zip);
            return false;
        }
        uint16_t localNameLen = readLE16(lfh + 26);
        uint16_t localExtraLen = readLE16(lfh + 28);
        uint64_t dataOffset = (uint64_t)localHeaderOffset + 30u + localNameLen + localExtraLen;

        size_t slash = outPath.find_last_of('/');
        if(slash != std::string::npos) {
            mkdirp(outPath.substr(0, slash));
        } else {
            mkdirp(outDir);
        }

        FILE* out = fopen(outPath.c_str(), "wb");
        if(!out) {
            fclose(zip);
            return false;
        }

        bool ok = false;
        if(method == 0) {
            ok = copyStoredEntry(zip, dataOffset, compressedSize, out);
        } else if(method == 8) {
            ok = inflateEntry(zip, dataOffset, compressedSize, out);
        }
        fclose(out);
        if(!ok) {
            fclose(zip);
            return false;
        }

        (void)flags;
        (void)uncompressedSize;

        if(fseeko(zip, nextCentralDirEntry, SEEK_SET) != 0) {
            fclose(zip);
            return false;
        }
    }

    fclose(zip);
    return true;
}

} // namespace

std::vector<unsigned char> CrossPlatform_DownloadBinary(const std::string& rawUrl) {
    @autoreleasepool {
        std::string url = encodeUrlSpaces(rawUrl);
        NSString* nsUrl = [NSString stringWithUTF8String:url.c_str()];
        if(!nsUrl) return {};
        NSURL* urlObj = [NSURL URLWithString:nsUrl];
        if(!urlObj) return {};

        NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:urlObj
                                                               cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                                                           timeoutInterval:30.0];
        [request setHTTPShouldHandleCookies:YES];

        NSURLResponse* response = nil;
        NSError* error = nil;
        NSData* data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
        if(!data || error) {
            return {};
        }

        const unsigned char* bytes = (const unsigned char*)[data bytes];
        return std::vector<unsigned char>(bytes, bytes + [data length]);
    }
}

std::string CrossPlatform_DownloadString(const std::string& url) {
    std::vector<unsigned char> data = CrossPlatform_DownloadBinary(url);
    if(data.empty()) return "";
    return std::string(data.begin(), data.end());
}

bool CrossPlatform_ExtractZip(const std::string& zipFile, const std::string& outDir) {
    return extractZipFile(zipFile, outDir);
}
#endif
