#include "glShaderProgram.hpp"
#include <fmt/format.h>
#include <fstream>
#include <sstream>

using namespace std;

namespace thewarrior::ui {

GLShaderProgram::GLShaderProgram(const std::string &vertexShaderFileName,
                                 const std::string &fragmentShaderFileName)
    : m_vertexShaderFileName(vertexShaderFileName),
      m_fragmentShaderFileName(fragmentShaderFileName)
{
}

GLShaderProgram::~GLShaderProgram()
{
    glDetachShader(m_shaderprogram, m_vertexshader);
    glDetachShader(m_shaderprogram, m_fragmentshader);
    glDeleteProgram(m_shaderprogram);
    glDeleteShader(m_vertexshader);
    glDeleteShader(m_fragmentshader);
}

const std::string &GLShaderProgram::getLastError() const
{
    return m_lastError;
}

GLuint GLShaderProgram::getShaderProgramID() const
{
    return m_shaderprogram;
}

bool GLShaderProgram::compileShaders()
{
    int IsCompiled_VS;
    int IsCompiled_FS;
    /* Read our shaders into the appropriate buffers */
    string vertexShaderContent = loadShaderFile(m_vertexShaderFileName);
    string fragmentShaderContent = loadShaderFile(m_fragmentShaderFileName);

    m_vertexshader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexShaderSource = vertexShaderContent.c_str();
    glShaderSource(m_vertexshader, 1, &vertexShaderSource, 0);
    glCompileShader(m_vertexshader);
    glGetShaderiv(m_vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
    if(IsCompiled_VS == false)
    {
       int maxLength;
       glGetShaderiv(m_vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
       if (maxLength > 0) {
        unique_ptr<char[]> vertexInfoLog = make_unique<char[]>(static_cast<size_t>(maxLength));
        glGetShaderInfoLog(m_vertexshader, maxLength, &maxLength, vertexInfoLog.get());
        m_lastError = fmt::format("Unable to compile vertex shader: {0}", vertexInfoLog.get());
       }
       else {
           m_lastError = "Unknown error in vertex shader compilation";
       }
       return false;
    }

    m_fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = fragmentShaderContent.c_str();
    glShaderSource(m_fragmentshader, 1, &fragmentShaderSource, 0);
    glCompileShader(m_fragmentshader);
    glGetShaderiv(m_fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
    if(IsCompiled_FS == false)
    {
       int maxLength;
       glGetShaderiv(m_fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
       if (maxLength > 0) {
        unique_ptr<char[]> fragmentInfoLog = make_unique<char[]>(static_cast<size_t>(maxLength));
        glGetShaderInfoLog(m_fragmentshader, maxLength, &maxLength, fragmentInfoLog.get());
        m_lastError = fmt::format("Unable to compile fragment shader: {0}", fragmentInfoLog.get());
       }
       else {
           m_lastError = "Unknown error in fragment shader compilation";
       }
       return false;
    }
    return true;
}

string GLShaderProgram::loadShaderFile(const string &file)
{
    stringstream retVal;
    ifstream shaderFileStream(file, ios::in);
    if (shaderFileStream.is_open()) {
        string line;
        while (getline(shaderFileStream, line)) {
            retVal << line << '\n';
        }
        shaderFileStream.close();
    }
    return retVal.str();
}

bool GLShaderProgram::linkShaders(const vector<string> &attributes)
{
    m_shaderprogram = glCreateProgram();
    glAttachShader(m_shaderprogram, m_vertexshader);
    glAttachShader(m_shaderprogram, m_fragmentshader);

    for(size_t index=0; index < attributes.size(); index++) {
        glBindAttribLocation(m_shaderprogram, static_cast<GLuint>(index), attributes[index].c_str());
    }
    //glBindAttribLocation(shaderprogram, 0, "in_Position");
    //glBindAttribLocation(shaderprogram, 1, "in_Color");
    //glBindAttribLocation(shaderprogram, 2, "in_VertexUV");

    glLinkProgram(m_shaderprogram);
    int IsLinked;
    glGetProgramiv(m_shaderprogram, GL_LINK_STATUS, &IsLinked);
    if(IsLinked == false)
    {
        int maxLength;
        glGetProgramiv(m_shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 0) {
            unique_ptr<char[]> shaderProgramInfoLog = make_unique<char[]>(static_cast<size_t>(maxLength));
            glGetProgramInfoLog(m_shaderprogram, maxLength, &maxLength, shaderProgramInfoLog.get());
            m_lastError = fmt::format("Unable to link shaders: {0}", shaderProgramInfoLog.get());
        }
        else {
           m_lastError = "Unknown error in linking shaders";
        }
        return false;
    }
    return true;
}

void GLShaderProgram::use()
{
    glUseProgram(m_shaderprogram);
}

} // namespace thewarrior::ui
