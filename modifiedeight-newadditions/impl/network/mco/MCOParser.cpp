#include <network/mco/MCOParser.hpp>
#include <json/json.h>
#include <network/mco/LoginInformation.hpp>

void MCOParser::parseErrorMessage(const std::string& a2, std::string& reason, int32_t& error) {
	Json::Value v8;
	Json::Reader v11;
	v11.parse(a2, v8, 1);
	error = v8.get("error", Json::Value()).asInt();
	reason = v8.get("reason", Json::Value()).asString();
}
void MCOParser::parseJoinWorld(const std::string& a2, std::string& ip, uint16_t& port, std::string& a5) {
	Json::Value v9;
	Json::Reader v12;
	if(v12.parse(a2, v9, 1)) {
		ip = v9.get("ip", Json::Value("127.0.0.1")).asString();
		port = v9.get("port", Json::Value(25675)).asInt();
		a5 = v9.get("yek", Json::Value("")).asString(); //TODO check is this actually "yek"
	}
}
LoginInformation MCOParser::parseMCOAccountValidSessionReturnValue(const std::string& a3) {
	LoginInformation ret;
	Json::Value v7;
	Json::Reader v11;
	v11.parse(a3, v7, 1);
	ret.accessToken = v7.get("accessToken", Json::Value("")).asString();
	ret.clientId = v7.get("clientToken", Json::Value("")).asString();
	ret.profileId = v7.get("selectedProfile", Json::Value(7)).asString();
	ret.profileName = v7.get("name", Json::Value("")).asString();
	return ret;
}
std::unordered_map<int64_t, MCOServerListItem> MCOParser::parseServerList(const std::string& a3);
void MCOParser::parseStatus(const std::string& a2, bool& buyServerEnabled, bool& createServersEnabled, bool& serviceEnabled) {
	Json::Value v8;
	Json::Reader v11;
	v11.parse(a2, v8, 1);
	buyServerEnabled = v8.get("buyServerEnabled", Json::Value()).asBool();
	createServersEnabled = v8.get("createServerEnabled", Json::Value()).asBool();
	serviceEnabled = v8.get("serviceEnabled", Json::Value()).asBool();
}
