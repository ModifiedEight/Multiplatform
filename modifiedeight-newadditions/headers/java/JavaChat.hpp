#pragma once
#include <_types.h>
#include <json/json.h>
#include <string>

/*
 * Java's JSON text components, flattened into the plain strings m8 draws.
 *
 * Chat, disconnect reasons and a server list MOTD are all the same shape, so
 * both the live session and the server list ping go through here.
 */

// Append the readable text of one component tree to *out.
void javaChatFlatten(const Json::Value& node, std::string* out);

/*
 * Parse a component tree and flatten it, stripping the section-sign colour
 * codes m8's font does not understand. A blob that is not valid JSON is handed
 * back as-is, because some servers send a bare string.
 */
std::string javaChatToText(const std::string& json);

// Flatten an already-parsed tree and strip its colour codes.
std::string javaChatToText(const Json::Value& node);
