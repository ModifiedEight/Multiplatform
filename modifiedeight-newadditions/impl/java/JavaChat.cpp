#include <java/JavaChat.hpp>

/*
 * Java sends chat as a JSON text component tree. Flatten it into the plain text
 * m8's chat line renderer can take: concatenate "text" and "extra", and for a
 * "translate" component fall back to the key plus its arguments rather than
 * dropping the message entirely (that is what carries join/leave/death notices).
 */
void javaChatFlatten(const Json::Value& node, std::string* out) {
	if(node.isString()) {
		*out += node.asString();
		return;
	}
	if(node.isArray()) {
		for(Json::ArrayIndex i = 0; i < node.size(); ++i) {
			javaChatFlatten(node[i], out);
		}
		return;
	}
	if(!node.isObject()) {
		return;
	}
	if(node.isMember("text") && node["text"].isString()) {
		*out += node["text"].asString();
	}
	if(node.isMember("translate") && node["translate"].isString()) {
		const Json::Value& with = node["with"];
		if(with.isArray() && with.size()) {
			for(Json::ArrayIndex i = 0; i < with.size(); ++i) {
				if(i) *out += " ";
				javaChatFlatten(with[i], out);
			}
		} else {
			*out += node["translate"].asString();
		}
	}
	if(node.isMember("extra")) {
		javaChatFlatten(node["extra"], out);
	}
}

std::string javaChatToText(const Json::Value& node) {
	std::string out;
	javaChatFlatten(node, &out);
	// Strip the section-sign colour codes m8's font does not understand.
	std::string clean;
	clean.reserve(out.size());
	for(size_t i = 0; i < out.size(); ++i) {
		if((uint8_t)out[i] == 0xC2 && i + 2 < out.size() && (uint8_t)out[i + 1] == 0xA7) {
			i += 2;
			continue;
		}
		clean += out[i];
	}
	return clean;
}

std::string javaChatToText(const std::string& json) {
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(json, root, false)) {
		// Some servers send a bare string for disconnect reasons.
		return json;
	}
	return javaChatToText(root);
}
