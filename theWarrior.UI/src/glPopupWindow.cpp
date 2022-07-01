#include "glPopupWindow.hpp"
#include <boost/algorithm/string.hpp>
#include <vector>

GLPopupWindow::GLPopupWindow(Size<float> size)
    : m_lastError(""),
      m_windowLocation({ 1.0F, 1.0F }),
      m_windowSize(size),
      m_windowCenter({ 1.0F, 1.0F }),
      m_shaderProgram(nullptr),
      m_glFormService(std::make_shared<GLFormService>()),
      m_textService(nullptr),
      m_windowGLTexture({ Texture(TextureInfo { "window", "window.png", 256, 256, 32, 32 }), 0 }),
      m_glTitle({ "", { 1.0F, 1.0F }, 0.6F }),
      m_displayTitle(false),
      m_windowObjects(std::vector<GLObject>()),
      m_windowTitleObjects(std::vector<GLObject>()),
      m_glObjects(std::vector<GLObject>()),
      m_glTextObjects(std::vector<GLTextObject>())
{
}

const std::string& GLPopupWindow::getLastError() const
{
    return m_lastError;
}

const Point<float> &GLPopupWindow::getWindowLocation() const
{
    return m_windowLocation;
}

const Size<float>& GLPopupWindow::getWindowSize() const
{
    return m_windowSize;
}

Point<float> GLPopupWindow::getWindowCenter() const
{
    return m_windowCenter;
}

bool GLPopupWindow::initShader(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName)
{
    m_shaderProgram = std::make_shared<GLShaderProgram>(vertexShaderFileName,
                                                        fragmentShaderFileName);
    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    if (!m_shaderProgram->linkShaders({ "vertex" })) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }
    return true;
}
void GLPopupWindow::initialize(const std::string &title,
                               const std::string &resourcePath,
                               std::shared_ptr<GLTextService> textService)
{
    m_glTitle.text = title;
    m_textService = textService;
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_windowGLTexture);
    m_glFormService->initialize(m_shaderProgram, textService);
}

void GLPopupWindow::generateGLElements()
{
    m_windowObjects.clear();
    m_windowTitleObjects.clear();
    m_glObjects.clear();
    m_glTextObjects.clear();
    //Window
    m_glFormService->generateQuad(m_glwindow, getWindowLocation(), getWindowSize());
    m_glFormService->generateBoxQuad(m_windowObjects,
                                    getWindowLocation(),
                                    getWindowSize(),
                                    &m_windowGLTexture.texture,
                                    17);
    //TitleBox
    m_displayTitle = !m_glTitle.text.empty();
    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position = { getWindowLocation().x() + 15.0F + (getWindowSize().width() / 2.0F) - (titleSize.width() / 2.0F),
                           getWindowLocation().y() + 40.0F };
    m_glFormService->generateBoxQuad(m_windowTitleObjects,
                                    {m_glTitle.position.x() - 35.0F, m_glTitle.position.y() - 30.0F},
                                    {titleSize.width() + 70.0F, 40.0F},
                                    &m_windowGLTexture.texture,
                                    17);
}

void GLPopupWindow::render()
{
    m_glFormService->drawQuad(m_glwindow, 0, 0.8F);
    for(const auto &obj : m_windowObjects) {
        m_glFormService->drawQuad(obj, m_windowGLTexture.glTextureId);
    }
    if (m_displayTitle) {
        for(const auto &obj : m_windowTitleObjects) {
            m_glFormService->drawQuad(obj, m_windowGLTexture.glTextureId);
        }
        m_glFormService->drawText(m_glTitle);
    }
    for(const auto &obj : m_glObjects) {
        m_glFormService->drawQuad(obj, obj.textureGLId);
    }
    for(const auto &textObj : m_glTextObjects) {
        m_glFormService->drawText(textObj);
    }
}

void GLPopupWindow::gameWindowSizeChanged(const Size<> &size)
{
    m_windowLocation = { (static_cast<float>(size.width()) / 2.0F) - (m_windowSize.width() / 2.0F), 
                         (static_cast<float>(size.height()) / 2.0F) - (m_windowSize.height() / 2.0F) };
    m_windowCenter = { m_windowLocation.x() + (m_windowSize.width() / 2.0F),
                       m_windowLocation.y() + (m_windowSize.height() / 2.0F) };
    m_glFormService->gameWindowSizeChanged(size);
}

void GLPopupWindow::generateQuad(GLObject &object, Point<float> location, Size<float> size, const Texture *texture, int textureId, GLuint textureGLId)
{
    object.textureGLId = textureGLId;
    m_glFormService->generateQuad(object, 
                                  { m_windowLocation.x() + location.x(), 
                                    m_windowLocation.y() + location.y() },
                                  size, texture, textureId);
}

void GLPopupWindow::generateBoxQuad(std::vector<GLObject> &objects, 
                     Point<float> location, 
                     Size<float> size,
                     const Texture *texture,
                     int textureBeginId,
                     GLuint textureGLId)
{
    m_glFormService->generateBoxQuad(objects,
                                     { m_windowLocation.x() + location.x(), 
                                       m_windowLocation.y() + location.y() },
                                     size, texture, textureBeginId, textureGLId);
}

void GLPopupWindow::addTextObject(GLTextObject textObject)
{
    textObject.position.setX(m_windowLocation.x() + textObject.position.x());
    textObject.position.setY(m_windowLocation.y() + textObject.position.y());
    m_glTextObjects.push_back(textObject);
}

void GLPopupWindow::addXCenteredTextObject(GLTextObject textObject, float x, float width)
{
    std::vector<std::string> lines;
    boost::split(lines, textObject.text, boost::is_any_of("\n"));
    float initialYPosition = textObject.position.y();
    int lineIndex = 0;
    for(const auto &line : lines) {
        textObject.text = line;
        auto textSize = m_textService->getTextSize(textObject.text, textObject.scale);
        textObject.position.setX(x + (width / 2.0F) - (textSize.width() / 2.0F));
        textObject.position.setY(initialYPosition + (static_cast<float>(lineIndex) * 20.0F));
        addTextObject(textObject);
        lineIndex++;
    }
}
