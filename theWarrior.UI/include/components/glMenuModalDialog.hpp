#pragma once

#include <SDL2/SDL_mixer.h>
#include <memory>
#include <string>
#include <vector>
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
    void initialize(const std::shared_ptr<GLTexture> texture,
            const std::shared_ptr<GLShaderProgram> shaderProgram,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<Mix_Chunk> menuMoveSound,
            std::shared_ptr<Mix_Chunk> menuClickSound);
    bool isVisible() const;
    bool isAutoSize() const;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    void show();
    void hide();
    void setAutoSize(bool value);
    void setMessage(const std::string &message);

 private:
    std::vector<GLTextObject> m_glMessageLines;
    bool m_visible;
    bool m_autoSize;
    GLPopupWindow m_menuWindow;
    GLMenuButton m_menuButtonOK;
};

}  // namespace thewarrior::ui::components
