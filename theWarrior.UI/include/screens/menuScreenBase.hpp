#pragma once

#include <SDL2/SDL_mixer.h>
#include <map>
#include <memory>
#include <string>
#include "components/glComponentBase.hpp"
#include "glObjectService.hpp"
#include "size.hpp"
#include "texture.hpp"

namespace thewarrior::ui::screens {

class MenuScreenBase : public components::GLComponentBase {
 public:
    MenuScreenBase(std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &textures,
                    std::map<std::string, unsigned int> &texturesGL);
    ~MenuScreenBase() override;
    void initializeBase(const components::GLComponentBaseInfo &info);
    const std::string &getLastError() const;
    void onGenerateGLElements() override;
    void onRender() override;

 protected:
    std::string m_lastError = "";
    std::map<std::string, std::shared_ptr<thewarrior::models::Texture>> &m_textures;
    std::map<std::string, unsigned int> &m_texturesGL;
    std::map<std::string, GLObject> m_namedObjects = {};
    static void freeGLObjects(std::map<std::string, GLObject> &objects);
    thewarrior::models::Size<float> getGLSizeFromPx(thewarrior::models::Size<int> value) const;
};

}  // namespace thewarrior::ui::screens
