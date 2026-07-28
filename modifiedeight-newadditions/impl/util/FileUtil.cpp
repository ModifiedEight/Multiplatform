#include <util/FileUtil.hpp>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#include <io.h>
#include <windows.h>
#define mkdir(dir, mode) _mkdir(dir)
#define rmdir(dir) _rmdir(dir)
#define unlink(file) _unlink(file)
#else
#include <dirent.h>
#include <unistd.h>
#endif

namespace FileUtil {

    std::string FindFileRecursively(const std::string& dir, const std::string& filename) {
        DIR *dp;
        struct dirent *dirp;
        if((dp = opendir(dir.c_str())) == NULL) return "";
        std::string result = "";
        while ((dirp = readdir(dp)) != NULL) {
            std::string name = dirp->d_name;
            if(name == "." || name == "..") continue;
            std::string fullPath = dir + "/" + name;
            
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    result = FindFileRecursively(fullPath, filename);
                    if(result != "") break;
                } else {
                    if(name == filename) {
                        result = fullPath;
                        break;
                    }
                }
            }
        }
        closedir(dp);
        return result;
    }

    std::string FindDirRecursively(const std::string& dir, const std::string& dirname) {
        DIR *dp;
        struct dirent *dirp;
        if((dp = opendir(dir.c_str())) == NULL) return "";
        std::string result = "";
        while ((dirp = readdir(dp)) != NULL) {
            std::string name = dirp->d_name;
            if(name == "." || name == "..") continue;
            std::string fullPath = dir + "/" + name;
            
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    if(name == dirname) {
                        result = fullPath;
                        break;
                    }
                    result = FindDirRecursively(fullPath, dirname);
                    if(result != "") break;
                }
            }
        }
        closedir(dp);
        return result;
    }

    bool CopyDirectory(const std::string& src, const std::string& dst) {
        DIR *dp;
        struct dirent *dirp;
        if((dp = opendir(src.c_str())) == NULL) return false;
        
        mkdir(dst.c_str(), 0777);
        
        while ((dirp = readdir(dp)) != NULL) {
            std::string name = dirp->d_name;
            if(name == "." || name == "..") continue;
            std::string srcPath = src + "/" + name;
            std::string dstPath = dst + "/" + name;
            
            struct stat st;
            if (stat(srcPath.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    CopyDirectory(srcPath, dstPath);
                } else {
                    std::ifstream srcFile(srcPath, std::ios::binary);
                    std::ofstream dstFile(dstPath, std::ios::binary);
                    dstFile << srcFile.rdbuf();
                }
            }
        }
        closedir(dp);
        return true;
    }

    bool RemoveDirectory(const std::string& dir) {
        DIR *dp;
        struct dirent *dirp;
        if((dp = opendir(dir.c_str())) == NULL) return false;
        
        while ((dirp = readdir(dp)) != NULL) {
            std::string name = dirp->d_name;
            if(name == "." || name == "..") continue;
            std::string fullPath = dir + "/" + name;
            
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    RemoveDirectory(fullPath);
                } else {
                    unlink(fullPath.c_str());
                }
            }
        }
        closedir(dp);
        rmdir(dir.c_str());
        return true;
    }
}
