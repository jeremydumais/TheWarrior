#pragma once

#include <SDL2/SDL_events.h>
#include <boost/signals2.hpp>
#include "components/glComponentBase.hpp"
#include "components/glGameStateList.hpp"
#include "components/glLabel.hpp"
#include "loadGameScreenController.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"

namespace thewarrior::ui::screens {

class LoadGameScreen : public MenuScreenBase {
 public:
    explicit LoadGameScreen(GLContext &glContext);
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
    controllers::LoadGameScreenController m_controller;
    components::GLGameStateList m_gameStateList;
    components::GLLabel m_loadGameLabel;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;
    void keyboardOKButtonPressed();
};

}  // namespace thewarrior::ui::screens
