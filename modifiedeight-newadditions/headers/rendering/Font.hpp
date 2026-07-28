#pragma once
#include <_types.h>
#include <string>
#include <vector>

struct Font
{
	Font(struct AppPlatform*, struct Options*, const std::string&, struct Textures*);
	float buildChar(int32_t, float, float, bool_t);
	int32_t charWidth(int32_t, bool_t);
	bool_t containsUnicodeChar(const std::string&);
	void draw(const char_t*, float, float, int32_t);
	void draw(const char_t*, float, float, int32_t, bool_t);
	void draw(const std::string&, float, float, int32_t);
	void draw(const std::string&, float, float, int32_t, bool_t);
	void drawShadow(const char_t*, float, float, int32_t);
	void drawShadow(const std::string&, float, float, int32_t);
	void drawShadow(const std::string&, float, float, int32_t, int32_t);
	void drawSlow(const char_t*, int32_t, float, float, int32_t, bool_t);
	void drawSlow(const std::string&, float, float, int32_t, bool_t);
	void drawTransformed(const std::string&, float, float, int32_t, float, float, bool_t, float);
	void drawWordWrap(const std::string&, float, float, float, int32_t, bool_t, bool_t);
	std::vector<std::vector<std::string>> getParagraphs(const std::string&);
	float getPixelLength(const std::string&);
	int32_t height(const std::string&, int32_t);
	void init(Options*);
	void setDefaultTexture(void);
	void setUnicodeTexture(int32_t);
	int32_t width(const std::string&);
	virtual ~Font();

	char_t field_4;
	char_t field_5;
	char_t field_6;
	char_t field_7;
	int32_t field_8;
	int32_t field_C;
	int32_t charWidths[256];
	float charLength[256];
	uint8_t *glyphSizes;
	char_t field_814;
	char_t field_815;
	char_t field_816;
	char_t field_817;
	int32_t field_818;
	int32_t field_81C;
	char field_820[0xFFF]; //unused thing, 0x820 - 0x181F
	struct _Font_someStruct{
		std::string file;
		struct Textures* textures;
		struct Options* options;
		struct AppPlatform* appPlatform;
		int32_t field_10, field_14, field_18, field_1C;
	} field_1820;
	int8_t field_1840;
	int8_t field_1841;
	int8_t field_1842;
	int8_t field_1843;
	int32_t field_1844[32];
	char_t field_18C4;
	char_t field_18C5;
	char_t field_18C6;
	char_t field_18C7;
};
