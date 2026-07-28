#include <rendering/GLBufferPool.hpp>
GLBufferPool glBufferPool(10);

GLBufferPool::GLBufferPool(unsigned int reserveCnt) {
	this->reserveCnt = reserveCnt;

}

GLBufferPool::~GLBufferPool(){
	this->trim();
}

bool_t GLBufferPool::trim(){
	//TODO check is correct
	if(this->unusedBuffers.size() == 0){
		return 0;
	}
	while(!this->unusedBuffers.empty()) {
		glDeleteBuffers(1, &this->unusedBuffers.front());
		this->unusedBuffers.pop_front();
	}
	return 1;
}

void GLBufferPool::release(uint32_t n){
	uint32_t v4;
	this->unusedBuffers.push_back(n);
	this->usedBuffers.erase(v4);
}

GLuint GLBufferPool::get(){
if(this->unusedBuffers.size() < this->reserveCnt) {
		while(this->unusedBuffers.size() < this->reserveCnt) {
			unsigned int bf;
			glGenBuffers(1, &bf);
			if(glGetError()) break;
			this->unusedBuffers.push_back(bf);
		}
	}

	if(this->unusedBuffers.empty()) {
		return 0;
	}

	uint32_t bf = this->unusedBuffers.front();
	this->unusedBuffers.pop_front();
	this->usedBuffers.insert(bf);
	return bf;
}
