#pragma once

#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "components/glMenuButton.hpp"
#include "glPopupWindow.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "inputDevicesState.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"
#include "texture.hpp"

namespace thewarrior::ui::screens {

class MainMenuScreen : public MenuScreenBase {
 public:
     MainMenuScreen(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                    std::map<std::string, unsigned int> &texturesGL);
     void initialize(const MenuScreenBaseInfo &info);
    void processEvents(SDL_Event &);
    void update();
    void render();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    boost::signals2::signal<void()> quitPressed;

 private:
    size_t m_menuSelectedIndex = 0;
    GLPopupWindow m_menuWindow;
    thewarrior::ui::components::GLMenuButton m_menuButtonNewGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonLoadGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonSettings;
    thewarrior::ui::components::GLMenuButton m_menuButtonQuit;
    void generateGLElements();
    void buttonUpPressed() override;
    void buttonDownPressed() override;
    void buttonActionPressed() override;
};

}  // namespace thewarrior::ui::screens
