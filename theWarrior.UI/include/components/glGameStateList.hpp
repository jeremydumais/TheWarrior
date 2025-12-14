#pragma once

#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <string>
#include "glComponentBase.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLGameStateList : public GLComponentBase {
 public:
    explicit GLGameStateList(thewarrior::models::Point<float> location);
    ~GLGameStateList() override = default;
    void initialize(const GLComponentBaseInfo &info);
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

 protected:
    thewarrior::models::Point<size_t> m_focusPosition;
};

}  // namespace thewarrior::ui::components
