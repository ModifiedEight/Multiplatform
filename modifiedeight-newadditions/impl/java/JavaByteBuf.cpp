#include <java/JavaByteBuf.hpp>
#include <string.h>

JavaByteBuf::JavaByteBuf() {
	this->readPos = 0;
	this->bad = 0;
}

JavaByteBuf::JavaByteBuf(const uint8_t* src, size_t len) {
	this->readPos = 0;
	this->bad = 0;
	if(src && len) {
		this->bytes.assign(src, src + len);
	}
}

void JavaByteBuf::clear() {
	this->bytes.clear();
	this->readPos = 0;
	this->bad = 0;
}

void JavaByteBuf::reset() {
	this->readPos = 0;
	this->bad = 0;
}

size_t JavaByteBuf::remaining() const {
	if(this->readPos >= this->bytes.size()) return 0;
	return this->bytes.size() - this->readPos;
}

bool_t JavaByteBuf::failed() const {
	return this->bad;
}

const uint8_t* JavaByteBuf::data() const {
	return this->bytes.empty() ? 0 : &this->bytes[0];
}

size_t JavaByteBuf::size() const {
	return this->bytes.size();
}

uint8_t JavaByteBuf::readByte() {
	if(this->readPos >= this->bytes.size()) {
		this->bad = 1;
		return 0;
	}
	return this->bytes[this->readPos++];
}

int8_t JavaByteBuf::readSByte() {
	return (int8_t)this->readByte();
}

bool_t JavaByteBuf::readBool() {
	return this->readByte() != 0;
}

int16_t JavaByteBuf::readShort() {
	return (int16_t)this->readUShort();
}

uint16_t JavaByteBuf::readUShort() {
	uint16_t hi = this->readByte();
	uint16_t lo = this->readByte();
	return (uint16_t)((hi << 8) | lo);
}

int32_t JavaByteBuf::readInt() {
	uint32_t v = 0;
	for(int32_t i = 0; i < 4; ++i) v = (v << 8) | this->readByte();
	return (int32_t)v;
}

int64_t JavaByteBuf::readLong() {
	uint64_t v = 0;
	for(int32_t i = 0; i < 8; ++i) v = (v << 8) | (uint64_t)this->readByte();
	return (int64_t)v;
}

float JavaByteBuf::readFloat() {
	uint32_t bits = (uint32_t)this->readInt();
	float out;
	memcpy(&out, &bits, 4);
	return out;
}

double JavaByteBuf::readDouble() {
	uint64_t bits = (uint64_t)this->readLong();
	double out;
	memcpy(&out, &bits, 8);
	return out;
}

int32_t JavaByteBuf::readVarInt() {
	int32_t result = 0;
	int32_t shift = 0;
	for(int32_t i = 0; i < 5; ++i) {
		uint8_t b = this->readByte();
		if(this->bad) return 0;
		result |= (int32_t)(b & 0x7F) << shift;
		if(!(b & 0x80)) return result;
		shift += 7;
	}
	this->bad = 1; // VarInt longer than 5 bytes is malformed
	return 0;
}

int64_t JavaByteBuf::readVarLong() {
	int64_t result = 0;
	int32_t shift = 0;
	for(int32_t i = 0; i < 10; ++i) {
		uint8_t b = this->readByte();
		if(this->bad) return 0;
		result |= (int64_t)(b & 0x7F) << shift;
		if(!(b & 0x80)) return result;
		shift += 7;
	}
	this->bad = 1;
	return 0;
}

std::string JavaByteBuf::readString(int32_t maxLen) {
	int32_t len = this->readVarInt();
	if(this->bad || len < 0 || len > maxLen * 4 || (size_t)len > this->remaining()) {
		this->bad = 1;
		return std::string();
	}
	std::string out((const char*)&this->bytes[this->readPos], (size_t)len);
	this->readPos += (size_t)len;
	return out;
}

void JavaByteBuf::readUUID(uint64_t* hi, uint64_t* lo) {
	uint64_t a = (uint64_t)this->readLong();
	uint64_t b = (uint64_t)this->readLong();
	if(hi) *hi = a;
	if(lo) *lo = b;
}

