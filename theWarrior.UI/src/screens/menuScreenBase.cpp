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

void MenuScreenBase::generateGLElementsBase() {
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

Size<float> MenuScreenBase::getGLSizeFromPx(Size<int> value) const {
    float pixelX = 1.0F / m_screenSize.width();
    float pixelY = 1.0F / m_screenSize.height();
    return Size<float>(static_cast<float>(value.width()) * pixelX,
                       static_cast<float>(value.height()) * pixelY);
}

}  // namespace thewarrior::ui::screens
