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
    const thewarrior::storage::GameStateMetadata &getMetadata();
    bool isSelected() const;
    void setSelected(bool selected);
 private:
    thewarrior::storage::GameStateMetadata m_metadata;
    GLLabel m_playerNameLabel;
    GLLabel m_levelLabel;
    GLLabel m_dateSavedLabel;
    bool m_selected;
    void onInitialize(const GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
};

}  // namespace thewarrior::ui::components
