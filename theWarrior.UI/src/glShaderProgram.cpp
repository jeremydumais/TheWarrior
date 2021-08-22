#include "glShaderProgram.hpp"
#include <fmt/format.h>
#include <fstream>
#include <sstream>

using namespace std;

GLShaderProgram::GLShaderProgram(const std::string &vertexShaderFileName,
                                 const std::string &fragmentShaderFileName) 
    : vertexShaderFileName(vertexShaderFileName),
      fragmentShaderFileName(fragmentShaderFileName)
{
}

GLShaderProgram::~GLShaderProgram() 
{
    glDetachShader(shaderprogram, vertexshader);
    glDetachShader(shaderprogram, fragmentshader);
    glDeleteProgram(shaderprogram);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);
}

const std::string &GLShaderProgram::getLastError() const
{
    return lastError;
}

GLuint GLShaderProgram::getShaderProgramID() const
{
    return shaderprogram;
}

bool GLShaderProgram::compileShaders() 
{
    int IsCompiled_VS;
    int IsCompiled_FS;
    char *vertexInfoLog;
    char *fragmentInfoLog;
    /* Read our shaders into the appropriate buffers */
    string vertexShaderContent = loadShaderFile(vertexShaderFileName);
    string fragmentShaderContent = loadShaderFile(fragmentShaderFileName);

    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexShaderSource = vertexShaderContent.c_str();
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexShaderSource, 0);
    glCompileShader(vertexshader);
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
    if(IsCompiled_VS == false)
    {
       int maxLength;
       glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
       vertexInfoLog = (char *)malloc(maxLength);
       glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
       lastError = fmt::format("Unable to compile vertex shader: {0}", vertexInfoLog);
       free(vertexInfoLog);
       return false;
    }

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = fragmentShaderContent.c_str();
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentShaderSource, 0);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
    if(IsCompiled_FS == false)
    {
       int maxLength;
       glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
       fragmentInfoLog = (char *)malloc(maxLength);
       glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
       lastError = fmt::format("Unable to compile fragment shader: {0}", fragmentInfoLog);
       free(fragmentInfoLog);
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
    shaderprogram = glCreateProgram();
    glAttachShader(shaderprogram, vertexshader);
    glAttachShader(shaderprogram, fragmentshader);

    for(size_t index=0; index < attributes.size(); index++) {
        glBindAttribLocation(shaderprogram, index, attributes[index].c_str());
    }
    //glBindAttribLocation(shaderprogram, 0, "in_Position");
    //glBindAttribLocation(shaderprogram, 1, "in_Color");
    //glBindAttribLocation(shaderprogram, 2, "in_VertexUV");

    glLinkProgram(shaderprogram);
    int IsLinked;
    glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int *)&IsLinked);
    if(IsLinked == false)
    {
       int maxLength;
       glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
       char *shaderProgramInfoLog = (char *)malloc(maxLength);
       glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);
       lastError = shaderProgramInfoLog;
       free(shaderProgramInfoLog);
       return false;
    }
    return true;
}

void GLShaderProgram::use() 
{
    glUseProgram(shaderprogram);
}
