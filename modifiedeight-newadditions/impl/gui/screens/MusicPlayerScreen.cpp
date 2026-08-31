#include <gui/screens/MusicPlayerScreen.hpp>
#include <sound/MusicPlayerManager.hpp>
#include <Minecraft.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <rendering/Font.hpp>
#include <gui/buttons/Button.hpp>
#include <cmath>
#include <algorithm>

MusicPlayerScreen::MusicPlayerScreen(int32_t x, int32_t y, int32_t z)
	: blockX(x), blockY(y), blockZ(z), scrollOffset(0),
	  dialogX(0), dialogY(0), dialogW(260), dialogH(210) {
}

MusicPlayerScreen::~MusicPlayerScreen() {
}

void MusicPlayerScreen::init() {
	this->buttons.clear();
	this->dialogW = std::min(260, this->width - 16);
	this->dialogH = std::min(210, this->height - 16);
	this->dialogX = (this->width - this->dialogW) / 2;
	this->dialogY = (this->height - this->dialogH) / 2;
	MusicPlayerManager::instance.loadTracks();
}

void MusicPlayerScreen::tick() {
	if (!this->minecraft->player) {
		this->minecraft->setScreen(0);
		return;
	}
	float dx = ((float)this->blockX + 0.5f) - this->minecraft->player->posX;
	float dy = ((float)this->blockY + 0.5f) - this->minecraft->player->posY;
	float dz = ((float)this->blockZ + 0.5f) - this->minecraft->player->posZ;
	if (dx * dx + dy * dy + dz * dz > 64.0f) {
		this->minecraft->setScreen(0);
	}
}

bool_t MusicPlayerScreen::renderGameBehind() { return 1; }
bool_t MusicPlayerScreen::isInGameScreen() { return 0; }

void MusicPlayerScreen::buttonClicked(Button* btn) {
}

void MusicPlayerScreen::mouseClicked(int32_t mx, int32_t my, int32_t btn) {
	const auto& tracks = MusicPlayerManager::instance.tracks;
	int32_t totalTracks = (int32_t)tracks.size();
	int32_t listY = this->dialogY + 54;
	int32_t listH = this->dialogH - 54 - 26;
	int32_t itemH = 20;
	int32_t visibleCount = listH / itemH;
	if (visibleCount < 1) visibleCount = 1;

	if (btn == 4) {
		if (this->scrollOffset > 0) this->scrollOffset--;
		return;
	}
	if (btn == 5) {
		if (this->scrollOffset + visibleCount < totalTracks) this->scrollOffset++;
		return;
	}

	int32_t closeX = this->dialogX + this->dialogW - 20;
	int32_t closeY = this->dialogY + 5;
	if (mx >= closeX && mx <= closeX + 15 && my >= closeY && my <= closeY + 15) {
		this->minecraft->setScreen(0);
		return;
	}

	int32_t ctrlY = this->dialogY + 33;
	int32_t modeW = 76;
	int32_t modeX = this->dialogX + 10;
	if (mx >= modeX && mx <= modeX + modeW && my >= ctrlY && my <= ctrlY + 16) {
		MusicPlayerManager::instance.cyclePlaybackMode();
		return;
	}

	int32_t prevX = modeX + modeW + 6;
	if (mx >= prevX && mx <= prevX + 22 && my >= ctrlY && my <= ctrlY + 16) {
		MusicPlayerManager::instance.playPrevTrack();
		return;
	}

	int32_t stopX = prevX + 26;
	if (mx >= stopX && mx <= stopX + 38 && my >= ctrlY && my <= ctrlY + 16) {
		if (MusicPlayerManager::instance.isPlayingAt(this->blockX, this->blockY, this->blockZ)) {
			MusicPlayerManager::instance.stop();
		} else if (!tracks.empty()) {
			int idx = (MusicPlayerManager::instance.currentTrackIndex >= 0) ? MusicPlayerManager::instance.currentTrackIndex : 0;
			MusicPlayerManager::instance.playTrack(idx, this->blockX, this->blockY, this->blockZ);
		}
		return;
	}

	int32_t nextX = stopX + 42;
	if (mx >= nextX && mx <= nextX + 22 && my >= ctrlY && my <= ctrlY + 16) {
		MusicPlayerManager::instance.playNextTrack();
		return;
	}

	int32_t scrollX = this->dialogX + this->dialogW - 22;
	if (this->scrollOffset > 0) {
		if (mx >= scrollX && mx <= scrollX + 16 && my >= listY && my <= listY + 16) {
			this->scrollOffset--;
			return;
		}
	}

	if (this->scrollOffset + visibleCount < totalTracks) {
		int32_t downY = listY + listH - 16;
		if (mx >= scrollX && mx <= scrollX + 16 && my >= downY && my <= downY + 16) {
			this->scrollOffset++;
			return;
		}
	}

	int32_t itemW = this->dialogW - 36;
	for (int32_t i = 0; i < visibleCount; ++i) {
		int32_t tIdx = this->scrollOffset + i;
		if (tIdx >= totalTracks) break;
		int32_t iy = listY + i * itemH;
		if (mx >= this->dialogX + 10 && mx <= this->dialogX + 10 + itemW && my >= iy && my <= iy + itemH - 2) {
			MusicPlayerManager::instance.playTrack(tIdx, this->blockX, this->blockY, this->blockZ);
			return;
		}
	}

	int32_t doneY = this->dialogY + this->dialogH - 22;
	int32_t doneX = this->dialogX + (this->dialogW - 70) / 2;
	if (mx >= doneX && mx <= doneX + 70 && my >= doneY && my <= doneY + 16) {
		this->minecraft->setScreen(0);
		return;
	}

	Screen::mouseClicked(mx, my, btn);
}

