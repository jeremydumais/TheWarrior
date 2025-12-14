#pragma once

#include <SDL2/SDL_mixer.h>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glLabel.hpp"
#include "glOnScreenKeyboardButton.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

constexpr std::array<std::string_view, 43> ONSCREENKEYBOARD_BUTTONSTEXT = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", "-",
    "z", "x", "c", "v", "b", "n", "m", "'", " .", "DEL",
    "SHIFT", "SPACE", "OK"
};

class GLOnScreenKeyboard : public GLComponentBase {
 public:
    explicit GLOnScreenKeyboard(GLContext &glContext, thewarrior::models::Point<float> location);
    ~GLOnScreenKeyboard() override;
    void initialize(const GLComponentBaseInfo &info);
    bool loadTextures();
    void setCaption(const std::string &title);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    void buttonUpPress();
    void buttonDownPress();
    void buttonLeftPress();
    void buttonRightPress();
    void buttonCancelPress();
    void buttonActionPress();
    boost::signals2::signal<void()> onOKButtonPressed;
    boost::signals2::signal<void()> onDELButtonPressed;
    boost::signals2::signal<void(char c)> onCharButtonPressed;

 protected:
    std::array<std::vector<std::shared_ptr<GLLabel>>, 5> m_buttonRows;
    thewarrior::models::Point<size_t> m_focusPosition;
    size_t m_fourthRowLastXPosition;
    bool m_isInCapsMode;
    void generateKeyboardItems();

 private:
    void updateFourthRowLastXPosition();
};

}  // namespace thewarrior::ui::components
