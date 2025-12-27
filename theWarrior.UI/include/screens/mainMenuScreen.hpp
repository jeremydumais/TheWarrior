#pragma once

#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <boost/signals2.hpp>
#include "components/glMenuButton.hpp"
#include "components/glComponentBase.hpp"
#include "glPopupWindow.hpp"
#include "menuScreenBase.hpp"
#include "size.hpp"

namespace thewarrior::ui::screens {

class MainMenuScreen : public MenuScreenBase {
 public:
    MainMenuScreen(GLContext &glContext);
    ~MainMenuScreen() override;
    void onInitialize(const components::GLComponentBaseInfo &info) override;
    bool loadTextures();
    void processEvents(SDL_Event &);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    boost::signals2::signal<void()> newGamePressed;
    boost::signals2::signal<void()> loadGamePressed;
    boost::signals2::signal<void()> quitPressed;

 private:
    size_t m_menuSelectedIndex = 0;
    GLPopupWindow m_menuWindow;
    thewarrior::ui::components::GLMenuButton m_menuButtonNewGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonLoadGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonSettings;
    thewarrior::ui::components::GLMenuButton m_menuButtonQuit;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;
};

}  // namespace thewarrior::ui::screens
