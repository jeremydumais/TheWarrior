#pragma once

#include <SDL2/SDL_mixer.h>
#include "components/glComponentBase.hpp"

namespace thewarrior::ui::screens {

class MenuScreenBase : public components::GLComponentBase {
 public:
    explicit MenuScreenBase(GLContext &glContext);
    void onGenerateGLElements() override;
    void onRender() override;
};

}  // namespace thewarrior::ui::screens
