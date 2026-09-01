#include <sound/MusicPlayerManager.hpp>
#include <sound/MusicEngine.hpp>
#include <Minecraft.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <_AssetFile.hpp>
#include <json/reader.h>
#include <cmath>
#include <fstream>
#include <iostream>

#include <dirent.h>

MusicPlayerManager MusicPlayerManager::instance;

MusicPlayerManager::MusicPlayerManager()
	: isPlaying(false), isPaused(false), pauseFade(1.0f), isPausing(false), isResuming(false),
	  currentTrackIndex(-1), blockX(0), blockY(0), blockZ(0),
	  volumeFade(0.0f), isFadingOut(false), nextTrackIndex(-1), nextX(0), nextY(0), nextZ(0),
	  playbackMode(PLAYBACK_SEQUENTIAL) {
}

void MusicPlayerManager::loadTracks() {
	this->tracks.clear();

	struct EmbeddedTrackInfo {
		const char* name;
		const unsigned char* data;
		const unsigned char* end;
	};
	static const EmbeddedTrackInfo embedded[] = {
		{"Minecraft", music_minecraft_start, music_minecraft_end},
		{"Subwoofer Lullaby", music_subwoofer_lullaby_start, music_subwoofer_lullaby_end},
		{"Living Mice", music_living_mice_start, music_living_mice_end},
		{"Haggstrom", music_haggstrom_start, music_haggstrom_end},
		{"Danny", music_danny_start, music_danny_end},
		{"Mice on Venus", music_mice_on_venus_start, music_mice_on_venus_end},
		{"Key", music_key_start, music_key_end},
		{"Oxygene", music_oxygene_start, music_oxygene_end},
		{"Equinoxe", music_equinoxe_start, music_equinoxe_end},
		{"Door", music_door_start, music_door_end},
		{"Moog City 2", music_moog_city_2_start, music_moog_city_2_end}
	};
	for (size_t i = 0; i < sizeof(embedded) / sizeof(embedded[0]); ++i) {
		CustomMusicTrack t;
		t.name = embedded[i].name;
		t.file = "";
		t.data = embedded[i].data;
		t.size = (size_t)(embedded[i].end - embedded[i].data);
		this->tracks.push_back(t);
	}

	std::string paths[] = {
		"music_data/tracks.json",
		"../music_data/tracks.json",
		"assets/../music_data/tracks.json",
		"assets/music_data/tracks.json",
		"music_data/music.json",
		"assets/music_data/music.json"
	};

	std::string jsonContent;
	for (const auto& p : paths) {
		std::ifstream infile(p);
		if (infile.is_open()) {
			std::string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
			if (!str.empty()) {
				jsonContent = str;
				break;
			}
		}
	}

	if (jsonContent.empty() && AppPlatform::_singleton) {
		AssetFile f = AppPlatform::_singleton->readAssetFile("music_data/tracks.json");
		if (f.bytes && f.length > 0) {
			jsonContent = std::string((char*)f.bytes, f.length);
			delete[] f.bytes;
		}
	}

	if (!jsonContent.empty()) {
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(jsonContent, root) && root.isArray()) {
			for (unsigned int i = 0; i < root.size(); ++i) {
				const Json::Value& item = root[i];
				if (item.isObject() && item.isMember("name") && item.isMember("file")) {
					CustomMusicTrack t;
					t.name = item["name"].asString();
					t.file = item["file"].asString();
					t.data = nullptr;
					t.size = 0;
					bool exists = false;
					for (const auto& existing : this->tracks) {
						if (existing.name == t.name && existing.file == t.file) { exists = true; break; }
					}
					if (!exists) this->tracks.push_back(t);
				}
			}
		}
	}

	const char* scanDirs[] = {"music_data", "assets/../music_data", "../music_data", "assets/music_data"};
	for (const char* d : scanDirs) {
		DIR* dir = opendir(d);
		if (dir) {
			struct dirent* ent;
			while ((ent = readdir(dir)) != nullptr) {
				std::string fname = ent->d_name;
				if (fname.size() > 4) {
					std::string ext = fname.substr(fname.size() - 4);
					if (ext == ".aac" || ext == ".ogg" || ext == ".wav" || ext == ".mp3") {
						std::string baseName = fname.substr(0, fname.size() - 4);
						bool already = false;
						for (const auto& tr : this->tracks) {
							if (tr.file == fname || tr.name == baseName) { already = true; break; }
						}
						if (!already) {
							CustomMusicTrack t;
							t.name = baseName;
							t.file = fname;
							t.data = nullptr;
							t.size = 0;
							this->tracks.push_back(t);
						}
					}
				}
			}
			closedir(dir);
		}
	}
}

