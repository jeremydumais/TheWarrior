#pragma once

#include <string>

class IShaderService
{
public:
    virtual bool initShader(const std::string &vertexShaderFileName,
                            const std::string &fragmentShaderFileName) = 0;
private:
};