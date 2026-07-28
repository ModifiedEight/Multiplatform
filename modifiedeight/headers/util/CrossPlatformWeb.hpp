#pragma once

#include <string>
#include <vector>

std::vector<unsigned char> CrossPlatform_DownloadBinary(const std::string& url);
std::string CrossPlatform_DownloadString(const std::string& url);
bool CrossPlatform_ExtractZip(const std::string& zipFile, const std::string& outDir);
