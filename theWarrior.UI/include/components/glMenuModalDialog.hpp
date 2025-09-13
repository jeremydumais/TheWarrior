#pragma once

#include <string>
#include <vector>
#include <boost/signals2.hpp>
#include "glComponentBase.hpp"
#include "glMenuButton.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::components {

class GLMenuModalDialog : public GLComponentBase {
 public:
    explicit GLMenuModalDialog(thewarrior::models::Point<float> location,
                               thewarrior::models::Size<float> size);
    ~GLMenuModalDialog() override = default;
    void initialize(const GLComponentBaseInfo &info);
    bool isVisible() const;
    bool isAutoSize() const;
    void show();
    void hide();
    void setAutoSize(bool value);
    void setMessage(const std::string &message);
    boost::signals2::signal<void()> onOKButtonPressed;

 private:
    std::vector<GLTextObject> m_glMessageLines;
    bool m_visible;
    bool m_autoSize;
    GLPopupWindow m_menuWindow;
    GLMenuButton m_menuButtonOK;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    void onButtonActionPressed() override;
    void onButtonCancelPressed() override;
};

}  // namespace thewarrior::ui::components
