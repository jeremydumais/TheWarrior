#pragma once

#include <SDL2/SDL_mixer.h>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glOnScreenKeyboardButton.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLOnScreenKeyboard {
 public:
    explicit GLOnScreenKeyboard(thewarrior::models::Point<float> location);
    virtual ~GLOnScreenKeyboard() = default;
    void initialize(const std::shared_ptr<GLTexture> texture,
                    const std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<Mix_Chunk> menuMoveSound,
                    std::shared_ptr<Mix_Chunk> menuClickSound);
    void setCaption(const std::string &title);
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    void buttonUpPress();
    void buttonDownPress();
    void buttonLeftPress();
    void buttonRightPress();
    void buttonActionPress();
    boost::signals2::signal<void()> onClickEvent;

 protected:
    thewarrior::models::Point<float> m_location;
    thewarrior::models::Point<float> m_initialLocation;
    thewarrior::models::Size<float> m_size;
    thewarrior::models::Point<float> m_windowCenter;
    thewarrior::models::Size<float> m_screenSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;
    std::shared_ptr<GLTexture> m_windowGLTexture;
    GLObject m_glwindow;
    std::array<std::vector<std::shared_ptr<GLOnScreenKeyboardButton>>, 5> m_buttonRows;
    GLTextObject m_enterNameObject;
    thewarrior::models::Point<size_t> m_focusPosition;
    size_t m_fourthRowLastXPosition;
    bool m_isInCapsMode;
    std::shared_ptr<Mix_Chunk> m_menuMoveSound;
    std::shared_ptr<Mix_Chunk> m_menuClickSound;
    void generateKeyboardItems();
    void playMoveSound();
    void playClickSound();

 private:
    void updateFourthRowLastXPosition();
};

}  // namespace thewarrior::ui::components
