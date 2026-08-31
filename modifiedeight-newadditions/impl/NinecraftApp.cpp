#include <I18n.hpp>
#include <NinecraftApp.hpp>
#include <cpputils.hpp>
#include <entity/LocalPlayer.hpp>
#include <entity/MobCategory.hpp>
#include <gui/Screen.hpp>
#include <gui/screens/SplashScreen.hpp>
#include <input/Mouse.hpp>
#include <item/Item.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <level/storage/ExternalFileLevelStorageSource.hpp>
#include <math/Mth.hpp>
#include <network/RakNetInstance.hpp>
#include <network/ServerSideNetworkHandler.hpp>
#include <rendering/Font.hpp>
#include <rendering/GLBufferPool.hpp>
#include <rendering/GameRenderer.hpp>
#include <rendering/LevelRenderer.hpp>
#include <rendering/ParticleEngine.hpp>
#include <rendering/PerfRenderer.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/textures/FireTexture.hpp>
#include <rendering/textures/LavaSideTexture.hpp>
#include <rendering/textures/LavaTexture.hpp>
#include <rendering/textures/WaterSideTexture.hpp>
#include <rendering/textures/WaterTexture.hpp>
#include <rendering/textures/SeagrassTexture.hpp>
#include <rendering/textures/TallSeagrassBottomTexture.hpp>
#include <rendering/textures/TallSeagrassTopTexture.hpp>
#include <rendering/textures/FoliageTexture.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/TileEntity.hpp>
#include <tile/material/Material.hpp>
#include <unigl.h>
#include <input/Multitouch.hpp>

std::shared_ptr<TextureAtlas> NinecraftApp::_itemsTextureAtlas;
std::shared_ptr<TextureAtlas> NinecraftApp::_terrainTextureAtlas;
bool NinecraftApp::_hasInitedStatics = 0;
NinecraftApp* NinecraftApp::instance = nullptr;

NinecraftApp::NinecraftApp(){
	NinecraftApp::instance = this;
	this->field_D48___ = 1;
	this->field_D4C = 0;
	this->field_D50 = 0;
	this->field_D64 = 0;
	//TODO bsd_signal(13);
}

std::shared_ptr<TextureAtlas> NinecraftApp::getTextureAtlas(TextureAtlasId atlas){
	if(atlas) return std::shared_ptr<TextureAtlas>(NinecraftApp::_itemsTextureAtlas);
	return std::shared_ptr<TextureAtlas>(NinecraftApp::_terrainTextureAtlas);
}
void NinecraftApp::handleBackNoReturn(void){
	if(this->field_CF4){
		if(this->currentScreen){
			this->currentScreen->handleBackEvent(this->field_CF4);
		}
	}
}
void NinecraftApp::initGLStates(){
	glEnable(0xBE2u);
	glEnable(0xB71u);
	glEnable(0xB44u);
	glEnable(0xDE1u);
	glDisable(0xB60u);
	glDisable(0xB50u);
	glDisable(0xBC0u);
	glDisable(0xB90u);
	glDisable(0x8037u);
	glDisable(0xB50u);
	glDepthFunc(GL_LEQUAL);
#ifdef USEGLES
	glDepthRangef(0, 1.0);
#else
	glDepthRange(0, 1.0);
#endif
	glAlphaFunc(0x204u, 0.5);
	glCullFace(0x405u);
	glShadeModel(0x1D00u);
	glEnableClientState(0x8074u);
	glHint(0xC50u, 0x1101u);
	glHint(0xC54u, 0x1101u);
	glBlendFunc(0x302u, 0x303u);
	glDepthMask(1u);
	glStencilFunc(0x202u, 0, 0xFFu);
	glStencilMask(0xFFu);
	glLightModelf(0xB52u, 0.0);
	glPolygonOffset(-1.0, -1.0);
	this->powerVR = this->platform()->isPowerVR();
}

