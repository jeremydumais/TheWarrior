#pragma once

#include <SDL2/SDL_mixer.h>
#include <memory>
#include <string>
#include <vector>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "point.hpp"
#include "size.hpp"

namespace thewarrior::ui::components {

struct GLComponentBaseInfo {
    const std::string &resourcesPath;
    std::shared_ptr<GLShaderProgram> shaderProgram;
    std::shared_ptr<GLTextService> textService;
    std::shared_ptr<GLTextureService> textureService;
    std::shared_ptr<InputDevicesState> inputDevicesState;
    std::shared_ptr<GLTexture> texture;
    std::shared_ptr<Mix_Chunk> menuMoveSound;
    std::shared_ptr<Mix_Chunk> menuClickSound;
    std::shared_ptr<Mix_Chunk> menuClickDisableSound;
    std::shared_ptr<Mix_Chunk> menuBackSound;
};

class GLComponentBase {
 public:
    GLComponentBase(thewarrior::models::Point<float> location,
                    thewarrior::models::Size<float> size);
    virtual ~GLComponentBase() = default;
    void initialize(const GLComponentBaseInfo &info);
     void generateGLElements();
     void update();
     void render();
     void gameWindowSizeChanged(const thewarrior::models::Size<int> &size);
     thewarrior::models::Size<float> getSize() const;
     thewarrior::models::Point<float> getLocation() const;
     void setLocation(thewarrior::models::Point<float> value);

 protected:
    std::string m_resourcesPath;
    thewarrior::models::Point<float> m_location;
    thewarrior::models::Point<float> m_initialLocation;
    thewarrior::models::Size<float> m_size;
    thewarrior::models::Size<float> m_screenSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLTextService> m_textService;
    std::shared_ptr<GLTextureService> m_textureService;
    std::unique_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTexture> m_glTexture;
    std::shared_ptr<InputDevicesState> m_inputDevicesState;
    Uint64 m_lastMoveUpTicks = 0;
    Uint64 m_lastMoveDownTicks = 0;
    Uint64 m_lastMoveLeftTicks = 0;
    Uint64 m_lastMoveRightTicks = 0;
    std::shared_ptr<Mix_Chunk> m_menuBackSound;
    std::shared_ptr<Mix_Chunk> m_menuClickSound;
    std::shared_ptr<Mix_Chunk> m_menuClickDisableSound;
    std::shared_ptr<Mix_Chunk> m_menuMoveSound;
    static void freeGLObjects(std::vector<GLObject> &objects);
    void playBackSound();
    void playClickSound();
    void playClickDisableSound();
    void playMoveSound();
    GLComponentBaseInfo getComponentBaseInfo() const;
    virtual void onGenerateGLElements() {}
    virtual void onRender() {}
    virtual void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) {}
    virtual void onButtonUpPressed() {}
    virtual void onButtonDownPressed() {}
    virtual void onButtonLeftPressed() {}
    virtual void onButtonRightPressed() {}
    virtual void onButtonCancelPressed() {}
    virtual void onButtonActionPressed() {}
};

}  // namespace thewarrior::ui::components
