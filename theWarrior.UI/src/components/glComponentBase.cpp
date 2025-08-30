#include <memory>
#include "glComponentBase.hpp"
#include "glFormService.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLComponentBase::GLComponentBase()
: m_screenSize(Size<float>(1.0F, 1.0F)),
m_shaderProgram(nullptr),
m_glFormService(std::make_shared<GLFormService>()),
m_textService(nullptr) {}

void GLComponentBase::initialize(const std::shared_ptr<GLTexture> texture,
                    const std::shared_ptr<GLShaderProgram> shaderProgram,
                    std::shared_ptr<GLTextService> textService) {
    m_glTexture = texture;
    m_shaderProgram = shaderProgram;
    m_textService = textService;
    m_glFormService->initialize(m_shaderProgram, textService);
}

void GLComponentBase::generateGLElements() {
    onGenerateGLElements();
}

void GLComponentBase::render() {
    onRender();
}

void GLComponentBase::gameWindowSizeChanged(const thewarrior::models::Size<int> &size) {
    m_screenSize = Size<float>(static_cast<float>(size.width()),
            static_cast<float>(size.height()));
    m_glFormService->gameWindowSizeChanged(size);
    onGameWindowSizeChanged(size);
}

void GLComponentBase::freeGLObjects(std::vector<GLObject> &objects) {
    for (auto &item : objects) {
        if (item.vboPosition) glDeleteBuffers(1, &item.vboPosition);
        if (item.vboColor) glDeleteBuffers(1, &item.vboColor);
        if (item.vboTexture) glDeleteBuffers(1, &item.vboTexture);
        if (item.vao) glDeleteVertexArrays(1, &item.vao);
    }
    objects.clear();
}

}  // namespace thewarrior::ui::components
