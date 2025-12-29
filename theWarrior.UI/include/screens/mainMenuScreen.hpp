#pragma once

#include <SDL2/SDL_mixer.h>
#include <boost/signals2.hpp>
#include "components/glPanel.hpp"
#include "components/glMenuButton.hpp"
#include "components/glComponentBase.hpp"
#include "menuScreenBase.hpp"

namespace thewarrior::ui::screens {

class MainMenuScreen : public MenuScreenBase {
 public:
    explicit MainMenuScreen(GLContext &glContext);
    ~MainMenuScreen() override;
    bool loadTextures();
    void processEvents(SDL_Event &);
    boost::signals2::signal<void()> newGamePressed;
    boost::signals2::signal<void()> loadGamePressed;
    boost::signals2::signal<void()> quitPressed;

 private:
    size_t m_menuSelectedIndex = 0;
    thewarrior::ui::components::GLMenuButton m_menuButtonNewGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonLoadGame;
    thewarrior::ui::components::GLMenuButton m_menuButtonSettings;
    thewarrior::ui::components::GLMenuButton m_menuButtonQuit;
    thewarrior::ui::components::GLPanel m_menuPanel;
    void onInitialize(const components::GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;
};

}  // namespace thewarrior::ui::screens
