#include <chrono>
#include <format>
#include <optional>
#include <string>
#include "glGameStateListEntry.hpp"
#include "gameStateMetadata.hpp"
#include "glComponentBase.hpp"
#include "glLabel.hpp"
#include "mainMenuCommons.hpp"

using namespace thewarrior::models;
using thewarrior::storage::GameStateMetadata;
using std::chrono::system_clock, std::chrono::current_zone, std::chrono::zoned_time;

namespace thewarrior::ui::components {

GLGameStateListEntry::GLGameStateListEntry(GLContext &glContext,
                                           const GameStateMetadata &gameEntryMetadata)
: GLComponentBase(glContext),
m_metadata(gameEntryMetadata),
m_playerNameLabel(glContext, gameEntryMetadata.playerName, Point<float>(0.0F, 0.0F), GLColor::Brown, 0.5F, TextAlignment::Left),
m_levelLabel(glContext, std::to_string(gameEntryMetadata.level), Point<float>(0.0F, 0.0F), GLColor::Brown, 0.5F),
m_dateSavedLabel(glContext, "", Point<float>(0.0F, 0.0F), GLColor::Brown, 0.5F, TextAlignment::Right),
m_selected(false) {}

void GLGameStateListEntry::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
    m_playerNameLabel.initialize(info);
    m_levelLabel.initialize(info);
    m_dateSavedLabel.initialize(info);
    const auto timePoint = system_clock::from_time_t(m_metadata.timestamp);
    const auto &localZone = current_zone();
    zoned_time localZonedTime {localZone, timePoint};
    m_dateSavedLabel.setCaption(std::format("{:%Y-%m-%d %H:%M}", localZonedTime));
}

void GLGameStateListEntry::onGenerateGLElements() {
    m_playerNameLabel.setLocation({m_location.x() - 360.0F, m_location.y()});
    m_playerNameLabel.generateGLElements();
    m_levelLabel.setLocation({m_location.x() + 30.0F, m_location.y()});
    m_levelLabel.generateGLElements();
    m_dateSavedLabel.setLocation({m_location.x() + 370.0F, m_location.y()});
    m_dateSavedLabel.generateGLElements();
    if (m_selected) {
        generateGLObject(TextureMainMenuItemSelected,
                         std::nullopt,
                         HorizontalAlignment::Center,
                         VerticalAlignment::Center,
                         Point<int>(0, static_cast<int>(m_location.y())));
    } else {
        generateGLObject(TextureMainMenuItemSeparator,
                         std::nullopt,
                         HorizontalAlignment::Center,
                         VerticalAlignment::Center,
                         Point<int>(0, static_cast<int>(m_location.y()) + 35));
    }
}

void GLGameStateListEntry::onRender() {
    if (m_selected) {
        drawGLObject(TextureMainMenuItemSelected);
    } else {
        drawGLObject(TextureMainMenuItemSeparator);
    }
    m_playerNameLabel.render();
    m_levelLabel.render();
    m_dateSavedLabel.render();
}

void GLGameStateListEntry::onGameWindowSizeChanged(const thewarrior::models::Size<> &size) {
    m_playerNameLabel.gameWindowSizeChanged(size);
    m_levelLabel.gameWindowSizeChanged(size);
    m_dateSavedLabel.gameWindowSizeChanged(size);
}

const thewarrior::storage::GameStateMetadata &GLGameStateListEntry::getMetadata() {
    return m_metadata;
}

bool GLGameStateListEntry::isSelected() const {
    return m_selected;
}

void GLGameStateListEntry::setSelected(bool selected) {
    m_selected = selected;
}

}  // namespace thewarrior::ui::components