void MusicPlayerScreen::keyPressed(int32_t key) {
	if (key == 27 || key == 6) this->minecraft->setScreen(0);
}

void MusicPlayerScreen::render(int32_t mx, int32_t my, float a4) {
	this->renderBackground(0);

	int32_t x0 = this->dialogX;
	int32_t y0 = this->dialogY;
	int32_t x1 = this->dialogX + this->dialogW;
	int32_t y1 = this->dialogY + this->dialogH;

	this->fill(x0, y0, x1, y1, 0xE8141414);
	this->fill(x0, y0, x1, y0 + 1, 0xFF5A5A5A);
	this->fill(x0, y1 - 1, x1, y1, 0xFF181818);
	this->fill(x0, y0, x0 + 1, y1, 0xFF5A5A5A);
	this->fill(x1 - 1, y0, x1, y1, 0xFF181818);

	std::string title = "Music Player";
	int32_t tw = this->font->width(title);
	this->font->drawShadow(title, x0 + (this->dialogW - tw) / 2, y0 + 6, 0xFFFFFFFF);

	int32_t closeX = x1 - 19;
	int32_t closeY = y0 + 5;
	bool closeHov = (mx >= closeX && mx <= closeX + 14 && my >= closeY && my <= closeY + 14);
	this->fill(closeX, closeY, closeX + 14, closeY + 14, closeHov ? 0xFF882222 : 0xFF333333);
	this->fill(closeX, closeY, closeX + 14, closeY + 1, closeHov ? 0xFFAA3333 : 0xFF444444);
	int32_t xw = this->font->width("x");
	this->font->drawShadow("x", closeX + (14 - xw) / 2, closeY + 3, 0xFFFFFFFF);

	bool isThisPlaying = MusicPlayerManager::instance.isPlayingAt(this->blockX, this->blockY, this->blockZ);
	std::string statusText;
	uint32_t statusCol = 0xAAAAAA;
	if (isThisPlaying) {
		statusText = "> " + MusicPlayerManager::instance.getCurrentTrackName();
		statusCol = 0x55FF55;
	} else {
		statusText = "Select a track to play";
		statusCol = 0xAAAAAA;
	}
	this->font->drawShadow(statusText, x0 + 10, y0 + 20, statusCol);

	int32_t ctrlY = y0 + 33;
	int32_t modeW = 76;
	int32_t modeX = x0 + 10;
	bool modeHov = (mx >= modeX && mx <= modeX + modeW && my >= ctrlY && my <= ctrlY + 16);
	this->fill(modeX, ctrlY, modeX + modeW, ctrlY + 16, modeHov ? 0xFF404050 : 0xFF2A2A35);
	this->fill(modeX, ctrlY, modeX + modeW, ctrlY + 1, modeHov ? 0xFF606075 : 0xFF3E3E4E);
	std::string modeStr = MusicPlayerManager::instance.getPlaybackModeName();
	int32_t mw = this->font->width(modeStr);
	this->font->drawShadow(modeStr, modeX + (modeW - mw) / 2, ctrlY + 4, 0xFFDDDDFF);

	int32_t prevX = modeX + modeW + 6;
	bool prevHov = (mx >= prevX && mx <= prevX + 22 && my >= ctrlY && my <= ctrlY + 16);
	this->fill(prevX, ctrlY, prevX + 22, ctrlY + 16, prevHov ? 0xFF4A4A4A : 0xFF2F2F2F);
	this->fill(prevX, ctrlY, prevX + 22, ctrlY + 1, prevHov ? 0xFF666666 : 0xFF444444);
	int32_t pw = this->font->width("<<");
	this->font->drawShadow("<<", prevX + (22 - pw) / 2, ctrlY + 4, 0xFFFFFFFF);

	int32_t stopX = prevX + 26;
	bool stopHov = (mx >= stopX && mx <= stopX + 38 && my >= ctrlY && my <= ctrlY + 16);
	uint32_t stopBg = isThisPlaying ? (stopHov ? 0xFF7A2020 : 0xFF501818) : (stopHov ? 0xFF206030 : 0xFF184422);
	uint32_t stopTop = isThisPlaying ? (stopHov ? 0xFFA03030 : 0xFF702020) : (stopHov ? 0xFF308840 : 0xFF226030);
	this->fill(stopX, ctrlY, stopX + 38, ctrlY + 16, stopBg);
	this->fill(stopX, ctrlY, stopX + 38, ctrlY + 1, stopTop);
	std::string stopStr = isThisPlaying ? "Stop" : "Play";
	int32_t sw = this->font->width(stopStr);
	this->font->drawShadow(stopStr, stopX + (38 - sw) / 2, ctrlY + 4, 0xFFFFFFFF);

	int32_t nextX = stopX + 42;
	bool nextHov = (mx >= nextX && mx <= nextX + 22 && my >= ctrlY && my <= ctrlY + 16);
	this->fill(nextX, ctrlY, nextX + 22, ctrlY + 16, nextHov ? 0xFF4A4A4A : 0xFF2F2F2F);
	this->fill(nextX, ctrlY, nextX + 22, ctrlY + 1, nextHov ? 0xFF666666 : 0xFF444444);
	int32_t nw = this->font->width(">>");
	this->font->drawShadow(">>", nextX + (22 - nw) / 2, ctrlY + 4, 0xFFFFFFFF);

	int32_t listY = y0 + 54;
	int32_t listH = this->dialogH - 54 - 26;
	int32_t itemH = 20;
	int32_t visibleCount = listH / itemH;
	if (visibleCount < 1) visibleCount = 1;

	const auto& tracks = MusicPlayerManager::instance.tracks;
	int32_t totalTracks = (int32_t)tracks.size();
	int32_t itemW = this->dialogW - 36;

	for (int32_t i = 0; i < visibleCount; ++i) {
		int32_t tIdx = this->scrollOffset + i;
		if (tIdx >= totalTracks) break;
		int32_t iy = listY + i * itemH;

		bool hov = (mx >= x0 + 10 && mx <= x0 + 10 + itemW && my >= iy && my <= iy + itemH - 2);
		bool active = (isThisPlaying && MusicPlayerManager::instance.currentTrackIndex == tIdx);

		uint32_t bg = active ? 0xFF1C4428 : (hov ? 0xFF353540 : 0xFF222222);
		uint32_t border = active ? 0xFF308048 : (hov ? 0xFF505060 : 0xFF333333);

		this->fill(x0 + 10, iy, x0 + 10 + itemW, iy + itemH - 2, bg);
		this->fill(x0 + 10, iy, x0 + 10 + itemW, iy + 1, border);
		this->fill(x0 + 10, iy + itemH - 3, x0 + 10 + itemW, iy + itemH - 2, 0xFF151515);

		std::string trackNum = std::to_string(tIdx + 1) + ". ";
		std::string trackLabel = trackNum + tracks[tIdx].name;
		uint32_t textCol = active ? 0x66FF66 : (hov ? 0xFFFFAA : 0xDDDDDD);
		this->font->drawShadow(trackLabel, x0 + 15, iy + 5, textCol);
	}

	if (totalTracks > visibleCount) {
		int32_t scrollX = x1 - 22;
		int32_t upY = listY;
		bool canUp = (this->scrollOffset > 0);
		bool upHov = (canUp && mx >= scrollX && mx <= scrollX + 16 && my >= upY && my <= upY + 16);
		this->fill(scrollX, upY, scrollX + 16, upY + 16, upHov ? 0xFF4A4A5A : (canUp ? 0xFF2E2E35 : 0xFF1D1D20));
		this->fill(scrollX, upY, scrollX + 16, upY + 1, upHov ? 0xFF65657E : (canUp ? 0xFF40404C : 0xFF2A2A2E));
		int32_t uw = this->font->width("^");
		this->font->drawShadow("^", scrollX + (16 - uw) / 2, upY + 4, canUp ? 0xFFFFFFFF : 0xFF555555);

		int32_t downY = listY + listH - 16;
		bool canDown = (this->scrollOffset + visibleCount < totalTracks);
		bool downHov = (canDown && mx >= scrollX && mx <= scrollX + 16 && my >= downY && my <= downY + 16);
		this->fill(scrollX, downY, scrollX + 16, downY + 16, downHov ? 0xFF4A4A5A : (canDown ? 0xFF2E2E35 : 0xFF1D1D20));
		this->fill(scrollX, downY, scrollX + 16, downY + 1, downHov ? 0xFF65657E : (canDown ? 0xFF40404C : 0xFF2A2A2E));
		int32_t dw = this->font->width("v");
		this->font->drawShadow("v", scrollX + (16 - dw) / 2, downY + 4, canDown ? 0xFFFFFFFF : 0xFF555555);

		int32_t trackTop = upY + 18;
		int32_t trackBottom = downY - 2;
		int32_t trackH = trackBottom - trackTop;
		if (trackH > 10) {
			this->fill(scrollX + 6, trackTop, scrollX + 10, trackBottom, 0xFF151518);
			float thumbRatio = (float)visibleCount / (float)totalTracks;
			int32_t thumbH = std::max(8, (int32_t)(thumbRatio * (float)trackH));
			float posRatio = (float)this->scrollOffset / (float)(totalTracks - visibleCount);
			int32_t thumbY = trackTop + (int32_t)(posRatio * (float)(trackH - thumbH));
			this->fill(scrollX + 5, thumbY, scrollX + 11, thumbY + thumbH, 0xFF505060);
		}
	}

	int32_t doneY = y1 - 22;
	int32_t doneX = x0 + (this->dialogW - 70) / 2;
	bool doneHov = (mx >= doneX && mx <= doneX + 70 && my >= doneY && my <= doneY + 16);
	this->fill(doneX, doneY, doneX + 70, doneY + 16, doneHov ? 0xFF4A554A : 0xFF2E352E);
	this->fill(doneX, doneY, doneX + 70, doneY + 1, doneHov ? 0xFF667866 : 0xFF445044);
	int32_t cw = this->font->width("Close");
	this->font->drawShadow("Close", doneX + (70 - cw) / 2, doneY + 4, 0xFFFFFFFF);
}
