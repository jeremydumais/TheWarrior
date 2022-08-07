#pragma once

#include "texture.hpp"

namespace thewarrior::ui {

struct GLTexture
{
    thewarrior::models::Texture texture;
    unsigned int glTextureId;
};

} // namespace thewarrior::ui
