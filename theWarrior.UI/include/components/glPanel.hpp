#pragma once

#include <string>
#include <variant>
#include <vector>
#include "glComponentBase.hpp"
#include "glObjectService.hpp"

namespace thewarrior::ui::components {

enum class GLPanelSkinType {
    NineSlice,
    SingleTexture
};

struct NineSliceSkin {
    int firstTextureIndex = 29;
    int backgroundTextureIndex = 37;
};

struct SingleTextureSkin {
    std::string textureName;
};

struct GLPanelCreationInfo {
    thewarrior::models::Point<float> location = thewarrior::models::Point<float>(0.0F, 0.0F);
    HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
    VerticalAlignment verticalAlignment = VerticalAlignment::Center;
    std::variant<NineSliceSkin, SingleTextureSkin> skin = NineSliceSkin {};
};

class GLPanel : public GLComponentBase {
 public:
    GLPanel(GLContext &glContext,
            thewarrior::models::Size<float> size,
            GLPanelCreationInfo info = GLPanelCreationInfo {});
    ~GLPanel() override;
    std::variant<NineSliceSkin, SingleTextureSkin> getSkin() const;
    void setSkin(const std::variant<NineSliceSkin, SingleTextureSkin> &skin);

 protected:
    std::vector<GLObject> m_windowObjects;
    std::vector<GLObject> m_windowBackgrounds;
    std::variant<NineSliceSkin, SingleTextureSkin> m_skin;
    void onInitialize(const GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<int> &) override;
};

}  // namespace thewarrior::ui::components
