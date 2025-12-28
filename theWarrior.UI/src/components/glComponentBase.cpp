#include <fmt/format.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "point.hpp"
#include "size.hpp"
#include "textureInfo.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLComponentBase::GLComponentBase(GLContext &glContext,
                                 Point<float> location,
                                 Size<float> size,
                                 HorizontalAlignment horizontalAlignment,
                                 VerticalAlignment verticalAlignement)
: m_registeredComponents({}),
m_initializationInfo(nullptr),
m_resourcesPath(""),
m_location(location),
m_initialLocation(location),
m_size(size),
m_screenSize(Size<float>(1.0F, 1.0F)),
m_shaderProgram(nullptr),
m_textService(nullptr),
m_glFormService(std::make_unique<GLFormService>()),
m_inputDevicesState(nullptr),
m_glContext(glContext),
m_horizontalAlignment(horizontalAlignment),
m_verticalAlignment(verticalAlignement) {}

GLComponentBase::~GLComponentBase() {
    freeGLObjects(m_namedObjects);
}

void GLComponentBase::initialize(const GLComponentBaseInfo &info) {
    m_initializationInfo = std::make_unique<GLComponentBaseInfo>(info);
    m_resourcesPath = info.resourcesPath;
    m_glTexture = info.texture;
    m_shaderProgram = info.shaderProgram;
    m_textService = info.textService;
    m_textureService = info.textureService;
    m_inputDevicesState = info.inputDevicesState;
    m_menuBackSound = info.menuBackSound;
    m_menuClickSound = info.menuClickSound;
    m_menuClickDisableSound = info.menuClickDisableSound;
    m_menuMoveSound = info.menuMoveSound;
    m_glFormService->initialize(info.shaderProgram, info.textService);
    for (auto *component : m_registeredComponents) {
        component->initialize(info);
    }
    onInitialize(info);
    generateGLElements();
}

void GLComponentBase::registerComponent(GLComponentBase *component) {
    m_registeredComponents.push_back(component);
}

const std::string &GLComponentBase::getLastError() const {
    return m_lastError;
}

HorizontalAlignment GLComponentBase::getHorizontalAlignment() const {
    return m_horizontalAlignment;
}

VerticalAlignment GLComponentBase::getVerticalAlignment() const {
    return m_verticalAlignment;
}

void GLComponentBase::generateGLElements() {
    freeGLObjects(m_namedObjects);
    for (auto *component : m_registeredComponents) {
        component->generateGLElements();
    }
    onGenerateGLElements();
}

