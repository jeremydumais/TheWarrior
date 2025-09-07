#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>
#include <memory>
#include <string>
#include "glComponentBase.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLComponentBase::GLComponentBase(Point<float> location,
                                 Size<float> size)
: m_resourcesPath(""),
m_location(location),
m_initialLocation(location),
m_size(size),
m_screenSize(Size<float>(1.0F, 1.0F)),
m_shaderProgram(nullptr),
m_textService(nullptr),
m_glFormService(std::make_unique<GLFormService>()),
m_inputDevicesState(nullptr) {}

void GLComponentBase::initialize(const GLComponentBaseInfo &info) {
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
}

void GLComponentBase::generateGLElements() {
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
    onGameWindowSizeChanged(size);
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

void GLComponentBase::freeGLObjects(std::vector<GLObject> &objects) {
    for (auto &item : objects) {
        if (item.vboPosition) glDeleteBuffers(1, &item.vboPosition);
        if (item.vboColor) glDeleteBuffers(1, &item.vboColor);
        if (item.vboTexture) glDeleteBuffers(1, &item.vboTexture);
        if (item.vao) glDeleteVertexArrays(1, &item.vao);
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

}  // namespace thewarrior::ui::components
