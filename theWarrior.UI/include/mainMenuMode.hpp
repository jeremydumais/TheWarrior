#pragma once

#include <map>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "screens/mainMenuScreen.hpp"
#include "size.hpp"
#include "texture.hpp"

namespace thewarrior::ui {

enum class MainMenuInputMode {
    Main,
    NewGamePlayerName,
    LoadGame,
    Settings
};

class MainMenuMode {
 public:
     MainMenuMode();
     void initialize(const std::string &resourcesPath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState);
    bool initShaders(const std::string &resourcesPath);
    const std::string &getLastError() const;
    void processEvents(SDL_Event &e);
    void update();
    void render();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    boost::signals2::signal<void()> quitRequested;

 private:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    MainMenuInputMode m_inputMode = MainMenuInputMode::Main;
    GLTextureService m_textureService;
    std::shared_ptr<GLShaderProgram> m_shaderProgram = nullptr;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> m_textures;
    std::map<std::string, unsigned int> m_texturesGL;
    std::map<std::string, GLObject> m_namedObjects = {};
    std::shared_ptr<GLTexture> m_windowGLTexture;
    screens::MainMenuScreen m_mainScreen;
    void loadMenuTextures();
    void quitPressed();
};

}  // namespace thewarrior::ui