void MusicPlayerManager::playTrack(int32_t trackIndex, int32_t x, int32_t y, int32_t z) {
	if (this->tracks.empty()) {
		this->loadTracks();
	}
	if (trackIndex < 0 || trackIndex >= (int32_t)this->tracks.size()) {
		return;
	}

	bool found = false;
	for (auto& pj : this->activePlayers) {
		if (pj.blockX == x && pj.blockY == y && pj.blockZ == z) {
			pj.trackIndex = trackIndex;
			pj.volumeFade = 0.0f;
			pj.isFadingOut = false;
			pj.nextTrackIndex = -1;
			found = true;
			break;
		}
	}
	if (!found) {
		PlayingJukebox pj;
		pj.trackIndex = trackIndex;
		pj.blockX = x;
		pj.blockY = y;
		pj.blockZ = z;
		pj.volumeFade = 0.0f;
		pj.isFadingOut = false;
		pj.nextTrackIndex = -1;
		this->activePlayers.push_back(pj);
	}

	const CustomMusicTrack& t = this->tracks[trackIndex];
	const unsigned char* playData = t.data;
	size_t playSize = t.size;

	if (!playData || playSize == 0) {
		this->loadedFileBytes.clear();
		std::string tryPaths[] = {
			std::string("music_data/") + t.file,
			std::string("../music_data/") + t.file,
			std::string("assets/../music_data/") + t.file,
			std::string("assets/music_data/") + t.file,
			std::string("assets/") + t.file,
			t.file
		};

		for (const auto& p : tryPaths) {
			std::ifstream infile(p, std::ios::binary);
			if (infile.is_open()) {
				infile.seekg(0, std::ios::end);
				size_t sz = infile.tellg();
				infile.seekg(0, std::ios::beg);
				if (sz > 0) {
					this->loadedFileBytes.resize(sz);
					infile.read((char*)this->loadedFileBytes.data(), sz);
					break;
				}
			}
		}

		if (this->loadedFileBytes.empty() && AppPlatform::_singleton) {
			AssetFile f = AppPlatform::_singleton->readAssetFile(std::string("music_data/") + t.file);
			if (!f.bytes || f.length <= 0) {
				f = AppPlatform::_singleton->readAssetFile(t.file);
			}
			if (f.bytes && f.length > 0) {
				this->loadedFileBytes.assign((uint8_t*)f.bytes, (uint8_t*)f.bytes + f.length);
				delete[] f.bytes;
			}
		}

		if (this->loadedFileBytes.empty()) {
			return;
		}
		playData = this->loadedFileBytes.data();
		playSize = this->loadedFileBytes.size();
	}

	if (MusicEngine::instance) {
		MusicEngine::instance->currentMode = MUSIC_MODE_CUSTOM;
		MusicEngine::instance->pauseSeconds = 60.0f;
		MusicTrack mt;
		mt.name = t.name.c_str();
		mt.data = playData;
		mt.size = playSize;
		MusicEngine::instance->playTrack(mt);
		MusicEngine::instance->currentMode = MUSIC_MODE_CUSTOM;
	}

	this->isPlaying = true;
	this->isPaused = false;
	this->pauseFade = 1.0f;
	this->isPausing = false;
	this->isResuming = false;
	this->currentTrackIndex = trackIndex;
	this->blockX = x;
	this->blockY = y;
	this->blockZ = z;
	this->volumeFade = 0.0f;
	this->isFadingOut = false;
	this->nextTrackIndex = -1;
}

void MusicPlayerManager::pause() {
	if (this->isPlaying && !this->isPaused && !this->isPausing) {
		this->isPausing = true;
		this->isResuming = false;
	}
}

void MusicPlayerManager::resume() {
	if (this->isPlaying && (this->isPaused || this->isPausing)) {
		if (MusicEngine::instance) {
			MusicEngine::instance->resumeStream();
		}
		this->isPaused = false;
		this->isPausing = false;
		this->isResuming = true;
	}
}