/* 1.8 packs a block position into a single long: 26 bits X, 12 bits Y, 26 bits Z. */
void JavaByteBuf::readPosition(int32_t* x, int32_t* y, int32_t* z) {
	uint64_t v = (uint64_t)this->readLong();
	int32_t px = (int32_t)(v >> 38);
	int32_t py = (int32_t)((v >> 26) & 0xFFF);
	int32_t pz = (int32_t)(v & 0x3FFFFFF);
	if(px >= (1 << 25)) px -= (1 << 26);
	if(py >= (1 << 11)) py -= (1 << 12);
	if(pz >= (1 << 25)) pz -= (1 << 26);
	if(x) *x = px;
	if(y) *y = py;
	if(z) *z = pz;
}

void JavaByteBuf::readBytes(uint8_t* dst, size_t len) {
	if(len > this->remaining()) {
		this->bad = 1;
		if(dst && len) memset(dst, 0, len);
		return;
	}
	if(dst && len) memcpy(dst, &this->bytes[this->readPos], len);
	this->readPos += len;
}

void JavaByteBuf::skip(size_t len) {
	if(len > this->remaining()) {
		this->bad = 1;
		this->readPos = this->bytes.size();
		return;
	}
	this->readPos += len;
}

void JavaByteBuf::writeByte(uint8_t v) {
	this->bytes.push_back(v);
}

void JavaByteBuf::writeSByte(int8_t v) {
	this->bytes.push_back((uint8_t)v);
}

void JavaByteBuf::writeBool(bool_t v) {
	this->bytes.push_back(v ? 1 : 0);
}

void JavaByteBuf::writeShort(int16_t v) {
	this->writeUShort((uint16_t)v);
}

void JavaByteBuf::writeUShort(uint16_t v) {
	this->bytes.push_back((uint8_t)(v >> 8));
	this->bytes.push_back((uint8_t)(v & 0xFF));
}

void JavaByteBuf::writeInt(int32_t v) {
	uint32_t u = (uint32_t)v;
	for(int32_t i = 3; i >= 0; --i) this->bytes.push_back((uint8_t)((u >> (i * 8)) & 0xFF));
}

void JavaByteBuf::writeLong(int64_t v) {
	uint64_t u = (uint64_t)v;
	for(int32_t i = 7; i >= 0; --i) this->bytes.push_back((uint8_t)((u >> (i * 8)) & 0xFF));
}

void JavaByteBuf::writeFloat(float v) {
	uint32_t bits;
	memcpy(&bits, &v, 4);
	this->writeInt((int32_t)bits);
}

void JavaByteBuf::writeDouble(double v) {
	uint64_t bits;
	memcpy(&bits, &v, 8);
	this->writeLong((int64_t)bits);
}

void JavaByteBuf::writeVarInt(int32_t v) {
	uint32_t u = (uint32_t)v;
	do {
		uint8_t b = (uint8_t)(u & 0x7F);
		u >>= 7;
		if(u) b |= 0x80;
		this->bytes.push_back(b);
	} while(u);
}

void JavaByteBuf::writeVarLong(int64_t v) {
	uint64_t u = (uint64_t)v;
	do {
		uint8_t b = (uint8_t)(u & 0x7F);
		u >>= 7;
		if(u) b |= 0x80;
		this->bytes.push_back(b);
	} while(u);
}

void JavaByteBuf::writeString(const std::string& s) {
	this->writeVarInt((int32_t)s.size());
	if(s.size()) this->bytes.insert(this->bytes.end(), s.begin(), s.end());
}

void JavaByteBuf::writePosition(int32_t x, int32_t y, int32_t z) {
	uint64_t v = ((uint64_t)(x & 0x3FFFFFF) << 38) | ((uint64_t)(y & 0xFFF) << 26) | (uint64_t)(z & 0x3FFFFFF);
	this->writeLong((int64_t)v);
}

void JavaByteBuf::writeBytes(const uint8_t* src, size_t len) {
	if(src && len) this->bytes.insert(this->bytes.end(), src, src + len);
}

void JavaByteBuf::writeBuf(const JavaByteBuf& other) {
	if(other.bytes.size()) this->bytes.insert(this->bytes.end(), other.bytes.begin(), other.bytes.end());
}

int32_t JavaByteBuf::varIntSize(int32_t v) {
	uint32_t u = (uint32_t)v;
	int32_t n = 0;
	do {
		++n;
		u >>= 7;
	} while(u);
	return n;
}
