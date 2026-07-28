#include <algorithm>
#include <memory>
#include <string>
#include "glChoicePopup.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui {

constexpr float ChoicesTopMargin = 30.0F;
constexpr float MinimumPopupWidth = 300.0F;
constexpr float ChoiceHorizontalPadding = 100.0F;
constexpr float TitleHorizontalPadding = 70.0F;
constexpr float PromptHorizontalPadding = 70.0F;
constexpr float PromptVerticalSpace = 60.0F;

GLChoicePopup::GLChoicePopup()
: GLPopupWindow({ 300.0F, 600.0F }),
m_menuCursorPosition(0),
m_menuItemCount(0),
m_glPrompt({ "", { 1.0F, 1.0F }, 0.6F }),
m_popupGLTexture({ Texture(TextureInfo { "inventoryWindow", "window.png", 256, 256, 32, 32 }), 0 }),
m_menuObjects(std::vector<GLObject>()),
m_glTextChoices(std::vector<GLTextObject>()) {
}

void GLChoicePopup::initialize(const std::string &resourcePath,
                               std::shared_ptr<GLTextService> textService,
                               std::shared_ptr<InputDevicesState> inputDevicesState) {
    GLPopupWindow::initialize("", resourcePath, textService);
    m_textService = textService;
    m_inputDevicesState = inputDevicesState;
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_popupGLTexture);
}

void GLChoicePopup::preparePopup(std::vector<std::string> choices,
                                 const std::string &title,
                                 const std::string &prompt) {
    setTitle(title);
    setPrompt(prompt);
    m_glTextChoices.clear();
    for (const auto &choice : choices) {
        m_glTextChoices.push_back({ choice, { 1.0F, 1.0F }, 0.6F });
    }
    m_menuItemCount = choices.size();
    m_menuCursorPosition = 0;

    float popupWidth = MinimumPopupWidth;
    for (const auto &choice : m_glTextChoices) {
        const auto choiceSize = m_textService->getTextSize(choice.text, choice.scale);
        popupWidth = std::max(popupWidth, choiceSize.width() + ChoiceHorizontalPadding);
    }
    if (isTitleDisplayed()) {
        const auto titleSize = m_textService->getTextSize(title, m_glTitle.scale);
        popupWidth = std::max(popupWidth, titleSize.width() + TitleHorizontalPadding);
    }
    if (!m_prompt.empty()) {
        const auto promptSize = m_textService->getTextSize(m_prompt, m_glPrompt.scale);
        popupWidth = std::max(popupWidth, promptSize.width() + PromptHorizontalPadding);
    }

    float topMargin = isTitleDisplayed() ? ChoicesTopMargin : 0.0F;
    const float promptSpace = m_prompt.empty() ? 0.0F : PromptVerticalSpace;
    m_windowSize.setWidth(popupWidth);
    m_windowSize.setHeight((70.0F * static_cast<float>(m_glTextChoices.size())) +
                           50.0F +
                           topMargin +
                           promptSpace);
    // Since the height of the windows is dynamic, we need to recalculate the windows location on screen.
    GLPopupWindow::gameWindowSizeChanged({static_cast<int>(m_screenSize.width()), static_cast<int>(m_screenSize.height())});
}

void GLChoicePopup::update() {
    const Uint64 MS_BETWEEN_SELECTION_CHANGE = 110;
    auto inputUpTicks = m_inputDevicesState->getUpPressedTicks();
    if (m_inputDevicesState->getUpPressed() &&
        inputUpTicks.has_value() &&
        (inputUpTicks.value() - lastMoveUpTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        moveUpPressed();
        lastMoveUpTicks = inputUpTicks.value();
        return;
    } else if (!m_inputDevicesState->getUpPressed()) {
        lastMoveUpTicks = 0;
    }

    auto inputDownTicks = m_inputDevicesState->getDownPressedTicks();
    if (m_inputDevicesState->getDownPressed() &&
        inputDownTicks.has_value() &&
        (inputDownTicks.value() - lastMoveDownTicks) > MS_BETWEEN_SELECTION_CHANGE) {
        moveDownPressed();
        lastMoveDownTicks = inputDownTicks.value();
        return;
    } else if (!m_inputDevicesState->getDownPressed()) {
        lastMoveDownTicks = 0;
    }

    if (m_inputDevicesState->getButtonAState() == InputElementState::Released) {
        actionButtonPressed();
    } else if (m_inputDevicesState->getButtonBState() == InputElementState::Released) {
        m_cancelClicked();
    }
}

void GLChoicePopup::render() {
    GLPopupWindow::render();
    if (!m_prompt.empty()) {
        m_glFormService->drawText(m_glPrompt);
    }
    for (size_t i = 0; i < m_glTextChoices.size(); i++) {
        m_glFormService->drawText(m_glTextChoices[i], i == m_menuCursorPosition ? GLColor::White : GLColor::Gray);
    }
}

void GLChoicePopup::generateGLElements() {
    GLPopupWindow::generateGLElements();
    m_menuObjects.clear();
    const float topMargin = isTitleDisplayed() ? ChoicesTopMargin : 0.0F;
    float choicesTop = m_windowLocation.y() + 70.0F + topMargin;
    if (!m_prompt.empty()) {
        const auto promptSize = m_textService->getTextSize(m_prompt, m_glPrompt.scale);
        m_glPrompt.position = {
            m_windowLocation.x() + (m_windowSize.width() / 2.0F) - (promptSize.width() / 2.0F),
            choicesTop
        };
        choicesTop += PromptVerticalSpace;
    }
    for (size_t i = 0; i < m_glTextChoices.size(); i++) {
        auto textSize = m_textService->getTextSize(m_glTextChoices[i].text, 0.6F);
        m_glTextChoices[i].position = { m_windowLocation.x() + (m_windowSize.width() / 2.0F) - (textSize.width() / 2.0F),
                                        choicesTop + (70.0F * static_cast<float>(i)) };
    }
}

void GLChoicePopup::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    generateGLElements();
}

void GLChoicePopup::setPrompt(const std::string &prompt) {
    m_prompt = prompt;
    m_glPrompt.text = prompt;
}

void GLChoicePopup::moveUpPressed() {
    if (m_menuCursorPosition > 0) {
        m_menuCursorPosition--;
        generateGLElements();
    }
}

void GLChoicePopup::moveDownPressed() {
    if (m_menuCursorPosition + 1 < m_menuItemCount) {
        m_menuCursorPosition++;
        generateGLElements();
    }
}

void GLChoicePopup::actionButtonPressed() {
    m_choiceClicked(m_menuCursorPosition);
}

}  // namespace thewarrior::ui
