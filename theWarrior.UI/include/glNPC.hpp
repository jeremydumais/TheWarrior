#pragma  once

#include "direction.hpp"
#include "glObjectService.hpp"
#include "npc.hpp"
#include "texture.hpp"
#include "tileSize.hpp"

namespace thewarrior::ui {

class GLNPC : public thewarrior::models::NPC {
 public:
     explicit GLNPC(const thewarrior::models::NPC &npc,
                    const thewarrior::models::Texture &texture);
     ~GLNPC() override = default;
    void initialize(const TileSize &tileSize);
    void draw() const;
    void generateGLObject();
    void unloadGLObject();
    void setGLTextureId(unsigned int textureId);
    void setGLObjectPosition() const;
    void onGameWindowTileSizeChanged(const TileSize &tileSize);

 private:
    const thewarrior::models::Texture &m_texture;
    GLObject glObject;
    unsigned int glTextureId;
    TileSize m_tileSize;
    thewarrior::models::Direction m_direction;
};

}  // namespace thewarrior::ui
