#include "glTextBox.hpp"
#include "itemFoundMessageDTO.hpp"
#include "point.hpp"
#include <fmt/format.h>
#include <stdexcept>

using namespace thewarrior::ui::controllers;

namespace thewarrior::ui {

GLTextBox::GLTextBox()
    : GLPopupWindow({ 1.0F, 1.0F }),
      m_screenSize(1.0F, 1.0F),
      m_itemStore(nullptr),
      m_lastError(""),
      m_messageDTO(nullptr),
      m_computedTextForDisplay({Size(0.0F, 0.0F), {}})
{
}

void GLTextBox::initialize(const std::string &resourcePath,
                           std::shared_ptr<GLTextService> textService,
                           std::shared_ptr<ItemStore> itemStore,
                           const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    GLPopupWindow::initialize("", resourcePath, textService);
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_windowGLTexture);
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
}

const std::string &GLTextBox::getLastError() const
{
    return m_lastError;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO)
{
    m_messageDTO = messageDTO;
    m_computedTextForDisplay = m_textService->prepareTextForDisplay(m_screenSize, m_messageDTO->message, messageDTO->scale);
    m_windowSize.setSize(m_computedTextForDisplay.textSize.width() + BOXPADDING,
                         m_computedTextForDisplay.textSize.height() + getImageHeight() + BOXPADDING);
    m_windowLocation.setX((m_screenSize.width() / 2.0F) - (m_windowSize.width() / 2.0F));
    m_windowLocation.setY((m_screenSize.height() / 2.0F) - (m_windowSize.height() / 2.0F));

    GLPopupWindow::generateGLElements();

    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        ItemFoundMessageDTO *itemFoundMsgDTO = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
        auto item = m_itemStore->findItem(itemFoundMsgDTO->itemId);
        if (!item) {
            throw std::runtime_error(fmt::format("Unable to found the item {0}", itemFoundMsgDTO->itemId));
        }
        //Find the texture
        auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
        if (!texture.has_value()) {
            throw std::runtime_error(fmt::format("Unable to found the texture {0}", item->getTextureName()));
        }
        m_glFormService->generateQuad(m_glObjects,
                                     { m_windowLocation.x() + (m_windowSize.width() / 2.0F) - (ITEMICONSIZE / 2.0F),
                                       m_windowLocation.y() + m_windowSize.height() - ITEMICONSIZE - 10.0F },
                                     { ITEMICONSIZE, ITEMICONSIZE },
                                     &texture.value().get(),
                                     item->getTextureIndex(),
                                     (*m_texturesGLItemStore).at(item->getTextureName()));
    }
}

void GLTextBox::draw()
{
    GLPopupWindow::render();
    m_textService->useShader();
    float lineTotal = static_cast<float>(m_computedTextForDisplay.lines.size());
    float lineHeight = (m_computedTextForDisplay.textSize.height() / lineTotal) - 10.0F;

    Point<float> messagePosition((m_screenSize.width() / 2.0F) - (m_computedTextForDisplay.textSize.width() / 2.0F),
                                 (m_screenSize.height() / 2.0F) + (m_computedTextForDisplay.textSize.height() / 2.0F) - (BOXPADDING / 2.0F));
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        messagePosition.setY(messagePosition.y() + 20.0F);
    }
    for(size_t i = 0; i < m_computedTextForDisplay.lines.size(); i++) {
        m_textService->renderText(m_computedTextForDisplay.lines[i],
                                  messagePosition.x(),
                                  messagePosition.y() - (static_cast<float>(i) * (lineHeight + 10.0F)),
                                  m_messageDTO->scale,
                                  glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
}
float GLTextBox::getImageHeight() const
{
    return (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) ? ITEMICONSIZE : 0.0F;
}

void GLTextBox::gameWindowSizeChanged(const Size<> &size)
{
    m_screenSize.setSize(static_cast<float>(size.width()),
                         static_cast<float>(size.height()));
    GLPopupWindow::gameWindowSizeChanged(size);
    //Resize currently displayed message
    if (m_messageDTO) {
        generateMessage(m_messageDTO);
    }
}

} // namespace thewarrior::ui
