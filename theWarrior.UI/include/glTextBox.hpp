#pragma once

#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "message.hpp"
#include "messageDTO.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <string>

class GLTextBox : public IShaderService
{
public:
    GLTextBox();
    ~GLTextBox() = default;
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    const std::string &getLastError() const;
    void setItemStore(std::shared_ptr<ItemStore> itemStore);
    void setItemStoreTextureMap(const std::map<std::string, unsigned int> *texturesGLItemStore);
    void generateMessage(std::shared_ptr<MessageDTO> messageDTO, int screenWidth, int screenHeight);
    void useShader();
    void draw(GLTextService &textService, int screenWidth, int screenHeight);
private:
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::unique_ptr<GLShaderProgram> shaderProgram;
    std::string m_lastError;
    std::shared_ptr<MessageDTO> m_messageDTO;
    GLObject m_glObject;
    GLObject m_glObjectIcon;
};