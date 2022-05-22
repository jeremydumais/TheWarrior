#pragma once

#include "glObjectService.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
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
    void generateMessage(std::shared_ptr<MessageDTO> messageDTO);
    void useShader();
    void draw(GLTextService &textService, int screenWidth, int screenHeight);
private:
    std::unique_ptr<GLShaderProgram> shaderProgram;
    std::string m_lastError;
    std::string m_message;
    GLObject m_glObject;
    GLObject m_glObjectIcon;
};