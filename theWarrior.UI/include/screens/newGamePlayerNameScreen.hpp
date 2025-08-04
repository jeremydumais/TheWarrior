#pragma once

#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "glPopupWindow.hpp"
#include "menuScreenBase.hpp"

namespace thewarrior::ui::screens {

class NewGamePlayerNameScreen : public MenuScreenBase {
 public:
    NewGamePlayerNameScreen(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                   std::map<std::string, unsigned int> &texturesGL);
    void initialize(const MenuScreenBaseInfo &info);
    bool loadTextures();
    void processEvents(SDL_Event &);
    void update();
    void render();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    boost::signals2::signal<void()> backPressed;
    boost::signals2::signal<void()> okPressed;

 private:
    size_t m_menuSelectedIndex = 0;
    GLPopupWindow m_menuWindow;
    void generateGLElements();
    void buttonUpPressed() override;
    void buttonDownPressed() override;
    void buttonCancelPressed() override;
    void buttonActionPressed() override;
};

}  // namespace thewarrior::ui::screens