void GLComponentBase::update() {
    const Uint64 MS_BETWEEN_SELECTION_CHANGE = 510;
    auto inputUpTicks = m_inputDevicesState->getUpPressedTicks();
    if (m_inputDevicesState->getUpPressed() &&
        inputUpTicks.has_value() &&
        (inputUpTicks.value() - m_lastMoveUpTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        onButtonUpPressed();
        m_lastMoveUpTicks = inputUpTicks.value();
        return;
    } else if (!m_inputDevicesState->getUpPressed()) {
        m_lastMoveUpTicks = 0;
    }
    auto inputDownTicks = m_inputDevicesState->getDownPressedTicks();
    if (m_inputDevicesState->getDownPressed() &&
        inputDownTicks.has_value() &&
        (inputDownTicks.value() - m_lastMoveDownTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        onButtonDownPressed();
        m_lastMoveDownTicks = inputDownTicks.value();
        return;
    } else if (!m_inputDevicesState->getDownPressed()) {
        m_lastMoveDownTicks = 0;
    }

    auto inputLeftTicks = m_inputDevicesState->getLeftPressedTicks();
    if (m_inputDevicesState->getLeftPressed() &&
        inputLeftTicks.has_value() &&
        (inputLeftTicks.value() - m_lastMoveLeftTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        onButtonLeftPressed();
        m_lastMoveLeftTicks = inputLeftTicks.value();
        return;
    } else if (!m_inputDevicesState->getLeftPressed()) {
        m_lastMoveLeftTicks = 0;
    }

    auto inputRightTicks = m_inputDevicesState->getRightPressedTicks();
    if (m_inputDevicesState->getRightPressed() &&
        inputRightTicks.has_value() &&
        (inputRightTicks.value() - m_lastMoveRightTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        onButtonRightPressed();
        m_lastMoveRightTicks = inputRightTicks.value();
        return;
    } else if (!m_inputDevicesState->getRightPressed()) {
        m_lastMoveRightTicks = 0;
    }

    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        onButtonActionPressed();
    }

    if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        onButtonCancelPressed();
    }
    generateGLElements();
}

void GLComponentBase::render() {
    onRender();
}

void GLComponentBase::gameWindowSizeChanged(const thewarrior::models::Size<int> &size) {
    m_screenSize = Size<float>(static_cast<float>(size.width()),
            static_cast<float>(size.height()));
    for (auto *component : m_registeredComponents) {
        component->gameWindowSizeChanged(size);
    }
    onGameWindowSizeChanged(size);
    generateGLElements();
}

Size<float> GLComponentBase::getSize() const {
    return m_size;
}

Point<float> GLComponentBase::getLocation() const {
    return m_initialLocation;
}

void GLComponentBase::setLocation(thewarrior::models::Point<float> value) {
    m_location = value;
    m_initialLocation = value;
    // Recalculate the real position
    onGameWindowSizeChanged(Size<int>{
            static_cast<int>(m_screenSize.width()),
            static_cast<int>(m_screenSize.height())
    });
}

void GLComponentBase::setSize(thewarrior::models::Size<float> value) {
    m_size = value;
}

void GLComponentBase::setHorizontalAlignment(HorizontalAlignment horizontalAlignment) {
    m_horizontalAlignment = horizontalAlignment;
}

void GLComponentBase::setVerticalAlignment(VerticalAlignment verticalAlignment) {
    m_verticalAlignment = verticalAlignment;
}


void GLComponentBase::freeGLObjects(std::vector<GLObject> &objects) {
    for (auto &item : objects) {
        if (item.vboPosition) glDeleteBuffers(1, &item.vboPosition);
        if (item.vboColor) glDeleteBuffers(1, &item.vboColor);
        if (item.vboTexture) glDeleteBuffers(1, &item.vboTexture);
        if (item.vao) glDeleteVertexArrays(1, &item.vao);
    }
    objects.clear();
}

void GLComponentBase::freeGLObjects(std::map<std::string, GLObject> &objects) {
    for (auto &item : objects) {
        if (item.second.vboPosition) glDeleteBuffers(1, &item.second.vboPosition);
        if (item.second.vboColor) glDeleteBuffers(1, &item.second.vboColor);
        if (item.second.vboTexture) glDeleteBuffers(1, &item.second.vboTexture);
        if (item.second.vao) glDeleteVertexArrays(1, &item.second.vao);
    }
    objects.clear();
}

void GLComponentBase::playBackSound() {
    Mix_PlayChannel(-1, m_menuBackSound.get(), 0);
}

void GLComponentBase::playClickSound() {
    Mix_PlayChannel(-1, m_menuClickSound.get(), 0);
}

void GLComponentBase::playClickDisableSound() {
    Mix_PlayChannel(-1, m_menuClickDisableSound.get(), 0);
}

void GLComponentBase::playMoveSound() {
    Mix_PlayChannel(-1, m_menuMoveSound.get(), 0);
}

Size<float> GLComponentBase::getGLSizeFromPx(Size<int> value) const {
    float pixelX = 1.0F / m_screenSize.width();
    float pixelY = 1.0F / m_screenSize.height();
    return Size<float>(static_cast<float>(value.width()) * pixelX,
                       static_cast<float>(value.height()) * pixelY);
}

GLComponentBaseInfo GLComponentBase::getComponentBaseInfo() const {
    GLComponentBaseInfo componentInfo {
        .resourcesPath = m_resourcesPath,
        .shaderProgram = m_shaderProgram,
        .textService = m_textService,
        .textureService = m_textureService,
        .inputDevicesState = m_inputDevicesState,
        .texture = m_glTexture,
        .menuMoveSound = m_menuMoveSound,
        .menuClickSound = m_menuClickSound,
        .menuClickDisableSound = m_menuClickDisableSound,
        .menuBackSound = m_menuBackSound
    };
    return componentInfo;
}

thewarrior::models::Point<float> GLComponentBase::getRelativeCenterPosition() const {
    float left = 0.0F;
    float top = 0.0F;
    //TODO: FIX m_location on each choice
    switch (m_horizontalAlignment) {
        case HorizontalAlignment::Left:
            left = (m_size.width() / 2.0F);
            break;
        case HorizontalAlignment::Center:
            left = (m_screenSize.width() / 2.0F) + m_location.x();
        break;
        case HorizontalAlignment::Right:
            left = m_screenSize.width() - m_size.width();
        break;
    }
    switch (m_verticalAlignment) {
        case VerticalAlignment::Top: break;
        case VerticalAlignment::Center:
            top = (m_screenSize.height() / 2.0F) + m_location.y();
        break;
        case VerticalAlignment::Bottom:
            top = m_screenSize.height() - m_size.height() + m_location.y();
        break;
    }
    return { left, top };
}

bool GLComponentBase::loadTexture(const std::string &name,
                                  const std::string &filename,
                                  int width,
                                  int height) {
    try {
        TextureInfo info {
            .name = name,
            .filename = filename,
            .width = width,
            .height = height,
            .tileWidth = width,
            .tileHeight = height
        };
        m_glContext.textures[info.name] = std::make_shared<Texture>(info);
        m_textureService->loadTexture(*m_glContext.textures[info.name], m_glContext.texturesGL[info.name]);
    } catch (const std::invalid_argument &err) {
        m_lastError = fmt::format("Unable to load the main menu logo texture: {0}", err.what());
        return false;
    }
    return true;
}

void GLComponentBase::unloadTexture(const std::string &textureName) {
    m_textureService->unloadTexture(m_glContext.texturesGL[textureName]);
}

void GLComponentBase::generateGLObject(const std::string &textureName,
                                       std::optional<Size<int>> objectSize,
                                       HorizontalAlignment horizontalAlignment,
                                       VerticalAlignment verticalAlignment,
                                       Point<int> offset) {
    std::vector<GLObject> menuObjects = {};
    auto screenGLSize = getGLSizeFromPx({
            static_cast<int>(m_screenSize.width()),
            static_cast<int>(m_screenSize.height()) });
    auto texture = m_glContext.textures[textureName];
    auto objectGLSize = getGLSizeFromPx(objectSize.has_value() ?
                        objectSize.value() :
                        Size<int>(texture->getWidth(), texture->getHeight()));
    // Determine the position
    float left = 0.0F;
    float top = 0.0F;
    switch (horizontalAlignment) {
        case HorizontalAlignment::Left: break;
        case HorizontalAlignment::Center:
            left = (screenGLSize.width() / 2.0F) - (objectGLSize.width() / 2.0F);
        break;
        case HorizontalAlignment::Right:
            left = screenGLSize.width() - objectGLSize.width();
        break;
    }
    switch (verticalAlignment) {
        case VerticalAlignment::Top: break;
        case VerticalAlignment::Center:
            top = (screenGLSize.height() / 2.0F) - (objectGLSize.height() / 2.0F);
        break;
        case VerticalAlignment::Bottom:
            top = screenGLSize.height() - objectGLSize.height();
        break;
    }
    auto glOffset = getGLSizeFromPx({offset.x(), offset.y()});
    m_glFormService->generateQuad(menuObjects,
                                  { left + glOffset.width(), top + glOffset.height() },
                                  { objectGLSize },
                                  texture.get(),
                                  0,
                                  m_glContext.texturesGL[textureName]);
    m_namedObjects[textureName] = menuObjects.at(0);
}

void GLComponentBase::drawGLObject(const std::string &textureName) {
    m_glFormService->drawQuad(m_namedObjects[textureName], m_glContext.texturesGL[textureName]);
}

void GLComponentBase::generateQuad(std::vector<GLObject> &objects,
        Point<float> location, Size<float> size,
        const Texture *texture, int textureId,
        GLuint textureGLId) {
    // Determine the position
    float left = 0.0F;
    float top = 0.0F;
    switch (m_horizontalAlignment) {
        case HorizontalAlignment::Left: break;
        case HorizontalAlignment::Center:
            left = (m_screenSize.width() / 2.0F) - (size.width() / 2.0F);
        break;
        case HorizontalAlignment::Right:
            left = m_screenSize.width() - size.width();
        break;
    }
    switch (m_verticalAlignment) {
        case VerticalAlignment::Top: break;
        case VerticalAlignment::Center:
            top = (m_screenSize.height() / 2.0F) - (size.height() / 2.0F);
        break;
        case VerticalAlignment::Bottom:
            top = m_screenSize.height() - size.height();
        break;
    }
    m_glFormService->generateQuad(
            objects, {left + location.x(), top + location.y()},
            size, texture, textureId, textureGLId);
}

void GLComponentBase::generateBoxQuad(std::vector<GLObject> &objects,
        Point<float> location, Size<float> size,
        const Texture *texture, int textureBeginId,
        GLuint textureGLId, float blockSize) {
    // Determine the position
    float left = 0.0F;
    float top = 0.0F;
    switch (m_horizontalAlignment) {
        case HorizontalAlignment::Left: break;
        case HorizontalAlignment::Center:
            left = (m_screenSize.width() / 2.0F) - (size.width() / 2.0F);
        break;
        case HorizontalAlignment::Right:
            left = m_screenSize.width() - size.width();
        break;
    }
    switch (m_verticalAlignment) {
        case VerticalAlignment::Top: break;
        case VerticalAlignment::Center:
            top = (m_screenSize.height() / 2.0F) - (size.height() / 2.0F);
        break;
        case VerticalAlignment::Bottom:
            top = m_screenSize.height() - size.height();
        break;
    }
    m_glFormService->generateBoxQuad(
            objects, { left + location.x(), top + location.y()},
            size, texture, textureBeginId, textureGLId, blockSize);
}


}  // namespace thewarrior::ui::components
