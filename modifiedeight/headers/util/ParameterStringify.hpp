#pragma once
#include <_types.h>
#include <vector>
#include <string>
#include <sstream>

struct ParameterStringify
{
	template<typename T>
	static void stringifyOne(std::vector<std::string>& vec, const T& arg) {
		std::stringstream ss;
		ss << arg;
		vec.push_back(ss.str());
	}

	static void stringifyNext(std::vector<std::string>& vec) {}

	template<typename T, typename... _args>
	static void stringifyNext(std::vector<std::string>& vec, const T& first, const _args&... args) {
		stringifyOne(vec, first);
		stringifyNext(vec, args...);
	}
};
