#include "glChoicePopup.hpp"

namespace thewarrior::ui {

GLChoicePopup::GLChoicePopup()
    : m_glFormService(nullptr),
      m_windowCenter(1.0F, 1.0F),
      m_menuCursorPosition(0),
      m_menuItemCount(0),
      m_popupGLTexture({ Texture(TextureInfo { "inventoryWindow", "window.png", 256, 256, 32, 32 }), 0 }),
      m_menuObjects(std::vector<GLObject>()),
      m_glTextChoices(std::vector<GLTextObject>())
{
}

void GLChoicePopup::initialize(const std::string &resourcePath,
                               std::shared_ptr<GLFormService> glFormService,
                               std::shared_ptr<GLTextService> textService,
                               std::shared_ptr<InputDevicesState> inputDevicesState)
{
    m_glFormService = glFormService;
    m_textService = textService;
    m_inputDevicesState = inputDevicesState;
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_popupGLTexture);
}

void GLChoicePopup::preparePopup(std::vector<std::string> choices)
{
    m_glTextChoices.clear();
    for(const auto &choice : choices) {
        m_glTextChoices.push_back({ choice, { 1.0F, 1.0F }, 0.6F });
    }
    m_menuItemCount = choices.size();
    m_menuCursorPosition = 0;
}

void GLChoicePopup::update()
{
    const Uint64 MS_BETWEEN_SELECTION_CHANGE = 110;
    auto inputUpTicks = m_inputDevicesState->getUpPressedTicks();
    if (m_inputDevicesState->getUpPressed() &&
        inputUpTicks.has_value() &&
        (inputUpTicks.value() - lastMoveUpTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        moveUpPressed();
        lastMoveUpTicks = inputUpTicks.value();
        return;
    }
    else if (!m_inputDevicesState->getUpPressed()) {
        lastMoveUpTicks = 0;
    }

    auto inputDownTicks = m_inputDevicesState->getDownPressedTicks();
    if (m_inputDevicesState->getDownPressed() &&
        inputDownTicks.has_value() &&
        (inputDownTicks.value() - lastMoveDownTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        moveDownPressed();
        lastMoveDownTicks = inputDownTicks.value();
        return;
    }
    else if (!m_inputDevicesState->getDownPressed()) {
        lastMoveDownTicks = 0;
    }

    if(m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        actionButtonPressed();
    }
    else if(m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        m_cancelClicked();
    }
}

void GLChoicePopup::render()
{
    for(const auto &obj : m_menuObjects) {
        m_glFormService->drawQuad(obj, m_popupGLTexture.glTextureId);
    }
    for(size_t i = 0; i < m_glTextChoices.size(); i++) {
        m_glFormService->drawText(m_glTextChoices[i], i == m_menuCursorPosition ? GLColor::White : GLColor::Gray);
    }
}

void GLChoicePopup::generateGLElements()
{

    const Size<float> POPUP_SIZE(300.0F, (70.0F * static_cast<float>(m_glTextChoices.size())) + 50.0F);
    const Point<float> POPUP_LOCATION(m_windowCenter.x() - (POPUP_SIZE.width() / 2.0F),
                                      m_windowCenter.y() - (POPUP_SIZE.height() / 2.0F));
    const float BLOCKSIZE = 32.0F;
    m_menuObjects.push_back(GLObject {});
    m_glFormService->generateQuad(m_menuObjects,
                                { POPUP_LOCATION.x() + BLOCKSIZE, POPUP_LOCATION.y() + BLOCKSIZE },
                                { POPUP_SIZE.width() - (BLOCKSIZE * 2.0F), POPUP_SIZE.height() - (BLOCKSIZE * 2.0F) },
                                &m_popupGLTexture.texture,
                                16);
    m_glFormService->generateBoxQuad(m_menuObjects,
                                    { POPUP_LOCATION.x(), POPUP_LOCATION.y() },
                                    { POPUP_SIZE.width(), POPUP_SIZE.height() },
                                    &m_popupGLTexture.texture,
                                    8);
    for(size_t i = 0; i < m_glTextChoices.size(); i++) {
        auto textSize = m_textService->getTextSize(m_glTextChoices[i].text, 0.6F);
        m_glTextChoices[i].position = { POPUP_LOCATION.x() + (POPUP_SIZE.width() / 2.0F) - (textSize.width() / 2.0F),
                                        POPUP_LOCATION.y() + 70.0F + (70.0F * static_cast<float>(i)) };
    }
}

void GLChoicePopup::gameWindowLocationChanged(const Point<float> &windowCenter)
{
    m_windowCenter = windowCenter;
}

void GLChoicePopup::moveUpPressed()
{
    if (m_menuCursorPosition > 0) {
        m_menuCursorPosition--;
        generateGLElements();
    }
}

void GLChoicePopup::moveDownPressed()
{
    if (m_menuCursorPosition + 1 < m_menuItemCount) {
        m_menuCursorPosition++;
        generateGLElements();
    }
}

void GLChoicePopup::actionButtonPressed()
{
    m_choiceClicked(m_menuCursorPosition);
}

} // namespace thewarrior::ui