void MusicPlayerManager::togglePause() {
	if (this->isPaused || this->isPausing) {
		this->resume();
	} else if (this->isPlaying) {
		this->pause();
	}
}

void MusicPlayerManager::stop() {
	if (this->isPlaying) {
		this->isFadingOut = true;
		this->nextTrackIndex = -1;
	}
	for (auto& pj : this->activePlayers) {
		pj.isFadingOut = true;
	}
}

void MusicPlayerManager::stopAt(int32_t x, int32_t y, int32_t z) {
	for (auto& pj : this->activePlayers) {
		if (pj.blockX == x && pj.blockY == y && pj.blockZ == z) {
			pj.isFadingOut = true;
		}
	}
	if (this->blockX == x && this->blockY == y && this->blockZ == z) {
		this->stop();
	}
}

void MusicPlayerManager::stopImmediate() {
	this->isPlaying = false;
	this->isPaused = false;
	this->isPausing = false;
	this->isResuming = false;
	this->pauseFade = 1.0f;
	this->isFadingOut = false;
	this->volumeFade = 0.0f;
	this->currentTrackIndex = -1;
	this->nextTrackIndex = -1;
	this->activePlayers.clear();
	if (MusicEngine::instance) {
		MusicEngine::instance->stop();
		MusicEngine::instance->currentMode = MUSIC_MODE_NONE;
	}
}

void MusicPlayerManager::cyclePlaybackMode() {
	this->playbackMode = (this->playbackMode + 1) % 4;
}

std::string MusicPlayerManager::getPlaybackModeName() const {
	switch (this->playbackMode) {
		case PLAYBACK_SEQUENTIAL: return "Order";
		case PLAYBACK_SHUFFLE: return "Shuffle";
		case PLAYBACK_REPEAT_ONE: return "Repeat 1";
		case PLAYBACK_SINGLE: return "Single";
		default: return "Order";
	}
}

void MusicPlayerManager::playNextTrack() {
	if (this->tracks.empty()) return;
	int count = (int)this->tracks.size();
	int nextIdx = 0;
	if (this->playbackMode == PLAYBACK_SHUFFLE) {
		if (count > 1) {
			do {
				nextIdx = rand() % count;
			} while (nextIdx == this->currentTrackIndex);
		} else {
			nextIdx = 0;
		}
	} else if (this->playbackMode == PLAYBACK_REPEAT_ONE) {
		nextIdx = (this->currentTrackIndex >= 0) ? this->currentTrackIndex : 0;
	} else {
		nextIdx = (this->currentTrackIndex + 1) % count;
	}
	this->playTrack(nextIdx, this->blockX, this->blockY, this->blockZ);
}

void MusicPlayerManager::playPrevTrack() {
	if (this->tracks.empty()) return;
	int count = (int)this->tracks.size();
	int prevIdx = (this->currentTrackIndex - 1 + count) % count;
	this->playTrack(prevIdx, this->blockX, this->blockY, this->blockZ);
}

void MusicPlayerManager::onTrackFinished() {
	this->isPlaying = false;
	this->isPaused = false;
	this->isPausing = false;
	this->isResuming = false;
	this->pauseFade = 1.0f;
	this->volumeFade = 0.0f;
	if (this->playbackMode != PLAYBACK_SINGLE && !this->tracks.empty()) {
		this->playNextTrack();
		return;
	}
	this->currentTrackIndex = -1;
	this->activePlayers.clear();
	if (MusicEngine::instance) {
		MusicEngine::instance->currentMode = MUSIC_MODE_NONE;
		MusicEngine::instance->pauseSeconds = 30.0f;
	}
}

bool MusicPlayerManager::isPlayingAt(int32_t x, int32_t y, int32_t z) const {
	for (const auto& pj : this->activePlayers) {
		if (pj.blockX == x && pj.blockY == y && pj.blockZ == z && pj.volumeFade > 0.01f) {
			return true;
		}
	}
	return this->isPlaying && this->blockX == x && this->blockY == y && this->blockZ == z;
}

std::string MusicPlayerManager::getCurrentTrackName() const {
	if (this->isPlaying && this->currentTrackIndex >= 0 && this->currentTrackIndex < (int32_t)this->tracks.size()) {
		return this->tracks[this->currentTrackIndex].name;
	}
	return "";
}

