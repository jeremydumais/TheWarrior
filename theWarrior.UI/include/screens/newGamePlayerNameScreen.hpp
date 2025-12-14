#pragma once

#include <SDL2/SDL_events.h>
#include <string>
#include <string_view>
#include <boost/signals2.hpp>
#include "components/glOnScreenKeyboard.hpp"
#include "components/glMenuModalDialog.hpp"
#include "components/glComponentBase.hpp"
#include "components/glLabel.hpp"
#include "glContext.hpp"
#include "glTextService.hpp"
#include "menuScreenBase.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::screens {

class NewGamePlayerNameScreen : public MenuScreenBase {
 public:
    NewGamePlayerNameScreen(GLContext &glContext);
    ~NewGamePlayerNameScreen() override;
    void initialize(const components::GLComponentBaseInfo &info);
    bool loadTextures();
    void processEvents(SDL_Event &e);
    void update();
    void onGenerateGLElements() override;
    void onRender() override;
    void unloadGLMapObjects();
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    const std::string &getPlayerName() const;
    boost::signals2::signal<void()> backPressed;
    boost::signals2::signal<void()> okPressed;

 protected:
    components::GLOnScreenKeyboard m_onScreenKeyboard;
    components::GLLabel m_newGameLabel;
    components::GLLabel m_enterPlayerNameLabel;
    components::GLLabel m_playerNameLabel;
    GLTextObject m_enterNameObject;
    std::string m_playerName;
    thewarrior::models::Point<float> m_playerNameLocation;
    thewarrior::models::Point<float> m_playerNameInitialLocation;
    components::GLMenuModalDialog m_modalDialog;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonLeftPressed() override;
    void onButtonRightPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;
    bool addPlayerNameChar(char c);
    bool removePlayerNameChar();
    void keyboardCharButtonPressed(char c);
    void keyboardDELButtonPressed();
    void keyboardOKButtonPressed();
    bool hasAtLeastTwoAlphaAscii(std::string_view value);
};

}  // namespace thewarrior::ui::screens
