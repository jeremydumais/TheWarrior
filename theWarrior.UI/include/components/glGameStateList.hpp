#pragma once

#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <vector>
#include "../../theWarrior.Storage/include/gameStateMetadata.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glGameStateListEntry.hpp"
#include "glLabel.hpp"
#include "point.hpp"
#include "size.hpp"
#include <boost/signals2.hpp>

namespace thewarrior::ui::components {

class GLGameStateList : public GLComponentBase {
 public:
    explicit GLGameStateList(GLContext &glContext,
                             thewarrior::models::Point<float> location);
    ~GLGameStateList() override = default;
    void reset();
    bool loadTextures();
    void setGameStates(const std::vector<storage::GameStateMetadata> &gameStateList);
    const storage::GameStateMetadata &getSelectedGameState() const;
    boost::signals2::signal<void()> onCancelButtonPressed;
    boost::signals2::signal<void()> onOKButtonPressed;

 protected:
    GLLabel m_playerHeaderLabel;
    GLLabel m_levelHeaderLabel;
    GLLabel m_dateSavedHeaderLabel;
    std::vector<std::unique_ptr<GLGameStateListEntry>> m_gameEntries;
    size_t m_cursorPosition;
    void onInitialize(const GLComponentBaseInfo &info) override;
    void onGenerateGLElements() override;
    void onRender() override;
    void onGameWindowSizeChanged(const thewarrior::models::Size<> &size) override;
    void onButtonUpPressed() override;
    void onButtonDownPressed() override;
    void onButtonCancelPressed() override;
    void onButtonActionPressed() override;

 private:
    void generateScrollBar();
    size_t getFirstEntryToDisplay() const;
};

}  // namespace thewarrior::ui::components
