#pragma once

#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <boost/signals2.hpp>
#include "components/glMenuModalDialog.hpp"
#include "components/glComponentBase.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "menuScreenBase.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::screens {

class LoadGameScreen : public MenuScreenBase {
 public:
    LoadGameScreen(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                   std::map<std::string, unsigned int> &texturesGL);
    void initialize(const components::GLComponentBaseInfo &info);
    bool loadTextures();
    void processEvents(SDL_Event &e);
    void update();
    void onGenerateGLElements() override;
    void onRender() override;
    void unloadGLMapObjects();
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    boost::signals2::signal<void()> backPressed;
    boost::signals2::signal<void()> okPressed;

 protected:
    GLPopupWindow m_menuWindow;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonLeftPressed() override;
    void onButtonRightPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;
    void keyboardOKButtonPressed();
};

}  // namespace thewarrior::ui::screens
