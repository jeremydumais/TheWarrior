#include <fmt/format.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "glTextBox.hpp"
#include "itemFoundMessageDTO.hpp"
#include "npcDialogueMessageDTO.hpp"
#include "point.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui::controllers;

namespace thewarrior::ui {

constexpr float TextRevealCharactersPerSecond = 45.0F;
constexpr float NextPageArrowBlinkPeriodInSeconds = 0.8F;

GLTextBox::GLTextBox()
: GLPopupWindow({ 1.0F, 1.0F }),
m_itemStore(nullptr),
m_messageDTO(nullptr),
m_computedTextForDisplay({Size<float>(0.0F, 0.0F), {}}) {
}

GLTextBox::~GLTextBox() {
    freeGLObjects(m_nextPageArrowObjects);
}

void GLTextBox::initialize(const std::string &resourcePath,
                           std::shared_ptr<GLTextService> textService,
                           std::shared_ptr<ItemStore> itemStore,
                           const std::map<std::string, unsigned int> *texturesGLItemStore) {
    GLPopupWindow::initialize("", resourcePath, textService);
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO) {
    m_messageDTO = messageDTO;
    m_computedTextForDisplay = m_textService->prepareTextForDisplay(m_screenSize, m_messageDTO->message, messageDTO->scale);
    freeGLObjects(m_nextPageArrowObjects);
    m_isNextPageArrowGenerated = false;
    m_nextPageArrowBlinkElapsedTime = 0.0F;
    const auto totalCharacterCount = getTotalCharacterCount();
    if (m_messageDTO->getType() == MessageDTOType::NPCDialogueMessage) {
        m_visibleCharacterCount = 0.0F;
        m_isRevealingText = totalCharacterCount > 0;

    } else {
        m_visibleCharacterCount = static_cast<float>(totalCharacterCount);
        m_isRevealingText = false;
    }
    m_windowSize.setSize(m_computedTextForDisplay.textSize.width() + BOXPADDING,
                         m_computedTextForDisplay.textSize.height() + getImageHeight() + BOXPADDING);
    m_windowLocation.setX((m_screenSize.width() / 2.0F) - (m_windowSize.width() / 2.0F));
    m_windowLocation.setY((m_screenSize.height() / 2.0F) - (m_windowSize.height() / 2.0F));


    GLPopupWindow::generateGLElements();

    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        auto *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
        auto item = m_itemStore->findItem(itemFoundMsgDTO->itemId);
        if (!item) {
            throw std::runtime_error(fmt::format("Unable to found the item {0}", itemFoundMsgDTO->itemId));
        }
        // Find the texture
        auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
        if (!texture.has_value()) {
            throw std::runtime_error(fmt::format("Unable to found the texture {0}", item->getTextureName()));
        }
        m_glFormService->generateQuad(m_glObjects,
                                     { m_windowLocation.x() + (m_windowSize.width() / 2.0F) - (getImageHeight() / 2.0F),
                                       m_windowLocation.y() + m_windowSize.height() - getImageHeight() - 10.0F },
                                     { getImageHeight(), getImageHeight() },
                                     &texture.value().get(),
                                     item->getTextureIndex(),
                                     (*m_texturesGLItemStore).at(item->getTextureName()));
    } else if (m_messageDTO->getType() == MessageDTOType::NPCDialogueMessage) {
        generateNextPageArrowIfNeeded();
    }
}

void GLTextBox::update(float deltaTime) {
    if (!m_isRevealingText) {
        if (m_isNextPageArrowGenerated) {
            m_nextPageArrowBlinkElapsedTime += deltaTime;
        }
        return;
    }

    m_visibleCharacterCount += TextRevealCharactersPerSecond * deltaTime;
    if (m_visibleCharacterCount >= static_cast<float>(getTotalCharacterCount())) {
        revealAllText();
    }
}