std::string MusicPlayerManager::getTrackNameAt(int32_t x, int32_t y, int32_t z) const {
	for (const auto& pj : this->activePlayers) {
		if (pj.blockX == x && pj.blockY == y && pj.blockZ == z && pj.trackIndex >= 0 && pj.trackIndex < (int32_t)this->tracks.size()) {
			return this->tracks[pj.trackIndex].name;
		}
	}
	return this->getCurrentTrackName();
}

void MusicPlayerManager::update(Minecraft* mc) {
	if (!mc || !mc->player) {
		return;
	}

	if (this->isPausing) {
		this->pauseFade -= 0.08f;
		if (this->pauseFade <= 0.0f) {
			this->pauseFade = 0.0f;
			this->isPausing = false;
			this->isPaused = true;
			if (MusicEngine::instance) {
				MusicEngine::instance->pauseStream();
			}
		}
	} else if (this->isResuming) {
		this->pauseFade += 0.08f;
		if (this->pauseFade >= 1.0f) {
			this->pauseFade = 1.0f;
			this->isResuming = false;
		}
	}

	float maxBlendedVol = 0.0f;
	static int s_particleTick = 0;
	++s_particleTick;

	for (size_t i = 0; i < this->activePlayers.size();) {
		auto& pj = this->activePlayers[i];
		if (mc->level && Tile::musicPlayer) {
			int curTile = mc->level->getTile(pj.blockX, pj.blockY, pj.blockZ);
			if (curTile != Tile::musicPlayer->blockID) {
				pj.isFadingOut = true;
			}
		}

		if (pj.isFadingOut) {
			pj.volumeFade -= 0.06f;
			if (pj.volumeFade <= 0.0f) {
				this->activePlayers.erase(this->activePlayers.begin() + i);
				continue;
			}
		} else if (pj.volumeFade < 1.0f && !this->isPaused) {
			pj.volumeFade += 0.04f;
			if (pj.volumeFade > 1.0f) pj.volumeFade = 1.0f;
		}

		float dx = ((float)pj.blockX + 0.5f) - mc->player->posX;
		float dy = ((float)pj.blockY + 0.5f) - mc->player->posY;
		float dz = ((float)pj.blockZ + 0.5f) - mc->player->posZ;
		float dist = sqrtf(dx * dx + dy * dy + dz * dz);

		float maxDist = 48.0f;
		float refDist = 2.0f;
		float spatial = 0.0f;
		if (dist <= refDist) {
			spatial = 1.0f;
		} else if (dist < maxDist) {
			float norm = (dist - refDist) / (maxDist - refDist);
			spatial = (1.0f - norm) * (1.0f - norm);
		}

		float vol = spatial * pj.volumeFade;
		if (vol > maxBlendedVol) {
			maxBlendedVol = vol;
		}

		if ((s_particleTick % 16) == 0 && mc->level && spatial > 0.1f && !this->isPaused) {
			mc->level->addParticle(PT_SMOKE, (float)pj.blockX + 0.5f, (float)pj.blockY + 1.2f, (float)pj.blockZ + 0.5f, 0.0, 0.04, 0.0, 0);
		}
		++i;
	}

	if (this->isFadingOut) {
		this->volumeFade -= 0.05f;
		if (this->volumeFade <= 0.0f) {
			this->volumeFade = 0.0f;
			this->isPlaying = false;
			this->isFadingOut = false;
			if (MusicEngine::instance) MusicEngine::instance->stop();
			if (this->nextTrackIndex >= 0) {
				int t = this->nextTrackIndex;
				int nx = this->nextX, ny = this->nextY, nz = this->nextZ;
				this->nextTrackIndex = -1;
				this->playTrack(t, nx, ny, nz);
			}
			return;
		}
	} else if (this->volumeFade < 1.0f && !this->isPaused) {
		this->volumeFade += 0.04f;
		if (this->volumeFade > 1.0f) this->volumeFade = 1.0f;
	}

	if (this->activePlayers.empty() && !this->isPlaying) {
		return;
	}

	if (this->activePlayers.empty() && this->isPlaying) {
		this->isPlaying = false;
		if (MusicEngine::instance) MusicEngine::instance->stop();
		return;
	}

	float masterVol = mc->options.musicVolume;
	float finalVol = maxBlendedVol * this->volumeFade * this->pauseFade * masterVol;
	if (MusicEngine::instance) {
		MusicEngine::instance->setVolume(finalVol);
	}
}
