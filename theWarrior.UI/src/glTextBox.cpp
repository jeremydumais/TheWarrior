#include "glTextBox.hpp"
#include "itemFoundMessageDTO.hpp"
#include "point.hpp"
#include <fmt/format.h>
#include <stdexcept>

GLTextBox::GLTextBox()
    : m_screenSize(1.0F, 1.0F),
      m_textService(nullptr),
      m_itemStore(nullptr),
      m_lastError(""),
      m_messageDTO(nullptr),
      m_computedTextForDisplay({Size(0.0F, 0.0F), {}}),
      m_windowObjects(std::vector<GLObject>(WINDOW_OBJ_MAX)),
      m_windowGLTexture({ Texture(TextureInfo { "window", "window.png", 256, 256, 32, 32 }), 0 })
{
}

bool GLTextBox::initShader(const std::string &vertexShaderFileName,
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

void GLTextBox::initialize(const std::string &resourcePath,
                           std::shared_ptr<GLTextService> textService,
                           std::shared_ptr<ItemStore> itemStore,
                           const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    m_textureService.setResourcesPath(resourcePath);
    m_textureService.loadTexture(m_windowGLTexture);
    m_textService = textService;
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
    m_glFormService.initialize(m_shaderProgram, textService);
}

const std::string &GLTextBox::getLastError() const
{
    return m_lastError;
}

void GLTextBox::generateMessage(std::shared_ptr<MessageDTO> messageDTO)
{
    m_messageDTO = messageDTO;
    m_computedTextForDisplay = m_textService->prepareTextForDisplay(m_screenSize, m_messageDTO->message, messageDTO->scale);
    Size<float> textBoxSize(m_computedTextForDisplay.textSize.width() + BOXPADDING, 
                            m_computedTextForDisplay.textSize.height() + getImageHeight() + BOXPADDING );
    Point<float> textBoxLocation((m_screenSize.width() / 2.0F) - (textBoxSize.width() / 2.0F), 
                                 (m_screenSize.height() / 2.0F) - (textBoxSize.height() / 2.0F));
    m_glFormService.generateQuad(m_glObject, textBoxLocation, textBoxSize);
    m_glFormService.generateBoxQuad(m_windowObjects.begin(),
                                    textBoxLocation,
                                    textBoxSize,
                                    &m_windowGLTexture.texture,
                                    17);
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
        m_glFormService.generateQuad(m_glObjectIcon, 
                                     { textBoxLocation.x() + (textBoxSize.width() / 2.0F) - (ITEMICONSIZE / 2.0F), 
                                       textBoxLocation.y() + textBoxSize.height() - ITEMICONSIZE - 10.0F },
                                     { ITEMICONSIZE, ITEMICONSIZE }, 
                                     &texture.value().get(), item->getTextureIndex());
    }
}

void GLTextBox::draw()
{
    m_shaderProgram->use();
    m_glFormService.drawQuad(m_glObject, 0, 0.8F);
    for(const auto &obj : m_windowObjects) {
        m_glFormService.drawQuad(obj, m_windowGLTexture.glTextureId);
    }
    if (m_messageDTO->getType() == MessageDTOType::ItemFoundMessage) {
        //Display the icon
        ItemFoundMessageDTO *dto = dynamic_cast<ItemFoundMessageDTO *>(m_messageDTO.get());
        m_glFormService.drawQuad(m_glObjectIcon, (*m_texturesGLItemStore).at(dto->textureName));
    }
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
    m_glFormService.gameWindowSizeChanged(size);
    //Resize currently displayed message
    if (m_messageDTO) {
        generateMessage(m_messageDTO);
    }
}
