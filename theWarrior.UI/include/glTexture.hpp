#pragma once

#include "texture.hpp"

namespace thewarrior::ui {

struct GLTexture
{
    Texture texture;
    unsigned int glTextureId;
};

} // namespace thewarrior::ui
