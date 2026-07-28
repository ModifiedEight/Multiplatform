#include <cpputils.hpp>
#include <sstream>
#include <string.h>
#include <errno.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/time.h>
#define access _access
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif
#include <network/mco/RestRequestJob.hpp>

template<>
void safeStopAndRemove<std::shared_ptr<RestRequestJob>>(std::shared_ptr<RestRequestJob>& a2) {
	if(a2.get()) {
		a2->stop();
		a2 = std::shared_ptr<RestRequestJob>(); //TODO check
	}
}
void splitString(const std::string& a1, char_t a2, std::vector<std::string>& a3) {
	std::stringstream v9(a1);
	for(std::string v7; std::getline(v9, v7, a2);) {
		a3.push_back(v7);
	}
}
bool_t DeleteDirectory(const std::string& a1, bool_t a2) {
	return recursiveDelete(a1.c_str()) == 0;
}

int recursiveDelete(const char_t* a1) {
#if defined(_WIN32) || defined(WIN32)
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile((std::string(a1)+("\\*.*")).c_str(), &data);
	if(hFind == INVALID_HANDLE_VALUE){
		return remove(a1);
	}
	do{
		if(!strcmp(data.cFileName, ".") || !strcmp(data.cFileName, "..")) continue;
		int result;
		if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			result = recursiveDelete((std::string(a1)+"\\"+data.cFileName).c_str());
		}else{
			result = remove((std::string(a1)+"\\"+data.cFileName).c_str());
		}
		if(result) return result;
	}while(FindNextFile(hFind, &data) != 0);
	return remove(a1);
#else
	DIR* v2 = opendir(a1);
	if(v2) {
		while(1) {
			struct dirent* v5 = readdir(v2);
			if(!v5) break;
			if(strcmp(v5->d_name, ".")) { //TODO it should use d_name[8] ?
				if(strcmp(v5->d_name, "..")) {
					char_t s[2048];
					sprintf(s, "%s/%s", a1, v5->d_name);
					int result = v5->d_type == DT_DIR ? recursiveDelete(s) : remove(s); //TODO references some unknown var
					if(result) {
						return result;
					}
				}
			}
		}

		closedir(v2);
	}
	return remove(a1);
#endif
}
bool_t exists(const char_t* a1){
	return access(a1, 0) == 0;
}
int FUNC_ERRNO() {
	return errno; //TODO check
}
int FUNC_MKDIR(const char* a1) {
#if defined(_WIN32) || defined(WIN32)
	return _mkdir(a1);
#else
	return mkdir(a1, 0755u);
#endif
}
bool createFolderIfNotExists(const char* a1){
	if ( exists(a1) || !FUNC_MKDIR(a1) )
	{
		return 1;
	}
	FUNC_ERRNO();
	return 0;
}
bool createTree(const char* a1, const char** a2, int a3) {
	if(!createFolderIfNotExists(a1)) {
		return 0;
	}
	std::string v10 = a1;
	for(int v7 = 0; v7 < a3; ++v7) {
		v10 += a2[v7];
		if(!createFolderIfNotExists(v10.c_str())) {
			return 0;
		}
	}
	return 1;
}

int getRawTimeS() {
	struct timeval v1;
	gettimeofday(&v1, 0);
	return v1.tv_sec;
}
