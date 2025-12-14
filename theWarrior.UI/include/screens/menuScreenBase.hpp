#pragma once

#include <SDL2/SDL_mixer.h>
#include "components/glComponentBase.hpp"

namespace thewarrior::ui::screens {

class MenuScreenBase : public components::GLComponentBase {
 public:
    MenuScreenBase(GLContext &glContext);
    void initializeBase(const components::GLComponentBaseInfo &info);
    void onGenerateGLElements() override;
    void onRender() override;
};

}  // namespace thewarrior::ui::screens
