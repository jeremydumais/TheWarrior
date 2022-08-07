#pragma once

#include "glColor.hpp"
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
#include <boost/signals2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace thewarrior::ui {

class GLPopupWindow : public IShaderService
{
public:
    explicit GLPopupWindow(thewarrior::models::Size<float> size);
    virtual ~GLPopupWindow() = default;
    const std::string &getLastError() const;
    const thewarrior::models::Point<float> &getWindowLocation() const;
    const thewarrior::models::Size<float> &getWindowSize() const;
    thewarrior::models::Point<float> getWindowCenter() const;
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initShader(const std::shared_ptr<GLShaderProgram> shaderProgram) override;
    void initialize(const std::string &title,
                    const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService);
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
    boost::signals2::signal<void()> onCloseEvent;
protected:
    std::string m_lastError;
    thewarrior::models::Point<float> m_windowLocation;
    thewarrior::models::Size<float> m_windowSize;
    thewarrior::models::Point<float> m_windowCenter;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;
    GLTexture m_windowGLTexture;
    GLObject m_glwindow;
    GLTextObject m_glTitle;
    bool m_displayTitle;
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowBackgrounds;
    std::vector<GLObject> m_windowTitleObjects;
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
                         GLuint textureGLId = 0);
    void addWindowPanel(thewarrior::models::Point<float> location,
                        thewarrior::models::Size<float> size,
                        int textureBeginId);
    void addTextObject(GLTextObject textObject);
    void addXCenteredTextObject(GLTextObject textObject, float x, float width);
    void addXCenteredTwoColumnsLabels(const std::string &label,
                                      const std::string &value,
                                      float yPosition,
                                      float scale,
                                      float x,
                                      float width,
                                      GLColor colorLabel = GLColor::White,
                                      GLColor colorValue = GLColor::White);
    std::vector<GLObject> m_glObjects;
    std::vector<GLTextObject> m_glTextObjects;
};

} // namespace thewarrior::ui
