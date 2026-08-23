#pragma once
#include <_types.h>
#include <gui/Screen.hpp>
#include <gui/ScreenId.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>

struct TexturesScreen : Screen {
    Screen* lastScreen;

    Touch::TButton* btnBack;
    Touch::TButton* btnImport;

    struct SavedTexture {
        std::string path;
        std::string title;
    };
    std::vector<SavedTexture> savedTextures;
    std::vector<Touch::TButton*> managedActionButtons;
    std::vector<Touch::TButton*> managedDeleteButtons;
    std::string activeTexturePack;

    int scrollOffset;
    int prevMouseY;
    bool isDragging;

    bool installRunning, installDone;
    std::string installMsg;
    std::string statusMsg;
    bool needsRefresh;
    bool shouldClose;
    std::string pendingDeletePath;

    std::shared_ptr<bool> alive;

    TexturesScreen(Screen* lastScreen = nullptr);
    virtual ~TexturesScreen();

    virtual void    init();
    virtual void    setupPositions();
    virtual void    render(int32_t, int32_t, float);
    virtual bool_t  handleBackEvent(bool_t);
    virtual void    tick();
    virtual bool_t  isInGameScreen();
    virtual void    buttonClicked(Button*);
    virtual void    mouseClicked(int32_t, int32_t, int32_t);
    virtual void    mouseReleased(int32_t, int32_t, int32_t);

    void refreshSavedTextures();
    void importTexturePack();
    void applySavedTexture(const std::string& path, const std::string& title);
    void deleteSavedTexture(const std::string& path);
    void deactivateTextures();

    static std::string pickZipFile();
};

using MarketplaceScreen = TexturesScreen;
using TexturePackScreen = TexturesScreen;