void GLTextBox::draw() {
    GLPopupWindow::render();
    if (m_isNextPageArrowGenerated &&
        std::fmod(m_nextPageArrowBlinkElapsedTime, NextPageArrowBlinkPeriodInSeconds) <
            (NextPageArrowBlinkPeriodInSeconds / 2.0F)) {
        for (const auto &obj : m_nextPageArrowObjects) {
            m_glFormService->drawQuad(obj, obj.textureGLId);
        }
    }

    m_textService->useShader();
    if (m_computedTextForDisplay.lines.empty()) {
        return;
    }
    auto lineTotal = static_cast<float>(m_computedTextForDisplay.lines.size());
    float lineHeight = (m_computedTextForDisplay.textSize.height() / lineTotal) - 10.0F;

    Point<float> messagePosition((m_screenSize.width() / 2.0F) - (m_computedTextForDisplay.textSize.width() / 2.0F),
                                 (m_screenSize.height() / 2.0F) + (((lineHeight + 10.0F) * lineTotal) / 2.0F) - lineHeight);
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        messagePosition.setY(messagePosition.y() + 20.0F);
    }
    auto remainingVisibleCharacters = static_cast<size_t>(m_visibleCharacterCount);
    for (size_t i = 0; i < m_computedTextForDisplay.lines.size(); i++) {
        const auto &line = m_computedTextForDisplay.lines[i];
        const auto visibleCharactersForLine = std::min(line.size(), remainingVisibleCharacters);
        const auto visibleLine = line.substr(0, visibleCharactersForLine);
        remainingVisibleCharacters -= visibleCharactersForLine;

        m_textService->renderText(visibleLine,
                                  messagePosition.x(),
                                  messagePosition.y() - (static_cast<float>(i) * (lineHeight + 10.0F)),
                                  m_messageDTO->scale,
                                  glm::vec3(1.0F, 1.0F, 1.0F));       // Color
    }
}

bool GLTextBox::isRevealingText() const {
    return m_isRevealingText;
}

void GLTextBox::revealAllText() {
    m_visibleCharacterCount = static_cast<float>(getTotalCharacterCount());
    m_isRevealingText = false;
    generateNextPageArrowIfNeeded();
}

float GLTextBox::getImageHeight() const {
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        return ITEMICONSIZE;
    } else if (m_messageDTO->getType() == MessageDTOType::NPCDialogueMessage) {
        return 32.0F;
    }
    return 0.0F;
}

void GLTextBox::gameWindowSizeChanged(const Size<> &size) {
    GLPopupWindow::gameWindowSizeChanged(size);
    // Resize currently displayed message
    if (m_messageDTO) {
        const auto visibleCharacterCount = m_visibleCharacterCount;
        const auto isRevealingText = m_isRevealingText;
        generateMessage(m_messageDTO);
        m_visibleCharacterCount = std::min(visibleCharacterCount, static_cast<float>(getTotalCharacterCount()));
        m_isRevealingText = isRevealingText && m_visibleCharacterCount < static_cast<float>(getTotalCharacterCount());
        generateNextPageArrowIfNeeded();
    }
}

size_t GLTextBox::getTotalCharacterCount() const {
    size_t total = 0;
    for (const auto &line : m_computedTextForDisplay.lines) {
        total += line.size();
    }
    return total;
}

void GLTextBox::generateNextPageArrowIfNeeded() {
    if (m_isNextPageArrowGenerated ||
        m_isRevealingText ||
        m_messageDTO->getType() != MessageDTOType::NPCDialogueMessage) {
        return;
    }

    auto *npcDialogueMsgDTO = dynamic_cast<NPCDialogueMessageDTO *>(m_messageDTO.get());
    if (npcDialogueMsgDTO == nullptr || !npcDialogueMsgDTO->hasNextPage) {
        return;
    }

    m_glFormService->generateQuad(m_nextPageArrowObjects,
                                  { m_windowLocation.x() + (m_windowSize.width() / 2.0F) - (getImageHeight() / 2.0F),
                                    m_windowLocation.y() + m_windowSize.height() - getImageHeight() - 10.0F },
                                  { getImageHeight(), getImageHeight() },
                                  &m_windowGLTexture.texture,
                                  25,
                                  m_windowGLTexture.glTextureId);
    m_isNextPageArrowGenerated = true;
}

}  // namespace thewarrior::ui
