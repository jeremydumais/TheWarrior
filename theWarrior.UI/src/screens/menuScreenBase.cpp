#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "menuScreenBase.hpp"
#include "size.hpp"
#include "texture.hpp"
#include "screens/mainMenuCommons.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

MenuScreenBase::MenuScreenBase(std::map<std::string, std::shared_ptr<Texture>> &textures,
                               std::map<std::string, unsigned int> &texturesGL)
: m_glFormService(std::make_shared<GLFormService>()),
m_screenSize(Size<float>(1.0F, 1.0F)),
m_textures(textures),
m_texturesGL(texturesGL) {
}

void MenuScreenBase::initializeBase(const MenuScreenBaseInfo &info) {
    m_resourcesPath = info.resourcesPath;
    m_shaderProgram = info.shaderProgram;
    m_glFormService->initialize(m_shaderProgram, info.textService);
    m_textureService.setResourcesPath(info.resourcesPath);
    m_inputDevicesState = info.inputDevicesState;
    m_windowGLTexture = info.windowGLTexture;
    m_menuMoveSound = info.menuMoveSound;
    m_menuClickSound = info.menuClickSound;
    m_menuClickDisableSound = info.menuClickDisableSound;
    m_menuBackSound = info.menuBackSound;
}

const std::string& MenuScreenBase::getLastError() const {
    return m_lastError;
}

void MenuScreenBase::updateBase() {
    const Uint64 MS_BETWEEN_SELECTION_CHANGE = 510;
    auto inputUpTicks = m_inputDevicesState->getUpPressedTicks();
    if (m_inputDevicesState->getUpPressed() &&
        inputUpTicks.has_value() &&
        (inputUpTicks.value() - m_lastMoveUpTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        buttonUpPressed();
        m_lastMoveUpTicks = inputUpTicks.value();
        return;
    } else if (!m_inputDevicesState->getUpPressed()) {
        m_lastMoveUpTicks = 0;
    }
    auto inputDownTicks = m_inputDevicesState->getDownPressedTicks();
    if (m_inputDevicesState->getDownPressed() &&
        inputDownTicks.has_value() &&
        (inputDownTicks.value() - m_lastMoveDownTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        buttonDownPressed();
        m_lastMoveDownTicks = inputDownTicks.value();
        return;
    } else if (!m_inputDevicesState->getDownPressed()) {
        m_lastMoveDownTicks = 0;
    }

    auto inputLeftTicks = m_inputDevicesState->getLeftPressedTicks();
    if (m_inputDevicesState->getLeftPressed() &&
        inputLeftTicks.has_value() &&
        (inputLeftTicks.value() - m_lastMoveLeftTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        buttonLeftPressed();
        m_lastMoveLeftTicks = inputLeftTicks.value();
        return;
    } else if (!m_inputDevicesState->getLeftPressed()) {
        m_lastMoveLeftTicks = 0;
    }

    auto inputRightTicks = m_inputDevicesState->getRightPressedTicks();
    if (m_inputDevicesState->getRightPressed() &&
        inputRightTicks.has_value() &&
        (inputRightTicks.value() - m_lastMoveRightTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        buttonRightPressed();
        m_lastMoveRightTicks = inputRightTicks.value();
        return;
    } else if (!m_inputDevicesState->getRightPressed()) {
        m_lastMoveRightTicks = 0;
    }

    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        buttonActionPressed();
    }

    if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        buttonCancelPressed();
    }
}

void MenuScreenBase::renderBase() {
    m_glFormService->drawQuad(m_namedObjects[TextureBackground], m_texturesGL[TextureBackground]);
}

void MenuScreenBase::gameWindowSizeChangedBase(const Size<> &size) {
    m_screenSize = Size<float>(static_cast<float>(size.width()),
                               static_cast<float>(size.height()));
}

void MenuScreenBase::freeGLObjects(std::vector<GLObject> &objects) {
    for (auto &item : objects) {
        if (item.vboPosition) glDeleteBuffers(1, &item.vboPosition);
        if (item.vboColor) glDeleteBuffers(1, &item.vboColor);
        if (item.vboTexture) glDeleteBuffers(1, &item.vboTexture);
        if (item.vao) glDeleteVertexArrays(1, &item.vao);
    }
    objects.clear();
}

void MenuScreenBase::freeGLObjects(std::map<std::string, GLObject> &objects) {
    for (auto &item : objects) {
        if (item.second.vboPosition) glDeleteBuffers(1, &item.second.vboPosition);
        if (item.second.vboColor) glDeleteBuffers(1, &item.second.vboColor);
        if (item.second.vboTexture) glDeleteBuffers(1, &item.second.vboTexture);
        if (item.second.vao) glDeleteVertexArrays(1, &item.second.vao);
    }
    objects.clear();
}

void MenuScreenBase::generateGLElementsBase() {
    freeGLObjects(m_namedObjects);
    std::vector<GLObject> menuObjects = {};
    m_glFormService->generateQuad(menuObjects,
                                  { 0.0F, 0.0F },
                                  { 1.0F, 1.0F}, m_textures[TextureBackground].get(), 25, m_texturesGL[TextureBackground]);
    m_namedObjects[TextureBackground] = menuObjects.at(0);
}

void MenuScreenBase::playMoveSound() {
    Mix_PlayChannel(-1, m_menuMoveSound.get(), 0);
}

void MenuScreenBase::playClickSound() {
    Mix_PlayChannel(-1, m_menuClickSound.get(), 0);
}

void MenuScreenBase::playClickDisableSound() {
    Mix_PlayChannel(-1, m_menuClickDisableSound.get(), 0);
}

void MenuScreenBase::playBackSound() {
    Mix_PlayChannel(-1, m_menuBackSound.get(), 0);
}

Size<float> MenuScreenBase::getGLSizeFromPx(Size<int> value) const {
    float pixelX = 1.0F / m_screenSize.width();
    float pixelY = 1.0F / m_screenSize.height();
    return Size<float>(static_cast<float>(value.width()) * pixelX,
                       static_cast<float>(value.height()) * pixelY);
}

void MenuScreenBase::buttonLeftPressed() {
}

void MenuScreenBase::buttonRightPressed() {
}

}  // namespace thewarrior::ui::screens
