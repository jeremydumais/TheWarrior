#pragma once

#include "glShaderProgram.hpp"
#include <memory>
#include <string>

class IShaderService
{
public:
    virtual bool initShader(const std::string &vertexShaderFileName,
                            const std::string &fragmentShaderFileName) = 0;
    virtual void initShader(const std::shared_ptr<GLShaderProgram> shaderProgram) = 0;
    virtual ~IShaderService() = default;
};