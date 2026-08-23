#pragma once
#include <_types.h>
#include <string>
#include <vector>

/*
 * JavaByteBuf - big-endian byte buffer with the Minecraft Java Edition
 * protocol primitives (VarInt, VarLong, String, UUID, Position).
 *
 * Reads are bounds checked; once a read runs past the end the buffer latches
 * an error flag and every following read returns zero, so a truncated or
 * malformed packet can never walk off the end of the storage.
 */
struct JavaByteBuf {
  std::vector<uint8_t> bytes;
  size_t readPos;
  bool_t bad;

  JavaByteBuf();
  JavaByteBuf(const uint8_t *, size_t);

  void clear();
  void reset();
  size_t remaining() const;
  bool_t failed() const;
  const uint8_t *data() const;
  size_t size() const;

  // ---- reading ----
  uint8_t readByte();
  int8_t readSByte();
  bool_t readBool();
  int16_t readShort();
  uint16_t readUShort();
  int32_t readInt();
  int64_t readLong();
  float readFloat();
  double readDouble();
  int32_t readVarInt();
  int64_t readVarLong();
  std::string readString(int32_t maxLen = 32767);
  void readUUID(uint64_t *hi, uint64_t *lo);
  void readPosition(int32_t *x, int32_t *y, int32_t *z);
  void readBytes(uint8_t *, size_t);
  void skip(size_t);

  // ---- writing ----
  void writeByte(uint8_t);
  void writeSByte(int8_t);
  void writeBool(bool_t);
  void writeShort(int16_t);
  void writeUShort(uint16_t);
  void writeInt(int32_t);
  void writeLong(int64_t);
  void writeFloat(float);
  void writeDouble(double);
  void writeVarInt(int32_t);
  void writeVarLong(int64_t);
  void writeString(const std::string &);
  void writePosition(int32_t, int32_t, int32_t);
  void writeBytes(const uint8_t *, size_t);
  void writeBuf(const JavaByteBuf &);

  static int32_t varIntSize(int32_t);
};
