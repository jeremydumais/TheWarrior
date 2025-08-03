#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>
#include "glMenuButton.hpp"
#include "glColor.hpp"
#include "glTexture.hpp"
#include "point.hpp"
#include <boost/algorithm/string.hpp>

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLMenuButton::GLMenuButton(Point<float> location, Size<float> size)
: m_location(location),
m_initialLocation(location),
m_size(size),
m_windowCenter({1.0F, 1.0F}),
m_screenSize({1.0F, 1.0F}),
m_shaderProgram(nullptr),
m_glFormService(std::make_shared<GLFormService>()),
m_textService(nullptr),
m_windowGLTexture(nullptr),
m_glCaption({"", {1.0F, 1.0F}, 0.6F}),
m_windowObjects(std::vector<GLObject>()),
m_windowBackgrounds(std::vector<GLObject>()),
m_textureBeginId(38),
m_hasFocus(false) {}

void GLMenuButton::initialize(const std::string &caption,
                              const std::shared_ptr<GLTexture> glTexture,
                              const std::shared_ptr<GLShaderProgram> shaderProgram,
                              std::shared_ptr<GLTextService> textService) {
  m_glCaption.text = caption;
  m_windowGLTexture = glTexture;
  m_shaderProgram = shaderProgram;
  m_textService = textService;
  m_glFormService->initialize(m_shaderProgram, textService);
}

void GLMenuButton::setCaption(const std::string &title) {
  m_glCaption.text = title;
  generateCaption();
}

void GLMenuButton::generateGLElements() {
  m_windowObjects.clear();
  m_windowBackgrounds.clear();
  // Window
  generateQuad(m_windowBackgrounds, m_location,
                                m_size, &m_windowGLTexture->texture,
                                m_textureBeginId + 8);
  generateBoxQuad(m_windowObjects, m_location,
                                   m_size, &m_windowGLTexture->texture,
                                   m_textureBeginId);
  generateCaption();
}

void GLMenuButton::render() {
  for (const auto &obj : m_windowBackgrounds) {
    m_glFormService->drawQuad(obj, m_windowGLTexture->glTextureId, 1.0F);
  }
  for (const auto &obj : m_windowObjects) {
    m_glFormService->drawQuad(obj, m_windowGLTexture->glTextureId);
  }
  m_glFormService->drawText(m_glCaption);
}

void GLMenuButton::gameWindowSizeChanged(const Size<> &size) {
  m_screenSize = Size<float>(static_cast<float>(size.width()),
                             static_cast<float>(size.height()));
  m_location = {
      (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
      (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
  m_windowCenter = {m_location.x() + (m_size.width() / 2.0F),
                    m_location.y() + (m_size.height() / 2.0F)};
  m_glFormService->gameWindowSizeChanged(size);
}

void GLMenuButton::setTextureBeginId(int value) {
    m_textureBeginId = value;
}

void GLMenuButton::setHasFocus(bool value) {
    m_hasFocus = value;
}

void GLMenuButton::generateQuad(std::vector<GLObject> &objects,
                                Point<float> location, Size<float> size,
                                const Texture *texture, int textureId,
                                GLuint textureGLId) {
  m_glFormService->generateQuad(
      objects, {m_initialLocation.x() + location.x(), m_initialLocation.y() + location.y()},
      size, texture, textureId, textureGLId);
}

void GLMenuButton::generateBoxQuad(std::vector<GLObject> &objects,
                                   Point<float> location, Size<float> size,
                                   const Texture *texture, int textureBeginId,
                                   GLuint textureGLId, float blockSize) {
  m_glFormService->generateBoxQuad(
      objects, {m_initialLocation.x() + location.x(), m_initialLocation.y() + location.y()},
      size, texture, textureBeginId, textureGLId, blockSize);
}

void GLMenuButton::generateCaption() {
  auto titleSize = m_textService->getTextSize(m_glCaption.text, 0.6F);
  m_glCaption.position = {m_location.x() + m_initialLocation.x() +
                              (m_size.width() / 2.0F) -
                              (titleSize.width() / 2.0F),
                          m_location.y() + m_initialLocation.y() +
                              (m_size.height() / 2.0F) +
                              (titleSize.height() / 2.0F)};
  if (m_hasFocus) {
      m_glCaption.color = GLColor::Green;
  } else {
      m_glCaption.color = GLColor::White;
  }
}

} // namespace thewarrior::ui::components
