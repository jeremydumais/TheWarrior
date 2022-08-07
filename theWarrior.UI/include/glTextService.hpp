#pragma once

#include "glColor.hpp"
#include "glShaderProgram.hpp"
#include "IShaderService.hpp"
#include "point.hpp"
#include <size.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace thewarrior::ui {

struct GLTextObject
{
    std::string text;
    Point<float> position;
    float scale;
    GLColor color = GLColor::White;
};

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

struct ComputedTextForDisplay{
    Size<float> textSize;
    std::vector<std::string> lines;
};

class GLTextService : public IShaderService
{
public:
    ~GLTextService();
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initShader(const std::shared_ptr<GLShaderProgram> shaderProgram) override;
    bool initFont(const std::string &fontFileName);
    const std::string &getLastError() const;
    void useShader();
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
    Size<float> getTextSize(const std::string &text, float scale) const;
    ComputedTextForDisplay prepareTextForDisplay(Size<float> screenSize, const std::string &text, float scale) const;
    void wrapLinesFromMaxScreenWidth(std::vector<std::string> &lines, const float maxWidth, const float scale) const;
    void gameWindowSizeChanged(const Size<> &size);
private:
    std::string m_lastError;
    std::shared_ptr<GLShaderProgram> m_shaderProgram;
    std::map<GLchar, Character> characters;
    unsigned int VAO, VBO;
};

} // namespace thewarrior::ui
