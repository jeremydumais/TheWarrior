#include <variant>
#include "glPanel.hpp"
#include "glComponentBase.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLPanel::GLPanel(GLContext &glContext,
                 Size<float> size,
                 GLPanelCreationInfo info)
: GLComponentBase(glContext, info.location, size, info.horizontalAlignment, info.verticalAlignment),
m_skin(info.skin) {}

GLPanel::~GLPanel() {
    GLComponentBase::freeGLObjects(m_windowObjects);
    GLComponentBase::freeGLObjects(m_windowBackgrounds);
}

std::variant<NineSliceSkin, SingleTextureSkin> GLPanel::getSkin() const {
    return m_skin;
}

void GLPanel::setSkin(const std::variant<NineSliceSkin, SingleTextureSkin> &skin) {
    m_skin = skin;
}

void GLPanel::onInitialize(const GLComponentBaseInfo &) {}

void GLPanel::onGenerateGLElements() {
    GLComponentBase::freeGLObjects(m_windowObjects);
    GLComponentBase::freeGLObjects(m_windowBackgrounds);
    std::visit([&](auto&& skin) {
        using T = std::decay_t<decltype(skin)>;

        if constexpr (std::is_same_v<T, NineSliceSkin>) {
            generateQuad(m_windowBackgrounds, m_initialLocation,
                    m_size, &m_glTexture->texture,
                    skin.backgroundTextureIndex);
            generateBoxQuad(m_windowObjects, m_initialLocation,
                    m_size, &m_glTexture->texture,
                    skin.firstTextureIndex);
        } else if constexpr (std::is_same_v<T, SingleTextureSkin>) {
            generateGLObject(skin.textureName,
                             Size<int>(static_cast<int>(m_size.width()),
                                       static_cast<int>(m_size.height())),
                             m_horizontalAlignment,
                             m_verticalAlignment,
                             Point<int>(static_cast<int>(m_initialLocation.x()),
                                        static_cast<int>(m_initialLocation.y())));
        }
    }, m_skin);
}

void GLPanel::onRender() {
    std::visit([&](auto&& skin) {
        using T = std::decay_t<decltype(skin)>;

        if constexpr (std::is_same_v<T, NineSliceSkin>) {
            for (const auto &obj : m_windowBackgrounds) {
                m_glFormService->drawQuad(obj, m_glTexture->glTextureId, 1.0F);
            }
            for (const auto &obj : m_windowObjects) {
                m_glFormService->drawQuad(obj, m_glTexture->glTextureId);
            }
        } else if constexpr (std::is_same_v<T, SingleTextureSkin>) {
            drawGLObject(skin.textureName);
        }
    }, m_skin);
}

void GLPanel::onGameWindowSizeChanged(const thewarrior::models::Size<int> &size) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
    std::visit([&](auto&& skin) {
        using T = std::decay_t<decltype(skin)>;

        if constexpr (std::is_same_v<T, NineSliceSkin>) {
            m_glFormService->gameWindowSizeChanged(size);
        }
    }, m_skin);
}

}  // namespace thewarrior::ui::components
