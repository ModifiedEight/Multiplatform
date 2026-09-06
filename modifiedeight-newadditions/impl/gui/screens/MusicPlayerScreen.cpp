#include <gui/screens/MusicPlayerScreen.hpp>
#include <sound/MusicPlayerManager.hpp>
#include <Minecraft.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <rendering/Font.hpp>
#include <gui/buttons/Button.hpp>
#include <cmath>
#include <algorithm>

#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <unigl.h>

static void renderTransparentBtn(Minecraft* mc, Font* font, int32_t x, int32_t y, int32_t w, int32_t h, const std::string& text, bool hov, bool enabled = true) {
	mc->texturesPtr->loadAndBindTexture("gui/gui.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!enabled) {
		glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
	} else if (hov) {
		glColor4f(0.75f, 0.75f, 0.75f, 0.85f);
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	}
	float u0 = 236.0f * 0.00390625f;
	float v0 = 64.0f * 0.00390625f;
	float u1 = (236.0f + 18.0f) * 0.00390625f;
	float v1 = (64.0f + 18.0f) * 0.00390625f;
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.vertexUV((float)x, (float)(y + h), 0.0f, u0, v1);
	t.vertexUV((float)(x + w), (float)(y + h), 0.0f, u1, v1);
	t.vertexUV((float)(x + w), (float)y, 0.0f, u1, v0);
	t.vertexUV((float)x, (float)y, 0.0f, u0, v0);
	t.draw(1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	int32_t tw = font->width(text);
	int32_t tx = x + (w - tw) / 2;
	int32_t ty = y + (h - 8) / 2;
	if (text == "✕") {
		tx = x + (w - tw + 1) / 2;
	} else if (text == "▲") {
		ty = y + (h - 8) / 2 - 1;
	} else if (text == "▼") {
		ty = y + (h - 8) / 2 + 1;
	}
	uint32_t col = !enabled ? 0x888888 : (hov ? 0xFFFFA0 : 0xFFFFFF);
	font->drawShadow(text, tx, ty, col);
}

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
	int32_t modeW = 68;
	int32_t modeX = this->dialogX + 10;
	if (mx >= modeX && mx <= modeX + modeW && my >= ctrlY && my <= ctrlY + 16) {
		MusicPlayerManager::instance.cyclePlaybackMode();
		return;
	}

	int32_t prevX = modeX + modeW + 4;
	if (mx >= prevX && mx <= prevX + 20 && my >= ctrlY && my <= ctrlY + 16) {
		MusicPlayerManager::instance.playPrevTrack();
		return;
	}

	int32_t playPauseX = prevX + 24;
	int32_t playPauseW = 54;
	if (mx >= playPauseX && mx <= playPauseX + playPauseW && my >= ctrlY && my <= ctrlY + 16) {
		if (MusicPlayerManager::instance.isPlayingAt(this->blockX, this->blockY, this->blockZ)) {
			MusicPlayerManager::instance.togglePause();
		} else if (!tracks.empty()) {
			int idx = (MusicPlayerManager::instance.currentTrackIndex >= 0) ? MusicPlayerManager::instance.currentTrackIndex : 0;
			MusicPlayerManager::instance.playTrack(idx, this->blockX, this->blockY, this->blockZ);
		}
		return;
	}

	int32_t stopX = playPauseX + playPauseW + 4;
	int32_t stopW = 38;
	if (mx >= stopX && mx <= stopX + stopW && my >= ctrlY && my <= ctrlY + 16) {
		if (MusicPlayerManager::instance.isPlayingAt(this->blockX, this->blockY, this->blockZ)) {
			MusicPlayerManager::instance.stopAt(this->blockX, this->blockY, this->blockZ);
		}
		return;
	}

	int32_t nextX = stopX + stopW + 4;
	if (mx >= nextX && mx <= nextX + 20 && my >= ctrlY && my <= ctrlY + 16) {
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
		int32_t downY = listY + visibleCount * itemH - 16;
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

	int32_t closeX = x1 - 20;
	int32_t closeY = y0 + 4;
	bool closeHov = (mx >= closeX && mx <= closeX + 16 && my >= closeY && my <= closeY + 16);
	renderTransparentBtn(this->minecraft, this->font, closeX, closeY, 16, 16, "✕", closeHov);

	bool isThisPlaying = MusicPlayerManager::instance.isPlayingAt(this->blockX, this->blockY, this->blockZ);
	bool isPaused = MusicPlayerManager::instance.isPaused;
	std::string statusText;
	uint32_t statusCol = 0xAAAAAA;
	if (isThisPlaying) {
		if (isPaused) {
			statusText = "|| " + MusicPlayerManager::instance.getCurrentTrackName() + " (Paused)";
			statusCol = 0xFFAA33;
		} else {
			statusText = "> " + MusicPlayerManager::instance.getCurrentTrackName();
			statusCol = 0x55FF55;
		}
	} else {
		statusText = "Select a track to play";
		statusCol = 0xAAAAAA;
	}
	this->font->drawShadow(statusText, x0 + 10, y0 + 20, statusCol);

	int32_t ctrlY = y0 + 33;
	int32_t modeW = 68;
	int32_t modeX = x0 + 10;
	bool modeHov = (mx >= modeX && mx <= modeX + modeW && my >= ctrlY && my <= ctrlY + 16);
	this->fill(modeX, ctrlY, modeX + modeW, ctrlY + 16, modeHov ? 0xFF404050 : 0xFF2A2A35);
	this->fill(modeX, ctrlY, modeX + modeW, ctrlY + 1, modeHov ? 0xFF606075 : 0xFF3E3E4E);
	std::string modeStr = MusicPlayerManager::instance.getPlaybackModeName();
	int32_t mw = this->font->width(modeStr);
	this->font->drawShadow(modeStr, modeX + (modeW - mw) / 2, ctrlY + 4, 0xFFDDDDFF);

	int32_t prevX = modeX + modeW + 4;
	bool prevHov = (mx >= prevX && mx <= prevX + 20 && my >= ctrlY && my <= ctrlY + 16);
	this->fill(prevX, ctrlY, prevX + 20, ctrlY + 16, prevHov ? 0xFF4A4A4A : 0xFF2F2F2F);
	this->fill(prevX, ctrlY, prevX + 20, ctrlY + 1, prevHov ? 0xFF666666 : 0xFF444444);
	int32_t pw = this->font->width("<<");
	this->font->drawShadow("<<", prevX + (20 - pw) / 2, ctrlY + 4, 0xFFFFFFFF);

	int32_t playPauseX = prevX + 24;
	int32_t playPauseW = 54;
	bool ppHov = (mx >= playPauseX && mx <= playPauseX + playPauseW && my >= ctrlY && my <= ctrlY + 16);
	std::string ppStr;
	uint32_t ppBg, ppTop, ppTextCol;
	if (isThisPlaying) {
		if (isPaused) {
			ppStr = "Resume";
			ppBg = ppHov ? 0xFF206030 : 0xFF184422;
			ppTop = ppHov ? 0xFF308840 : 0xFF226030;
			ppTextCol = 0xFF88FF88;
		} else {
			ppStr = "Pause";
			ppBg = ppHov ? 0xFF554420 : 0xFF3A3015;
			ppTop = ppHov ? 0xFF776030 : 0xFF4E4020;
			ppTextCol = 0xFFFFFF88;
		}
	} else {
		ppStr = "Play";
		ppBg = ppHov ? 0xFF206030 : 0xFF184422;
		ppTop = ppHov ? 0xFF308840 : 0xFF226030;
		ppTextCol = 0xFFFFFFFF;
	}
	this->fill(playPauseX, ctrlY, playPauseX + playPauseW, ctrlY + 16, ppBg);
	this->fill(playPauseX, ctrlY, playPauseX + playPauseW, ctrlY + 1, ppTop);
	int32_t ppw = this->font->width(ppStr);
	this->font->drawShadow(ppStr, playPauseX + (playPauseW - ppw) / 2, ctrlY + 4, ppTextCol);

	int32_t stopX = playPauseX + playPauseW + 4;
	int32_t stopW = 38;
	bool stopHov = (mx >= stopX && mx <= stopX + stopW && my >= ctrlY && my <= ctrlY + 16);
	uint32_t stopBg = isThisPlaying ? (stopHov ? 0xFF7A2020 : 0xFF501818) : (stopHov ? 0xFF383838 : 0xFF282828);
	uint32_t stopTop = isThisPlaying ? (stopHov ? 0xFFA03030 : 0xFF702020) : (stopHov ? 0xFF505050 : 0xFF3A3A3A);
	this->fill(stopX, ctrlY, stopX + stopW, ctrlY + 16, stopBg);
	this->fill(stopX, ctrlY, stopX + stopW, ctrlY + 1, stopTop);
	int32_t sw = this->font->width("Stop");
	this->font->drawShadow("Stop", stopX + (stopW - sw) / 2, ctrlY + 4, isThisPlaying ? 0xFFFFFFFF : 0xFF888888);

	int32_t nextX = stopX + stopW + 4;
	bool nextHov = (mx >= nextX && mx <= nextX + 20 && my >= ctrlY && my <= ctrlY + 16);
	this->fill(nextX, ctrlY, nextX + 20, ctrlY + 16, nextHov ? 0xFF4A4A4A : 0xFF2F2F2F);
	this->fill(nextX, ctrlY, nextX + 20, ctrlY + 1, nextHov ? 0xFF666666 : 0xFF444444);
	int32_t nw = this->font->width(">>");
	this->font->drawShadow(">>", nextX + (20 - nw) / 2, ctrlY + 4, 0xFFFFFFFF);

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

		char tNumBuf[16];
		snprintf(tNumBuf, sizeof(tNumBuf), "%d. ", tIdx + 1);
		std::string trackLabel = std::string(tNumBuf) + tracks[tIdx].name;
		uint32_t textCol = active ? 0x66FF66 : (hov ? 0xFFFFAA : 0xDDDDDD);
		this->font->drawShadow(trackLabel, x0 + 15, iy + 5, textCol);
	}

	if (totalTracks > visibleCount) {
		int32_t scrollX = x1 - 22;
		int32_t upY = listY;
		bool canUp = (this->scrollOffset > 0);
		bool upHov = (canUp && mx >= scrollX && mx <= scrollX + 16 && my >= upY && my <= upY + 16);
		renderTransparentBtn(this->minecraft, this->font, scrollX, upY, 16, 16, "▲", upHov, canUp);

		int32_t downY = listY + visibleCount * itemH - 16;
		bool canDown = (this->scrollOffset + visibleCount < totalTracks);
		bool downHov = (canDown && mx >= scrollX && mx <= scrollX + 16 && my >= downY && my <= downY + 16);
		renderTransparentBtn(this->minecraft, this->font, scrollX, downY, 16, 16, "▼", downHov, canDown);

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
