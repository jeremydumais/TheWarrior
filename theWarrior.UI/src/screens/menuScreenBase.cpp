#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "menuScreenBase.hpp"
#include "mainMenuCommons.hpp"
#include "glComponentBase.hpp"
#include "size.hpp"
#include "texture.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

MenuScreenBase::MenuScreenBase(std::map<std::string, std::shared_ptr<Texture>> &textures,
                               std::map<std::string, unsigned int> &texturesGL)
: components::GLComponentBase(Point<float>(0.0F, 0.0F), Size<float>(0.0F, 0.0F)),
m_textures(textures),
m_texturesGL(texturesGL) {
}

MenuScreenBase::~MenuScreenBase() {
    freeGLObjects(m_namedObjects);
}

void MenuScreenBase::initializeBase(const components::GLComponentBaseInfo &info) {
    components::GLComponentBase::initialize(info);
}

const std::string& MenuScreenBase::getLastError() const {
    return m_lastError;
}

void MenuScreenBase::onGenerateGLElements() {
    freeGLObjects(m_namedObjects);
    std::vector<GLObject> menuObjects = {};
    m_glFormService->generateQuad(menuObjects,
                                  { 0.0F, 0.0F },
                                  { 1.0F, 1.0F}, m_textures[TextureBackground].get(), 25, m_texturesGL[TextureBackground]);
    m_namedObjects[TextureBackground] = menuObjects.at(0);
}

void MenuScreenBase::onRender() {
    m_glFormService->drawQuad(m_namedObjects[TextureBackground], m_texturesGL[TextureBackground]);
}

void MenuScreenBase::freeGLObjects(std::map<std::string, GLObject> &objects) {
    for (auto &item : objects) {
        if (item.second.vboPosition) glDeleteBuffers(1, &item.second.vboPosition);
        if (item.second.vboColor) glDeleteBuffers(1, &item.second.vboColor);
        if (item.second.vboTexture) glDeleteBuffers(1, &item.second.vboTexture);
        if (item.second.vao) glDeleteVertexArrays(1, &item.second.vao);
    }
    objects.clear();
}

}  // namespace thewarrior::ui::screens
