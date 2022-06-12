#pragma once

#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "message.hpp"
#include "messageDTO.hpp"
#include "point.hpp"
#include "size.hpp"
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
    void setTextService(GLTextService *textService);
    void setItemStore(std::shared_ptr<ItemStore> itemStore);
    void setItemStoreTextureMap(const std::map<std::string, unsigned int> *texturesGLItemStore);
    void generateMessage(std::shared_ptr<MessageDTO> messageDTO);
    void useShader();
    void draw();
    void gameWindowSizeChanged(const Size<> &size);
private:
    Size<float> m_screenSize;
    GLTextService *m_textService;
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::unique_ptr<GLShaderProgram> shaderProgram;
    std::string m_lastError;
    std::shared_ptr<MessageDTO> m_messageDTO;
    ComputedTextForDisplay m_computedTextForDisplay;
    GLObject m_glObject;
    GLObject m_glObjectIcon;
    const float BOXPADDING = 60.0F;
    void generateGLTextBox();
    void generateGLIcon();
    Point<float> getBoxStartPosition() const;
    float getBoxHalfHeight() const;
    float getComputedBoxPadding() const;
    float getImageHeight() const;
    void drawQuad(const GLObject &glObject, GLuint textureGLIndex);
};