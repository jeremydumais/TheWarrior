#pragma once

#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <string>
#include <vector>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glGameStateListEntry.hpp"
#include "glLabel.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLGameStateList : public GLComponentBase {
 public:
    explicit GLGameStateList(GLContext &glContext, thewarrior::models::Point<float> location);
    ~GLGameStateList() override = default;
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

 protected:
    thewarrior::models::Point<size_t> m_focusPosition;
    GLLabel m_playerHeaderLabel;
    GLLabel m_levelHeaderLabel;
    GLLabel m_dateSavedHeaderLabel;
    std::vector<std::unique_ptr<GLGameStateListEntry>> m_gameEntries;
};

}  // namespace thewarrior::ui::components
