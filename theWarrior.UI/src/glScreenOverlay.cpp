#include "fmt/format.h"
#include "glScreenOverlay.hpp"
#include <algorithm>
#include <string>

namespace thewarrior::ui {

GLScreenOverlay::~GLScreenOverlay() {
    unload();
}

bool GLScreenOverlay::initialize(
    const std::string &resourcesPath) {

    m_shaderProgram = std::make_shared<GLShaderProgram>(
        fmt::format("{}/shaders/overlay_330_vs.glsl",resourcesPath),
        fmt::format("{}/shaders/overlay_330_fs.glsl",resourcesPath));

    if (!m_shaderProgram->compileShaders()) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }

    if (!m_shaderProgram->linkShaders({"in_Position"})) {
        m_lastError = m_shaderProgram->getLastError();
        return false;
    }

    const auto programId = m_shaderProgram->getShaderProgramID();

    m_colorUniform = glGetUniformLocation(programId, "overlayColor");

    m_opacityUniform = glGetUniformLocation(programId, "overlayOpacity");

    if (m_colorUniform == -1 || m_opacityUniform == -1) {
        m_lastError = "Unable to find screen-overlay shader uniforms.";
        return false;
    }

    constexpr GLfloat vertices[] = {
        -1.0F,  1.0F,
         1.0F,  1.0F,
         1.0F, -1.0F,
        -1.0F, -1.0F
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void GLScreenOverlay::render(float opacity, float brightness) const {
    if (m_shaderProgram == nullptr || m_vao == 0 || opacity <= 0.0F) {
        return;
    }

    const float clampedOpacity = std::clamp(opacity, 0.0F, 1.0F);

    const auto programId = m_shaderProgram->getShaderProgramID();

    glUseProgram(programId);

    glUniform3f(
        m_colorUniform,
        brightness,
        brightness,
        brightness);

    glUniform1f(m_opacityUniform, clampedOpacity);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
}

void GLScreenOverlay::unload() {
    if (m_positionVbo != 0) {
        glDeleteBuffers(1, &m_positionVbo);
        m_positionVbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    m_shaderProgram.reset();
}

const std::string &GLScreenOverlay::getLastError() const {
    return m_lastError;
}

}  // namespace thewarrior::ui
