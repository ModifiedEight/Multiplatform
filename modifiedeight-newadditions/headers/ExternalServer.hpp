#pragma once
#include <_types.h>
#include <string>

struct ExternalServer
{
	int32_t field_0;
	std::string field_4, field_8;
	int32_t field_C;
	// Set when this entry is a Minecraft Java Edition 1.8.x server rather than
	// an MCPE one, which decides whether joining goes through RakNet or through
	// the Java session.
	bool_t isJava;

	ExternalServer();
	ExternalServer(const ExternalServer&);
	ExternalServer(int32_t, const std::string&, const std::string&, int32_t);
	ExternalServer(int32_t, const std::string&, const std::string&, int32_t, bool_t);
	//~ExternalServer();
};
