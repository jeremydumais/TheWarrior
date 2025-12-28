#pragma once

#include <SDL2/SDL_mixer.h>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "glContext.hpp"
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

enum class HorizontalAlignment {
    Left,
    Center,
    Right
};

enum class VerticalAlignment {
    Top,
    Center,
    Bottom
};

class GLComponentBase {
 public:
    explicit GLComponentBase(GLContext &glContext,
                             thewarrior::models::Point<float> location = thewarrior::models::Point<float>(0.0F, 0.0F),
                             thewarrior::models::Size<float> size = thewarrior::models::Size<float>(0.0F, 0.0F),
                             HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center,
                             VerticalAlignment verticalAlignment = VerticalAlignment::Center);
    virtual ~GLComponentBase();
    void initialize(const GLComponentBaseInfo &info);
    void registerComponent(GLComponentBase *component);
    const std::string &getLastError() const;
    HorizontalAlignment getHorizontalAlignment() const;
    VerticalAlignment getVerticalAlignment() const;
    void generateGLElements();
    void update();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<int> &size);
    thewarrior::models::Size<float> getSize() const;
    thewarrior::models::Point<float> getLocation() const;
    void setLocation(thewarrior::models::Point<float> value);
    void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);
    void setVerticalAlignment(VerticalAlignment verticalAlignment);

 protected:
    std::vector<GLComponentBase *> m_registeredComponents;
    std::unique_ptr<GLComponentBaseInfo> m_initializationInfo;
    std::string m_resourcesPath;
    std::string m_lastError = "";
    thewarrior::models::Point<float> m_location;
    thewarrior::models::Point<float> m_initialLocation;
    thewarrior::models::Size<float> m_size;
    thewarrior::models::Size<float> m_screenSize;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLTextService> m_textService;
    std::shared_ptr<GLTextureService> m_textureService;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTexture> m_glTexture;
    std::shared_ptr<InputDevicesState> m_inputDevicesState;
    Uint64 m_lastMoveUpTicks = 0;
    Uint64 m_lastMoveDownTicks = 0;
    GLContext &m_glContext;
    std::map<std::string, GLObject> m_namedObjects = {};
    HorizontalAlignment m_horizontalAlignment;
    VerticalAlignment m_verticalAlignment;
    Uint64 m_lastMoveLeftTicks = 0;
    Uint64 m_lastMoveRightTicks = 0;
    std::shared_ptr<Mix_Chunk> m_menuBackSound;
    std::shared_ptr<Mix_Chunk> m_menuClickSound;
    std::shared_ptr<Mix_Chunk> m_menuClickDisableSound;
    std::shared_ptr<Mix_Chunk> m_menuMoveSound;
    static void freeGLObjects(std::vector<GLObject> &objects);
    static void freeGLObjects(std::map<std::string, GLObject> &objects);
    void playBackSound();
    void playClickSound();
    void playClickDisableSound();
    void playMoveSound();
    thewarrior::models::Size<float> getGLSizeFromPx(thewarrior::models::Size<int> value) const;
    GLComponentBaseInfo getComponentBaseInfo() const;
    thewarrior::models::Point<float> getRelativeCenterPosition() const;
    bool loadTexture(const std::string &name,
                     const std::string &filename,
                     int width,
                     int height);
    void unloadTexture(const std::string &textureName);
    void generateGLObject(const std::string &textureName,
                          std::optional<thewarrior::models::Size<int>> objectSize = std::nullopt,
                          HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center,
                          VerticalAlignment verticalAlignment = VerticalAlignment::Center,
                          thewarrior::models::Point<int> offset = thewarrior::models::Point<int>(0, 0));
    void drawGLObject(const std::string &textureName);
    void generateQuad(std::vector<GLObject> &objects,
                      thewarrior::models::Point<float> location,
                      thewarrior::models::Size<float> size,
                      const thewarrior::models::Texture *texture,
                      int textureId,
                      GLuint textureGLId = 0);
    void generateBoxQuad(std::vector<GLObject> &objects,
                         thewarrior::models::Point<float> location,
                         thewarrior::models::Size<float> size,
                         const thewarrior::models::Texture *texture,
                         int textureBeginId,
                         GLuint textureGLId = 0,
                         float blockSize = 32.0F);
    virtual void onInitialize(const GLComponentBaseInfo &) {}
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
