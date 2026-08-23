#include <gui/buttons/CategoryButton.hpp>
#include <Minecraft.hpp>
#include <Options.hpp>
#include <rendering/Tesselator.hpp>
CategoryButton::CategoryButton(int32_t id, NinePatchLayer* f74, NinePatchLayer* f78, Button** f70) : CategoryButton("", id, f74, f78, f70){

}

CategoryButton::CategoryButton(std::string ss, int32_t id, NinePatchLayer* f74, NinePatchLayer* f78, Button** f70) : ImageButton(id, ss){
	this->field_74 = f74;
	this->field_78 = f78;
	this->field_70 = f70;
}
CategoryButton::~CategoryButton() {
}
void CategoryButton::renderBg(struct Minecraft* mc, int32_t a3, int32_t a4) {
	int32_t v4, v5;
	int32_t posX, posY;
	NinePatchLayer* v8;
	if(this->active && this->pressed && (v4 = this->posX, a3 >= v4) && (v5 = this->posY, a4 >= v5) && a3 < v4 + this->width && a4 < v5 + this->height || *this->field_70 == this) {
		posX = this->posX;
		posY = this->posY;
		v8 = this->field_78;
	} else {
		posX = this->posX;
		posY = this->posY;
		v8 = this->field_74;
	}
	if (v8) {
		v8->setSize((float)this->width, (float)this->height);
		v8->draw(Tesselator::instance, posX, posY);
	}
}
void CategoryButton::render(struct Minecraft* mc, int32_t x, int32_t y) {
	ImageButton::render(mc, x, y);
	if (this->buttonID == 6 && this->image.field_0 == "gui/touchgui2.png" && this->image.field_14 == 134 && this->image.field_18 == 56) {
		int32_t cx = this->posX + this->width / 2;
		int32_t cy = this->posY + this->height / 2;
		this->fill(cx - 3, cy - 6, cx + 4, cy - 5, 0xFF1B1B1B);
		this->fill(cx - 3, cy + 6, cx + 4, cy + 7, 0xFF1B1B1B);
		this->fill(cx - 3, cy - 5, cx - 2, cy - 2, 0xFF1B1B1B);
		this->fill(cx + 3, cy - 5, cx + 4, cy - 2, 0xFF1B1B1B);
		this->fill(cx - 3, cy + 3, cx - 2, cy + 6, 0xFF1B1B1B);
		this->fill(cx + 3, cy + 3, cx + 4, cy + 6, 0xFF1B1B1B);
		this->fill(cx - 6, cy - 3, cx - 5, cy + 4, 0xFF1B1B1B);
		this->fill(cx + 6, cy - 3, cx + 7, cy + 4, 0xFF1B1B1B);
		this->fill(cx - 5, cy - 3, cx - 2, cy - 2, 0xFF1B1B1B);
		this->fill(cx - 5, cy + 3, cx - 2, cy + 4, 0xFF1B1B1B);
		this->fill(cx + 3, cy - 3, cx + 6, cy - 2, 0xFF1B1B1B);
		this->fill(cx + 3, cy + 3, cx + 6, cy + 4, 0xFF1B1B1B);
		this->fill(cx - 2, cy - 5, cx + 3, cy + 6, 0xFF6B6B6B);
		this->fill(cx - 5, cy - 2, cx + 6, cy + 3, 0xFF6B6B6B);
		this->fill(cx - 2, cy - 5, cx + 3, cy - 4, 0xFFDBDBDB);
		this->fill(cx - 2, cy - 4, cx - 1, cy - 2, 0xFFDBDBDB);
		this->fill(cx - 2, cy + 3, cx - 1, cy + 5, 0xFFDBDBDB);
		this->fill(cx - 5, cy - 2, cx - 2, cy - 1, 0xFFDBDBDB);
		this->fill(cx + 3, cy - 2, cx + 6, cy - 1, 0xFFDBDBDB);
		this->fill(cx - 5, cy - 1, cx - 4, cy + 3, 0xFFDBDBDB);
		this->fill(cx - 2, cy + 5, cx + 3, cy + 6, 0xFF3C3C3C);
		this->fill(cx + 2, cy - 4, cx + 3, cy - 2, 0xFF3C3C3C);
		this->fill(cx + 2, cy + 3, cx + 3, cy + 5, 0xFF3C3C3C);
		this->fill(cx - 5, cy + 2, cx - 2, cy + 3, 0xFF3C3C3C);
		this->fill(cx + 3, cy + 2, cx + 6, cy + 3, 0xFF3C3C3C);
		this->fill(cx + 5, cy - 1, cx + 6, cy + 2, 0xFF3C3C3C);
	}
}
bool_t CategoryButton::isSecondImage(bool_t) {
	return 0;
}
