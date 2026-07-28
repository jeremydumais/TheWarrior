#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <memory>
#include <string>
#include <vector>
#include "glObjectService.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui {

class GLChoicePopup: public GLPopupWindow {
 public:
    GLChoicePopup();
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    void preparePopup(std::vector<std::string> choices,
                      const std::string &title = "",
                      const std::string &prompt = "");
    void update();
    void render();
    void generateGLElements();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void setPrompt(const std::string &prompt);
    boost::signals2::signal<void(size_t choice)> m_choiceClicked;
    boost::signals2::signal<void()> m_cancelClicked;

 private:
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    size_t m_menuCursorPosition;
    size_t m_menuItemCount;
    std::string m_prompt;
    GLTextObject m_glPrompt;
    GLTextureService m_textureService;
    GLTexture m_popupGLTexture;
    std::vector<GLObject> m_menuObjects;
    std::vector<GLTextObject> m_glTextChoices;
    Uint64 lastMoveUpTicks = 0;
    Uint64 lastMoveDownTicks = 0;
    void moveUpPressed();
    void moveDownPressed();
    void actionButtonPressed();
};

}  // namespace thewarrior::ui
