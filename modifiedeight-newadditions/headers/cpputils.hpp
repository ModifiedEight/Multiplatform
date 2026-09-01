#pragma once
#include <_types.h>
#include <Config.hpp>
#include <vector>
#include <string>
#include <memory>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define FUNC_ERRNO _errno2
#define FUNC_MKDIR mkdir2
#else
#define FUNC_ERRNO _errno
#define FUNC_MKDIR mkdir
#endif
struct RestRequestJob;
struct Minecraft;

template <typename T> void safeRemove(T*& p){
	if(p){
		delete p;
		p = 0;
	}
}

template <typename T> void safeStopAndRemove(T&);


void splitString(const std::string&, char_t, std::vector<std::string>&);
bool_t exists(const char_t* a1);
int recursiveDelete(const char_t*);
bool_t DeleteDirectory(const std::string& a1, bool_t);
int FUNC_ERRNO();
int FUNC_MKDIR(const char*);
bool createFolderIfNotExists(const char*);
bool createTree(const char*, const char**, int);
int getRawTimeS();

#include <cstdio>

inline std::string to_string(int v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", v);
	return std::string(buf);
}
inline std::string to_string(unsigned int v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%u", v);
	return std::string(buf);
}
inline std::string to_string(long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%ld", v);
	return std::string(buf);
}
inline std::string to_string(unsigned long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%lu", v);
	return std::string(buf);
}
inline std::string to_string(long long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%lld", v);
	return std::string(buf);
}
inline std::string to_string(unsigned long long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%llu", v);
	return std::string(buf);
}
inline std::string to_string(float v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%f", v);
	return std::string(buf);
}
inline std::string to_string(double v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%f", v);
	return std::string(buf);
}

#if defined(ANDROID)
namespace std {
inline std::string to_string(int v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", v);
	return std::string(buf);
}
inline std::string to_string(unsigned int v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%u", v);
	return std::string(buf);
}
inline std::string to_string(long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%ld", v);
	return std::string(buf);
}
inline std::string to_string(unsigned long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%lu", v);
	return std::string(buf);
}
inline std::string to_string(long long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%lld", v);
	return std::string(buf);
}
inline std::string to_string(unsigned long long v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%llu", v);
	return std::string(buf);
}
inline std::string to_string(float v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%f", v);
	return std::string(buf);
}
inline std::string to_string(double v) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%f", v);
	return std::string(buf);
}
}
#endif
