#pragma once

#include <map>
#include <memory>
#include <string>
#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include "texture.hpp"

namespace thewarrior::ui::screens {

class MenuScreenBase {
 public:
     MenuScreenBase(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                    std::map<std::string, unsigned int> &texturesGL);
     virtual ~MenuScreenBase() = default;
     void initializeBase(const std::string &resourcesPath,
            std::shared_ptr<GLShaderProgram> shaderProgram,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<InputDevicesState> inputDevicesState,
            std::shared_ptr<GLTexture> windowGLTexture);
    const std::string &getLastError() const;
    void updateBase();
    void renderBase();
    virtual void buttonUpPressed() = 0;
    virtual void buttonDownPressed() = 0;
    virtual void buttonActionPressed() = 0;

 protected:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    GLTextureService m_textureService;
    std::shared_ptr<GLShaderProgram> m_shaderProgram = nullptr;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &m_textures;
    std::map<std::string, unsigned int> &m_texturesGL;
    std::map<std::string, GLObject> m_namedObjects = {};
    std::shared_ptr<GLTexture> m_windowGLTexture;
    Uint64 m_lastMoveUpTicks = 0;
    Uint64 m_lastMoveDownTicks = 0;
    void generateGLElementsBase();
    void moveUpPressed();
    void moveDownPressed();
    void actionButtonPressed();
};

}  // namespace thewarrior::ui::screens
