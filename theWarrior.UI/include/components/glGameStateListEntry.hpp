#pragma once

#include "../../theWarrior.Storage/include/gameStateMetadata.hpp"
#include "glComponentBase.hpp"
#include "glLabel.hpp"

namespace thewarrior::ui::components {

class GLGameStateListEntry : public components::GLComponentBase  {
 public:
    explicit GLGameStateListEntry(GLContext &glContext,
                                  const thewarrior::storage::GameStateMetadata &gameEntryMetadata);
    ~GLGameStateListEntry() override = default;
    void initialize(const GLComponentBaseInfo &info);
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
 private:
    thewarrior::storage::GameStateMetadata m_metadata;
    GLLabel m_playerNameLabel;
    GLLabel m_levelLabel;
    GLLabel m_dateSavedLabel;
};

}  // namespace thewarrior::ui::components
