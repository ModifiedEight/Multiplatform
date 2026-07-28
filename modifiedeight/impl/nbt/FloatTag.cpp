#include <nbt/FloatTag.hpp>
#include <util/input/IDataInput.hpp>
#include <util/output/IDataOutput.hpp>
#include <sstream>

FloatTag::FloatTag(const std::string& n, float v) : Tag(n){
	this->value = v;
}
void FloatTag::write(IDataOutput* out){
	out->writeFloat(this->value);
}
void FloatTag::load(IDataInput* in){
	this->value = in->readFloat();
}
int32_t FloatTag::getId(void) const{
	return 5;
}
std::string FloatTag::toString(void){
	std::stringstream ss;
	ss << this->value;
	return ss.str();
}
Tag* FloatTag::copy(void){
	return new FloatTag(this->getName(), this->value);
}
bool_t FloatTag::equals(const Tag& v){
	const FloatTag* tg = (const FloatTag*) &v;
	bool_t eq = Tag::equals(v);
	if(eq) return this->value == tg->value;
	return eq;
}
