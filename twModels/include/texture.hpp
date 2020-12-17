#pragma once

#include <string>

class Texture
{
public:
    explicit Texture(const std::string &name, const std::string &filename);
    const std::string &getName() const;
    const std::string &getFilename() const;
    void setName(const std::string &name);
    void setFilename(const std::string &filename);
private:
    std::string name;
    std::string filename;
};