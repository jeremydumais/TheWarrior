#include <fmt/format.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "glPopupWindow.hpp"
#include "glObjectService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include <boost/algorithm/string.hpp>

using namespace thewarrior::models;

namespace thewarrior::ui {

GLPopupWindow::GLPopupWindow(Size<float> size)
: m_lastError(""),
m_windowLocation({ 1.0F, 1.0F }),
m_windowSize(size),
m_windowCenter({ 1.0F, 1.0F }),
m_screenSize({ 1.0F, 1.0F }),
m_shaderProgram(nullptr),
m_glFormService(std::make_shared<GLFormService>()),
m_textService(nullptr),
m_windowGLTexture({ Texture(TextureInfo { "window", "window.png", 256, 256, 32, 32 }), 0 }),
m_texture(nullptr),
m_glTitle({ "", { 1.0F, 1.0F }, 0.6F }),
m_displayTitle(false),
m_windowObjects(std::vector<GLObject>()),
m_windowBackgrounds(std::vector<GLObject>()),
m_windowTitleObjects(std::vector<GLObject>()),
m_textureBeginId(17),
m_fillCenter(false),
m_glObjects(std::vector<GLObject>()),
m_glTextObjects(std::vector<GLTextObject>()) {
}

GLPopupWindow::~GLPopupWindow() {
    freeGLObjects(m_windowObjects);
    freeGLObjects(m_windowBackgrounds);
    freeGLObjects(m_glObjects);
    freeGLObjects(m_windowTitleObjects);
}

const std::string& GLPopupWindow::getLastError() const {
    return m_lastError;
}

const Point<float> &GLPopupWindow::getWindowLocation() const {
    return m_windowLocation;
}

const Size<float>& GLPopupWindow::getWindowSize() const {
    return m_windowSize;
}

Point<float> GLPopupWindow::getWindowCenter() const {
    return m_windowCenter;
}

bool GLPopupWindow::initShader(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName) {
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

void GLPopupWindow::initShader(const std::shared_ptr<GLShaderProgram> shaderProgram) {
    m_shaderProgram = shaderProgram;
}

void GLPopupWindow::initialize(const std::string &title,
                               const std::string &resourcePath,
                               std::shared_ptr<GLTextService> textService) {
    m_glTitle.text = title;
    m_textService = textService;
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_windowGLTexture);
    m_glFormService->initialize(m_shaderProgram, textService);
}

void GLPopupWindow::initialize(const std::string &title,
                               const std::shared_ptr<GLTexture> texture,
                               std::shared_ptr<GLTextService> textService) {
    m_glTitle.text = title;
    m_textService = textService;
    m_texture = texture;
    m_glFormService->initialize(m_shaderProgram, textService);
}

void GLPopupWindow::setTitle(const std::string &title) {
    m_glTitle.text = title;
    generateTitleBox();
}

void GLPopupWindow::generateGLElements() {
    freeGLObjects(m_windowObjects);
    freeGLObjects(m_windowBackgrounds);
    freeGLObjects(m_glObjects);
    m_glTextObjects.clear();
    // Window
    m_glFormService->generateQuad(m_windowBackgrounds, getWindowLocation(), getWindowSize(),
            m_fillCenter ? &getTexturePtr()->texture : nullptr,
            m_fillCenter ? m_textureBeginId + 8 : 0);
    m_glFormService->generateBoxQuad(m_windowObjects,
                                    getWindowLocation(),
                                    getWindowSize(),
                                    &getTexturePtr()->texture,
                                    m_textureBeginId);
    generateTitleBox();
}

void GLPopupWindow::render() {
    for (const auto &obj : m_windowBackgrounds) {
        m_glFormService->drawQuad(obj,
                m_fillCenter ? getTexturePtr()->glTextureId : 0,
                m_fillCenter ? 1.0F : 0.9F);
    }
    for (const auto &obj : m_windowObjects) {
        m_glFormService->drawQuad(obj, getTexturePtr()->glTextureId);
    }
    if (m_displayTitle) {
        for (const auto &obj : m_windowTitleObjects) {
            m_glFormService->drawQuad(obj, getTexturePtr()->glTextureId);
        }
        m_glFormService->drawText(m_glTitle);
    }
    for (const auto &obj : m_glObjects) {
        m_glFormService->drawQuad(obj, obj.textureGLId);
    }
    for (const auto &textObj : m_glTextObjects) {
        m_glFormService->drawText(textObj);
    }
}

void GLPopupWindow::gameWindowSizeChanged(const Size<> &size) {
    m_screenSize = Size<float>(static_cast<float>(size.width()), static_cast<float>(size.height()));
    m_windowLocation = { (static_cast<float>(size.width()) / 2.0F) - (m_windowSize.width() / 2.0F),
                         (static_cast<float>(size.height()) / 2.0F) - (m_windowSize.height() / 2.0F) };
    m_windowCenter = { m_windowLocation.x() + (m_windowSize.width() / 2.0F),
                       m_windowLocation.y() + (m_windowSize.height() / 2.0F) };
    m_glFormService->gameWindowSizeChanged(size);
}

void GLPopupWindow::setTextureBeginId(int value) {
    m_textureBeginId = value;
}

void GLPopupWindow::setFillCenter(bool value) {
    m_fillCenter = value;
}

void GLPopupWindow::setWindowSize(thewarrior::models::Size<float> size) {
    m_windowSize = size;
    gameWindowSizeChanged(Size<int>(static_cast<int>(m_screenSize.width()),
                                    static_cast<int>(m_screenSize.height())));
}

void GLPopupWindow::freeGLObjects(std::vector<GLObject> &objects) {
    for (auto &item : objects) {
        if (item.vboPosition) glDeleteBuffers(1, &item.vboPosition);
        if (item.vboColor) glDeleteBuffers(1, &item.vboColor);
        if (item.vboTexture) glDeleteBuffers(1, &item.vboTexture);
        if (item.vao) glDeleteVertexArrays(1, &item.vao);
    }
    objects.clear();
}

void GLPopupWindow::freeGLObjects(std::map<std::string, GLObject> &objects) {
    for (auto &item : objects) {
        if (item.second.vboPosition) glDeleteBuffers(1, &item.second.vboPosition);
        if (item.second.vboColor) glDeleteBuffers(1, &item.second.vboColor);
        if (item.second.vboTexture) glDeleteBuffers(1, &item.second.vboTexture);
        if (item.second.vao) glDeleteVertexArrays(1, &item.second.vao);
    }
    objects.clear();
}

void GLPopupWindow::generateQuad(std::vector<GLObject> &objects, Point<float> location, Size<float> size, const Texture *texture, int textureId, GLuint textureGLId) {
    m_glFormService->generateQuad(objects,
                                  { m_windowLocation.x() + location.x(),
                                    m_windowLocation.y() + location.y() },
                                  size, texture, textureId, textureGLId);
}

void GLPopupWindow::generateBoxQuad(std::vector<GLObject> &objects,
                     Point<float> location,
                     Size<float> size,
                     const Texture *texture,
                     int textureBeginId,
                     GLuint textureGLId,
                     float blockSize) {
    m_glFormService->generateBoxQuad(objects,
                                     { m_windowLocation.x() + location.x(),
                                       m_windowLocation.y() + location.y() },
                                     size, texture, textureBeginId, textureGLId, blockSize);
}

void GLPopupWindow::generateTitleBox() {
    freeGLObjects(m_windowTitleObjects);
    m_displayTitle = !m_glTitle.text.empty();
    auto titleSize = m_textService->getTextSize(m_glTitle.text, 0.6F);
    m_glTitle.position = { getWindowLocation().x() + 15.0F + (getWindowSize().width() / 2.0F) - (titleSize.width() / 2.0F),
                           getWindowLocation().y() + 40.0F };
    m_glFormService->generateBoxQuad(m_windowTitleObjects,
                                    {m_glTitle.position.x() - 35.0F, m_glTitle.position.y() - 30.0F},
                                    {titleSize.width() + 70.0F, 40.0F},
                                    &getTexturePtr()->texture,
                                    17);
}

void GLPopupWindow::addWindowPanel(Point<float> location,
                                   Size<float> size,
                                   int textureBeginId) {
    generateBoxQuad(m_windowObjects,
                    location,
                    size,
                    &getTexturePtr()->texture,
                    textureBeginId,
                    getTexturePtr()->glTextureId);
}

void GLPopupWindow::addTextObject(GLTextObject textObject) {
    textObject.position.setX(m_windowLocation.x() + textObject.position.x());
    textObject.position.setY(m_windowLocation.y() + textObject.position.y());
    m_glTextObjects.push_back(textObject);
}

void GLPopupWindow::addXCenteredTextObject(GLTextObject textObject, float x, float width) {
    std::vector<std::string> lines;
    boost::split(lines, textObject.text, boost::is_any_of("\n"));
    float initialYPosition = textObject.position.y();
    int lineIndex = 0;
    for (const auto &line : lines) {
        textObject.text = line;
        auto textSize = m_textService->getTextSize(textObject.text, textObject.scale);
        textObject.position.setX(x + (width / 2.0F) - (textSize.width() / 2.0F));
        textObject.position.setY(initialYPosition + (static_cast<float>(lineIndex) * 20.0F));
        addTextObject(textObject);
        lineIndex++;
    }
}

void GLPopupWindow::addXCenteredTwoColumnsLabels(const std::string &label,
                                               const std::string &value,
                                               float yPosition,
                                               float scale,
                                               float x,
                                               float width,
                                               GLColor colorLabel,
                                               GLColor colorValue) {
    auto labelStrSize = m_textService->getTextSize(label, scale);
    auto labelAndValueSize = m_textService->getTextSize(fmt::format("{0}{1}", label, value), scale);

    addTextObject({label, {(x + (width / 2.0F)) - (labelAndValueSize.width() / 2.0F), yPosition}, scale, colorLabel});
    addTextObject({value, {(x + (width / 2.0F)) - (labelAndValueSize.width() / 2.0F) + labelStrSize.width(), yPosition}, scale, colorValue});
}

GLTexture *GLPopupWindow::getTexturePtr() {
    if (m_texture) {
        return m_texture.get();
    }
    return &m_windowGLTexture;
}

}  // namespace thewarrior::ui
