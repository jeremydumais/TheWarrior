#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <string>
#include "menuScreenBase.hpp"
#include "glContext.hpp"
#include "mainMenuCommons.hpp"
#include "glComponentBase.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::screens {

MenuScreenBase::MenuScreenBase(GLContext &glContext)
: components::GLComponentBase(glContext) {}

void MenuScreenBase::onGenerateGLElements() {
    generateGLObject(TextureBackground, Size<int>(static_cast<int>(m_screenSize.width()),
                                                          static_cast<int>(m_screenSize.height())));
}

void MenuScreenBase::onRender() {
    drawGLObject(TextureBackground);
}

}  // namespace thewarrior::ui::screens
