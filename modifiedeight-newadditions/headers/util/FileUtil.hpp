#pragma once
#include <string>

namespace FileUtil {
    std::string FindFileRecursively(const std::string& dir, const std::string& filename);
    std::string FindDirRecursively(const std::string& dir, const std::string& dirname);
    bool CopyDirectory(const std::string& src, const std::string& dst);
    bool RemoveDirectory(const std::string& dir);
}
