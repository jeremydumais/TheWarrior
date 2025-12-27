#pragma once

#include <SDL2/SDL_events.h>
#include <string>
#include <vector>
#include <boost/signals2.hpp>
#include "components/glComponentBase.hpp"
#include "components/glGameStateList.hpp"
#include "components/glLabel.hpp"
#include "components/glMenuButton.hpp"
#include "loadGameScreenController.hpp"
#include "menuScreenBase.hpp"

namespace thewarrior::ui::screens {

class LoadGameScreen : public MenuScreenBase {
 public:
    explicit LoadGameScreen(GLContext &glContext);
    void onInitialize(const components::GLComponentBaseInfo &info) override;
    bool loadTextures();
    void update();
    void onGenerateGLElements() override;
    void onRender() override;
    void reset();
    const std::string &getSelectedGameStateFileName() const;
    boost::signals2::signal<void()> backPressed;
    boost::signals2::signal<void()> okPressed;

 protected:
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;

 private:
    enum class FocusElement {
        GameStateList,
        LoadButton
    };
    controllers::LoadGameScreenController m_controller;
    components::GLGameStateList m_gameStateList;
    components::GLLabel m_loadGameLabel;
    components::GLMenuButton m_menuButtonLoad;
    std::vector<storage::GameStateMetadata> m_gameStates;
    FocusElement m_focusElement;
    void gameStateListOKButtonPressed();
    void gameStateListCancelButtonPressed();
};

}  // namespace thewarrior::ui::screens
