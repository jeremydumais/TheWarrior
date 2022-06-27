#pragma once

#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "IShaderService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <memory>
#include <string>
#include <vector>

class GLPopupWindow : public IShaderService
{
public:
    explicit GLPopupWindow(Size<float> size);
    virtual ~GLPopupWindow() = default;
    const std::string &getLastError() const;
    const Point<float> &getWindowLocation() const;
    const Size<float> &getWindowSize() const;
    Point<float> getWindowCenter() const;
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initialize(const std::string &title,
                    const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService);
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const Size<> &size);
protected:
    std::string m_lastError;
    Point<float> m_windowLocation;
    Size<float> m_windowSize;
    Point<float> m_windowCenter;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService; 
    GLTexture m_windowGLTexture;
    GLObject m_glwindow;
    GLTextObject m_glTitle;
    bool m_displayTitle;
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowTitleObjects;
    void generateQuad(GLObject &object, 
                      Point<float> location, 
                      Size<float> size, 
                      const Texture *texture, 
                      int textureId,
                      GLuint textureGLId = 0);
    void generateBoxQuad(std::vector<GLObject>::iterator begin, 
                         Point<float> location, 
                         Size<float> size,
                         const Texture *texture,
                         int textureBeginId,
                         GLuint textureGLId = 0);
    void addTextObject(GLTextObject textObject);
    void addXCenteredTextObject(GLTextObject textObject, float x, float width);
    std::vector<GLObject> m_glObjects;
    std::vector<GLTextObject> m_glTextObjects;
};