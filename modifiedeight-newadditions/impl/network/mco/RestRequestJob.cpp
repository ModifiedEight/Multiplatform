#include <network/mco/RestRequestJob.hpp>
#include <util/JobStatus.hpp>
#include <network/mco/RestCallTagData.hpp>
#include <time.h>
#include <util/Util.hpp>
#include <network/mco/CurlRestRequestJob.hpp>

std::shared_ptr<RestRequestJob> RestRequestJob::CreateJob(RestRequestType a2, std::shared_ptr<RestService> a3, Minecraft* a4) {
#if defined(__linux__) && !defined(ANDROID)
	std::shared_ptr<RestRequestJob> ret(new CurlRestRequestJob());
	ret->field_8 = ret; //TODO check is this actually how it is assigned
	ret->restService = a3;
	ret->requestType = a2;
	return ret;
#else
	DEBUGMSG("RestRequestJob::CreateJob(not implemented)\n");
	return std::shared_ptr<RestRequestJob>();
#endif
}

RestRequestJob::RestRequestJob(){
	this->field_4 = 0;
	this->status = JS_0;
}
void RestRequestJob::launchRequest(std::shared_ptr<RestRequestJob> a1, std::shared_ptr<ThreadCollection> a2, std::function<void(int32_t, const std::string&, const RestCallTagData&, std::shared_ptr<RestRequestJob>)> a3, std::function<void(bool, bool, int32_t, const std::string&, const RestCallTagData&, std::shared_ptr<RestRequestJob>)> a4) {
	a1->field_10 = a3;
	a1->field_20 = a4;
	Job::addToThreadCollection(a1, *a2.get());
}
void RestRequestJob::setBody(const std::string& a2) {
	this->body = a2;
}

template<>
void RestRequestJob::setMethod<>(const std::string& a2) {
	std::vector<std::string> v4;
	this->field_30 = Util::simpleFormat(a2, v4);
}

template<>
void RestRequestJob::setMethod<long long,std::string,int,std::string>(const std::string& a2, long long, std::string, int, std::string);
template<>
void RestRequestJob::setMethod<long long,std::string>(const std::string& a2, long long, std::string);

void RestRequestJob::setTagData(const RestCallTagData& a2) {
	this->field_44 = a2;
}

RestRequestJob::~RestRequestJob() {
}
void RestRequestJob::stop() {
	this->status = JS_STOPPED;
}
void RestRequestJob::run() {
	this->trySetStatus(JS_STARTED);
	timespec t{1, 0};
	nanosleep(&t, 0);
	this->trySetStatus(JS_FINISHED);
}
void RestRequestJob::finish(){
	//copyRawCharArrayInside(&v6, "Yey"); Nay
	printf("RestRequestJob::finish - not implemented\n"); //TODO

}
