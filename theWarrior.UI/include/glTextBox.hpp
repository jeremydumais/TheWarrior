#pragma once

#include "message.hpp"
#include "messageDTO.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>

class GLTextBox
{
public:
    GLuint vao;
    GLuint vboPosition;
    GLuint vboColor;
    GLuint vboTexture;
    void showMessage(std::shared_ptr<MessageDTO> messageDTO);
};