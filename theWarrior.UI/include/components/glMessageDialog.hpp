#pragma once

#include <string>
#include <vector>
#include <boost/signals2.hpp>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glLabel.hpp"
#include "glMenuButton.hpp"
#include "glPanel.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::components {

class GLMessageDialog : public GLComponentBase {
 public:
    explicit GLMessageDialog(GLContext &glContext,
                               thewarrior::models::Point<float> location,
                               thewarrior::models::Size<float> size);
    ~GLMessageDialog() override = default;
    bool isVisible() const;
    void show();
    void hide();
    void setMessage(const std::string &message);
    boost::signals2::signal<void()> onClosed;

 protected:
    std::vector<GLTextObject> m_glMessageLines;
    bool m_visible;
    GLPanel m_menuPanel;
    GLLabel m_messageLabel;
    GLMenuButton m_menuButtonOK;
    void onInitialize(const GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    void onButtonActionPressed() override;
    void onButtonCancelPressed() override;
};

}  // namespace thewarrior::ui::components
