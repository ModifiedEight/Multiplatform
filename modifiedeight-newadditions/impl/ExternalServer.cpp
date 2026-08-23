#include <ExternalServer.hpp>

ExternalServer::ExternalServer(const ExternalServer& a2) {
	this->field_0 = a2.field_0;
	this->field_4 = a2.field_4;
	this->field_8 = a2.field_8;
	this->field_C = a2.field_C;
	this->isJava = a2.isJava;
}

ExternalServer::ExternalServer(){
	this->field_0 = 0;
	this->field_C = 0;
	this->isJava = 0;
}

ExternalServer::ExternalServer(int32_t a2, const std::string& a3, const std::string& a4, int32_t a5) {
	this->field_0 = a2;
	this->field_4 = a3;
	this->field_8 = a4;
	this->field_C = a5;
	this->isJava = 0;
}

ExternalServer::ExternalServer(int32_t a2, const std::string& a3, const std::string& a4, int32_t a5, bool_t a6) {
	this->field_0 = a2;
	this->field_4 = a3;
	this->field_8 = a4;
	this->field_C = a5;
	this->isJava = a6;
}
