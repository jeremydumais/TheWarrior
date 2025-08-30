#pragma once

#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "components/glOnScreenKeyboard.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "menuScreenBase.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::screens {

class NewGamePlayerNameScreen : public MenuScreenBase {
 public:
    NewGamePlayerNameScreen(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                   std::map<std::string, unsigned int> &texturesGL);
    void initialize(const MenuScreenBaseInfo &info);
    bool loadTextures();
    void processEvents(SDL_Event &e);
    void update();
    void render();
    void unloadGLMapObjects();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    boost::signals2::signal<void()> backPressed;
    boost::signals2::signal<void()> okPressed;

 private:
    std::shared_ptr<GLTextService> m_textService;
    GLPopupWindow m_menuWindow;
    components::GLOnScreenKeyboard m_onScreenKeyboard;
    GLTextObject m_enterNameObject;
    std::string m_playerName;
    thewarrior::models::Point<float> m_playerNameLocation;
    thewarrior::models::Point<float> m_playerNameInitialLocation;
    void generateGLElements();
    void buttonUpPressed() override;
    void buttonDownPressed() override;
    void buttonLeftPressed() override;
    void buttonRightPressed() override;
    void buttonCancelPressed() override;
    void buttonActionPressed() override;
    bool addPlayerNameChar(char c);
    bool removePlayerNameChar();
    void keyboardCharButtonPressed(char c);
    void keyboardDELButtonPressed();
};

}  // namespace thewarrior::ui::screens
