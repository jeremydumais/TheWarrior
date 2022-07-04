#include "glTextService.hpp"
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>

using namespace std;

GLTextService::~GLTextService()
{
    for(auto &character : characters) {
        glDeleteBuffers(1, &character.second.TextureID);
    }
}

bool GLTextService::initShader(const std::string &vertexShaderFileName,
                         const std::string &fragmentShaderFileName) 
{
    m_shaderProgram = make_shared<GLShaderProgram>(vertexShaderFileName,
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

void GLTextService::initShader(const std::shared_ptr<GLShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

const string& GLTextService::getLastError() const
{
    return m_lastError;
}

void GLTextService::useShader() 
{
    m_shaderProgram->use();
}

bool GLTextService::initFont(const std::string &fontFileName) 
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        m_lastError = "ERROR::FREETYPE: Could not init FreeType Library";
        return false;
    }

	// find path to font
    if (fontFileName.empty()) {
        m_lastError = "ERROR::FREETYPE: Failed to load fontFileName";
        return false;
    }
    FT_Face face;
    if (FT_New_Face(ft, fontFileName.c_str(), 0, &face)) {
        m_lastError = "ERROR::FREETYPE: Failed to load font";
        return false;
    }
    else {
        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                m_lastError = "ERROR::FREETYTPE: Failed to load Glyph";
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                static_cast<GLsizei>(face->glyph->bitmap.width),
                static_cast<GLsizei>(face->glyph->bitmap.rows),
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

void GLTextService::renderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUniform3f(glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = characters[*c];

        float xpos = x + floor(static_cast<float>(ch.Bearing.x) * scale);
        float ypos = y - floor(static_cast<float>(ch.Size.y - ch.Bearing.y) * scale);

        float w = floor(static_cast<float>(ch.Size.x) * scale);
        float h = floor(static_cast<float>(ch.Size.y) * scale);
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0F, 0.0F },            
            { xpos,     ypos,       0.0F, 1.0F },
            { xpos + w, ypos,       1.0F, 1.0F },

            { xpos,     ypos + h,   0.0F, 0.0F },
            { xpos + w, ypos,       1.0F, 1.0F },
            { xpos + w, ypos + h,   1.0F, 0.0F }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += floor(static_cast<float>(ch.Advance >> 6) * scale); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Size<float> GLTextService::getTextSize(const std::string &text, float scale) const
{
    std::string::const_iterator c;
    Size<float> totalSize(0.0F, 0.0F);

    for (c = text.begin(); c != text.end(); c++) {
        Character ch = characters.at(*c);
        float charHeight = static_cast<float>(ch.Size.y) * scale;
        float totalWidth = totalSize.width();
        float advance = floor(static_cast<float>(ch.Advance >> 6) * scale); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        totalSize.setWidth(totalWidth + advance);
        if (totalSize.height() < charHeight) {
            totalSize.setHeight(charHeight);
        }
    }
    
    return totalSize;
}

ComputedTextForDisplay GLTextService::prepareTextForDisplay(Size<float> screenSize, const std::string &text, float scale) const
{
    const float DISPLAYMAXWIDTH = screenSize.width() - 60.0F;
    const float DISPLAYMINWIDTH = 400.0F;
    float lineHeight = 0.0F;
    ComputedTextForDisplay retval { Size<float>(DISPLAYMINWIDTH, 0.0F), {}};
    //Split the string by carriage return \n
    boost::split(retval.lines, text, boost::is_any_of("\n"));
    //Ensure each line is not too large
    wrapLinesFromMaxScreenWidth(retval.lines, DISPLAYMAXWIDTH, scale);
    for(size_t indexLine = 0; indexLine < retval.lines.size(); indexLine++) {   
        const auto lineSize = getTextSize(retval.lines[indexLine], scale);
        if (lineSize.width() > DISPLAYMINWIDTH && lineSize.width() <= DISPLAYMAXWIDTH && lineSize.width() > retval.textSize.width()) {
            retval.textSize.setWidth(lineSize.width());
        }

        if (lineSize.height() > lineHeight) {
            retval.textSize.setHeight(lineSize.height());
        }
    }
    //TODO const float LinesVerticalSpacing 10.0F
    retval.textSize.setHeight((retval.textSize.height() + 10.0F) * static_cast<float>(retval.lines.size()));

    return retval;
}

void GLTextService::wrapLinesFromMaxScreenWidth(std::vector<std::string> &lines, const float maxWidth, const float scale) const
{
    const float SPACESIZE = getTextSize(" ", scale).width();
    for(size_t indexLine = 0; indexLine < lines.size(); indexLine++) {
        auto &line = lines[indexLine];
        const auto lineSize = getTextSize(line, scale);
        if (lineSize.width() > maxWidth)
        {
            std::string lineNewContent = "";
            std::string lineOverflow = "";
            std::vector<std::string> words = {};
            boost::split(words, line, boost::is_any_of(" "));
            bool overflow = false;
            for(const auto &word : words) {
                if (overflow) {
                    lineOverflow += " " + word;
                    continue;
                }
                if (lineNewContent == "") {
                    lineNewContent += word;
                    continue;
                }
                else {
                    Size<float> wordSize = getTextSize(word, scale);
                    Size<float> lineNewContentSize = getTextSize(lineNewContent, scale);
                    if (lineNewContentSize.width() + SPACESIZE + wordSize.width() <= maxWidth) {
                        lineNewContent += " " + word;
                    }
                    else {
                        overflow = true;
                        lineOverflow += word;
                    }
                }
            }
            line = lineNewContent;
            lines.insert(lines.begin() + static_cast<long int>(indexLine) + 1, lineOverflow);
        }
    }
}

void GLTextService::gameWindowSizeChanged(const Size<> &size)
{
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(size.width()), 0.0f, static_cast<float>(size.height()));
    m_shaderProgram->use();
    GLint projectionUniformLocation  = glGetUniformLocation(m_shaderProgram->getShaderProgramID(), "projection");
    glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