void NinecraftApp::restartServer(){
	Level* levelPtr; // r3
	Level* v6; // r0
	Entity* v7; // r1
	ServerSideNetworkHandler* serverSideNetworkHandler; // r0

	levelPtr = this->level;
	if(levelPtr) {
		for(int32_t i = levelPtr->playersMaybe.size() - 1; i >= 0; --i) {
			Player* player = levelPtr->playersMaybe.back();
			this->level->removePlayer(player);
			levelPtr->playersMaybe.pop_back();
		}
		this->rakNetInstance->resetIsBroken();
		this->gui.addMessage("server", "This server has restarted!", 200);
		this->hostMultiplayer(19132);
		if ( this->serverSideNetworkHandler )
		{
			serverSideNetworkHandler->levelGenerated(this->level);
		}
	}
}
void NinecraftApp::updateStats(){

}

NinecraftApp::~NinecraftApp(void){
	this->teardown();
}
bool_t NinecraftApp::onLowMemory(void){
	//TODO check
	if(glBufferPool.unusedBuffers.empty()) return 0;
	while(glBufferPool.unusedBuffers.back() != glBufferPool.unusedBuffers.front()){
		uint32_t s = glBufferPool.unusedBuffers.at(0);
		glDeleteBuffers(1, &s);
		glBufferPool.unusedBuffers.pop_front();
	}
	return 1;
}
void NinecraftApp::onAppResumed(void){
	this->initGLStates();
	Tesselator::instance.init();
	Minecraft::onAppResumed();
}
void NinecraftApp::update(void){
	//this->field_D68.lock(); //TODO std::__ndk1::system_error: unique_lock::lock: references null mutex: Operation not permitted
	if(!this->some_std_vec.empty()){
		for(int v2 = 0; v2 < this->some_std_vec.size(); ++v2){
			this->handleBackNoReturn();
		}
		this->some_std_vec.clear(); //TODO check
	}
	//this->field_D68.unlock(); //TODO should have if(v5.field_4)
	++this->field_D4C;
	Multitouch::commit();
	Minecraft::update();
#if defined(ANDROID) and defined(USEGLES)
	if(this->context.field_10){
		eglSwapBuffers(this->context.field_0, this->context.field_8);
	}
#endif
	Mouse::reset2();
	if(this->level){
		if(this->rakNetInstance->isProbablyBroken()){
			if(this->rakNetInstance->isServer()){
				this->restartServer();
			}
		}
	}
	this->updateStats();
}
bool_t NinecraftApp::handleBack(bool_t a2){
	if(!this->field_CF4){
		if(this->level){
			if(!a2) {
				if(!this->currentScreen) {
					this->pauseGame(1);
					return 0;
				}
				if(!this->currentScreen->handleBackEvent(0)) {
					if(this->player->currentContainer) {
						this->player->closeContainer();
					} else {
						this->setScreen(0);
					}
				}
			}
		}else{
			if(this->currentScreen){
				this->currentScreen->handleBackEvent(a2);
			}
		}
	}
	return 1;
}
void NinecraftApp::handleBack(void){
	this->some_std_vec.push_back(1); //TODO check
}
void NinecraftApp::init(void){
	const char* v43[] = {"/games", "/com.mojang", "/minecraftpe"};
	createTree(this->dataPathMaybe.c_str(), v43, 3);
	this->field_D00 = this->dataPathMaybe + "/games" + "/com.mojang" + "/minecraftpe";
	int32_t x = 0;
	do{
		float v7 = sin((float)x / 10430.0);
		Mth::_sin[x] = v7;
		++x;
	}while(x != 65536);
	if(!NinecraftApp::_hasInitedStatics){
		std::string v40 = "images/";
		NinecraftApp::_hasInitedStatics = 1;
		NinecraftApp::_terrainTextureAtlas = std::shared_ptr<TextureAtlas>(new TextureAtlas(v40+"terrain.meta"));
		NinecraftApp::_itemsTextureAtlas = std::shared_ptr<TextureAtlas>(new TextureAtlas(v40+"items.meta"));
		NinecraftApp::_terrainTextureAtlas->load(this);
		NinecraftApp::_itemsTextureAtlas->load(this);
		Material::initMaterials();
		MobCategory::initMobCategories();
		Tile::initTiles(NinecraftApp::_terrainTextureAtlas);
		Item::initItems(NinecraftApp::_itemsTextureAtlas);
		Biome::initBiomes();
		TileEntity::initTileEntities();
	}
	this->initGLStates();
	Tesselator::instance.init();
	I18n::loadLanguage(this->platform(), "en_US");
	Minecraft::init();

	this->levelStorageSource = new ExternalFileLevelStorageSource(this->dataPathMaybe, this->field_CC4);
	this->field_CFC = 0;
	this->texturesPtr = new Textures(&this->options, this->platform());
	this->texturesPtr->addDynamicTexture(new FireTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("fire")->getUV(0)));
	if (NinecraftApp::_terrainTextureAtlas->getTextureItem("fire")->getUVCount() > 1) {
		this->texturesPtr->addDynamicTexture(new FireTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("fire")->getUV(1)));
	}
	this->texturesPtr->addDynamicTexture(new WaterTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("still_water")->getUV(0)));
	this->texturesPtr->addDynamicTexture(new WaterSideTexture());
	this->texturesPtr->addDynamicTexture(new LavaTexture());
	this->texturesPtr->addDynamicTexture(new LavaSideTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("flowing_lava")->getUV(0)));
	if (NinecraftApp::_terrainTextureAtlas->field_4.count("seagrass")) {
		this->texturesPtr->addDynamicTexture(new SeagrassTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("seagrass")->getUV(0)));
	}
	if (NinecraftApp::_terrainTextureAtlas->field_4.count("tall_seagrass_bottom")) {
		this->texturesPtr->addDynamicTexture(new TallSeagrassBottomTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("tall_seagrass_bottom")->getUV(0)));
	}
	if (NinecraftApp::_terrainTextureAtlas->field_4.count("tall_seagrass_top")) {
		this->texturesPtr->addDynamicTexture(new TallSeagrassTopTexture(*NinecraftApp::_terrainTextureAtlas->getTextureItem("tall_seagrass_top")->getUV(0)));
	}
	struct FoliageDef {
		const char* name;
		int uvIndex;
		float speed;
		float phase;
		float amplitude;
		int fixedRows;
		bool isVine;
		bool isLeaf;
		bool isReeds;
		bool isWaterlily;
		bool isDoublePlantTop;
		bool isDoublePlantBottom;
	};
	static const FoliageDef defs[] = {
		{"tallgrass", 0, 0.035f, 0.0f, 0.8f, 2, false, false, false, false, false, false},
		{"tallgrass", 1, 0.035f, 0.0f, 0.8f, 2, false, false, false, false, false, false},
		{"tallgrass", 2, 0.026f, 1.8f, 0.7f, 2, false, false, false, false, false, false},
		{"tallgrass", 3, 0.035f, 0.0f, 0.8f, 2, false, false, false, false, false, false},
		{"tallgrass", 4, 0.026f, 1.8f, 0.7f, 2, false, false, false, false, false, false},
		{"wheat_stage_x", 0, 0.032f, 0.5f, 0.5f, 3, false, false, false, false, false, false},
		{"wheat_stage_x", 1, 0.032f, 0.8f, 0.6f, 3, false, false, false, false, false, false},
		{"wheat_stage_x", 2, 0.032f, 1.1f, 0.6f, 3, false, false, false, false, false, false},
		{"wheat_stage_x", 3, 0.032f, 1.4f, 0.7f, 3, false, false, false, false, false, false},
		{"wheat_stage_x", 4, 0.032f, 1.7f, 0.7f, 2, false, false, false, false, false, false},
		{"wheat_stage_x", 5, 0.032f, 2.0f, 0.8f, 2, false, false, false, false, false, false},
		{"wheat_stage_x", 6, 0.032f, 2.3f, 0.8f, 2, false, false, false, false, false, false},
		{"wheat_stage_x", 7, 0.032f, 2.6f, 0.9f, 2, false, false, false, false, false, false},
		{"carrots_stage_x", 0, 0.030f, 0.5f, 0.5f, 3, false, false, false, false, false, false},
		{"carrots_stage_x", 1, 0.030f, 1.0f, 0.6f, 3, false, false, false, false, false, false},
		{"carrots_stage_x", 2, 0.030f, 1.5f, 0.7f, 2, false, false, false, false, false, false},
		{"carrots_stage_x", 3, 0.030f, 2.0f, 0.8f, 2, false, false, false, false, false, false},
		{"potatoes_stage_x", 0, 0.030f, 0.7f, 0.5f, 3, false, false, false, false, false, false},
		{"potatoes_stage_x", 1, 0.030f, 1.2f, 0.6f, 3, false, false, false, false, false, false},
		{"potatoes_stage_x", 2, 0.030f, 1.7f, 0.7f, 2, false, false, false, false, false, false},
		{"potatoes_stage_x", 3, 0.030f, 2.2f, 0.8f, 2, false, false, false, false, false, false},
		{"beetroot_stage_x", 0, 0.028f, 0.6f, 0.5f, 3, false, false, false, false, false, false},
		{"beetroot_stage_x", 1, 0.028f, 1.1f, 0.6f, 3, false, false, false, false, false, false},
		{"beetroot_stage_x", 2, 0.028f, 1.6f, 0.7f, 2, false, false, false, false, false, false},
		{"beetroot_stage_x", 3, 0.028f, 2.1f, 0.8f, 2, false, false, false, false, false, false},
		{"pumpkin_stem", 0, 0.030f, 1.3f, 0.7f, 3, false, false, false, false, false, false},
		{"pumpkin_stem", 1, 0.030f, 2.3f, 0.8f, 2, false, false, false, false, false, false},
		{"melon_stem", 0, 0.030f, 1.5f, 0.7f, 3, false, false, false, false, false, false},
		{"melon_stem", 1, 0.030f, 2.5f, 0.8f, 2, false, false, false, false, false, false},
		{"sapling", 0, 0.032f, 4.2f, 0.7f, 3, false, false, false, false, false, false},
		{"sapling", 1, 0.028f, 1.4f, 0.7f, 3, false, false, false, false, false, false},
		{"sapling", 2, 0.030f, 2.8f, 0.7f, 3, false, false, false, false, false, false},
		{"sapling", 3, 0.029f, 0.5f, 0.7f, 3, false, false, false, false, false, false},
		{"leaves", 0, 0.035f, 0.0f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves", 1, 0.035f, 1.5f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves", 2, 0.035f, 3.0f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves", 3, 0.035f, 4.5f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_opaque", 0, 0.035f, 0.0f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_opaque", 1, 0.035f, 1.5f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_opaque", 2, 0.035f, 3.0f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_opaque", 3, 0.035f, 4.5f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_jungle", 0, 0.035f, 2.0f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_acacia", 0, 0.035f, 3.5f, 1.0f, 0, false, true, false, false, false, false},
		{"leaves_big_oak", 0, 0.035f, 5.0f, 1.0f, 0, false, true, false, false, false, false},
		{"flower_dandelion", 0, 0.025f, 3.1f, 0.7f, 3, false, false, false, false, false, false},
		{"flower_rose_blue", 0, 0.026f, 4.8f, 0.7f, 3, false, false, false, false, false, false},
		{"flower_paeonia", 0, 0.028f, 1.2f, 0.7f, 3, false, false, false, false, false, false},
		{"flower_oxeye_daisy", 0, 0.025f, 2.7f, 0.7f, 3, false, false, false, false, false, false},
		{"flower_houstonia", 0, 0.030f, 0.9f, 0.6f, 3, false, false, false, false, false, false},
		{"flower_blue_orchid", 0, 0.024f, 5.1f, 0.8f, 3, false, false, false, false, false, false},
		{"flower_allium", 0, 0.025f, 3.8f, 0.7f, 3, false, false, false, false, false, false},
		{"mushroom_red", 0, 0.025f, 1.5f, 0.6f, 3, false, false, false, false, false, false},
		{"mushroom_brown", 0, 0.025f, 2.5f, 0.6f, 3, false, false, false, false, false, false},
		{"reeds", 0, 0.022f, 1.5f, 0.8f, 2, false, false, true, false, false, false},
		{"cactus", 0, 0.015f, 0.0f, 0.4f, 4, false, false, false, false, false, false},
		{"cactus", 1, 0.015f, 0.0f, 0.4f, 4, false, false, false, false, false, false},
		{"cactus", 2, 0.015f, 0.0f, 0.4f, 4, false, false, false, false, false, false},
		{"vine", 0, 0.028f, 0.0f, 1.2f, 0, true, false, false, false, false, false},
		{"waterlily", 0, 0.016f, 0.5f, 0.9f, 0, false, false, false, true, false, false},
		{"double_plant_grass_top", 0, 0.035f, 0.7f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_grass_bottom", 0, 0.035f, 0.7f, 1.4f, 0, false, false, false, false, false, true},
		{"double_plant_fern_top", 0, 0.028f, 2.3f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_fern_bottom", 0, 0.028f, 2.3f, 1.4f, 0, false, false, false, false, false, true},
		{"double_plant_rose_top", 0, 0.028f, 3.9f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_rose_bottom", 0, 0.028f, 3.9f, 1.4f, 0, false, false, false, false, false, true},
		{"double_plant_sunflower_front", 0, 0.028f, 1.9f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_sunflower_top", 0, 0.028f, 1.9f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_paeonia_top", 0, 0.028f, 4.7f, 1.4f, 0, false, false, false, false, true, false},
		{"double_plant_paeonia_bottom", 0, 0.028f, 4.7f, 1.4f, 0, false, false, false, false, false, true},
		{"double_plant_syringa_top", 0, 0.028f, 2.8f, 1.4f, 0, false, false, false, false, true, false}
	};
	for (const auto& def : defs) {
		auto it = NinecraftApp::_terrainTextureAtlas->field_4.find(def.name);
		if (it != NinecraftApp::_terrainTextureAtlas->field_4.end() && def.uvIndex < it->second.getUVCount()) {
			TextureUVCoordinateSet* uv = it->second.getUV(def.uvIndex);
			if (uv && uv->minX >= 0.0f) {
				this->texturesPtr->addDynamicTexture(new FoliageTexture(*uv, def.speed, def.phase, def.amplitude, def.fixedRows, def.isVine, def.isLeaf, def.isReeds, def.isWaterlily, def.isDoublePlantTop, def.isDoublePlantBottom));
			}
		}
	}
	this->gui.texturesLoaded(this->texturesPtr);
	this->field_190 = 0;
	this->levelRenderer = new LevelRenderer(this, std::shared_ptr<TextureAtlas>(NinecraftApp::_terrainTextureAtlas));
	this->gameRenderer = new GameRenderer(this);
	this->particleEngine = new ParticleEngine(this->level, this->texturesPtr);
	this->font = new Font(this->platform(), &this->options, "font/default8.png", this->texturesPtr);
	this->perfRenderer = new PerfRenderer(this, this->font);
	this->checkGLError("Init complete");
	this->updateStatusUserAttributes();
	this->options.validateVersion();
	this->setScreen(new SplashScreen());
}

void NinecraftApp::teardown(void){
	Minecraft::teardown();
}
