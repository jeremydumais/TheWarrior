#include <fmt/format.h>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include "glGameStateList.hpp"
#include "../../theWarrior.Storage/include/gameStateMetadata.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glGameStateListEntry.hpp"
#include "glLabel.hpp"
#include "mainMenuCommons.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;
using thewarrior::storage::GameStateMetadata;

namespace thewarrior::ui::components {

constexpr size_t MaxVisibleEntries = 6;

GLGameStateList::GLGameStateList(GLContext &glContext,
                                 Point<float> location)
: GLComponentBase(glContext, location, Size<float>(700.0F, 700.0F)),
m_playerHeaderLabel(glContext, "Player", Point<float>(-360.0F, -200.0F), GLColor::Brown, 0.6F, TextAlignment::Left),
m_levelHeaderLabel(glContext, "Level", Point<float>(30.0F, -200.0F), GLColor::Brown),
m_dateSavedHeaderLabel(glContext, "Date Saved", Point<float>(250.0F, -200.0F), GLColor::Brown),
m_gameEntries(std::vector<std::unique_ptr<GLGameStateListEntry>>()),
m_cursorPosition(0) {}

void GLGameStateList::initialize(const GLComponentBaseInfo &info,
                                 const std::vector<GameStateMetadata> &gameStateList) {
    GLComponentBase::initialize(info);
    m_playerHeaderLabel.initialize(info);
    m_levelHeaderLabel.initialize(info);
    m_dateSavedHeaderLabel.initialize(info);
    std::for_each(gameStateList.begin(), gameStateList.end(), [this, &info](const auto &entry) {
        auto glEntry = std::make_unique<GLGameStateListEntry>(m_glContext, entry);
        glEntry->initialize(info);
        m_gameEntries.push_back(std::move(glEntry));
        });
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
    generateGLElements();
}

void GLGameStateList::reset() {
    m_cursorPosition = 0;
}

bool GLGameStateList::loadTextures() {
    return loadTexture(TextureMainMenuItemSeparator, "mainmenu_itemseparator.png", 777, 13) &&
        loadTexture(TextureMainMenuItemSelected, "mainmenu_itemselected.png", 800, 90) &&
        loadTexture(TextureMainMenuScrollBar, "mainmenu_scrollbar.png", 102, 482) &&
        loadTexture(TextureMainMenuScrollBarCursor, "mainmenu_scrollbar_cursor.png", 26, 47);
}

void GLGameStateList::onGenerateGLElements() {
    m_playerHeaderLabel.generateGLElements();
    m_levelHeaderLabel.generateGLElements();
    m_dateSavedHeaderLabel.generateGLElements();
    generateGLObject(TextureMainMenuItemSeparator,
                     std::nullopt,
                     HorizontalAlignment::Center,
                     VerticalAlignment::Center,
                     Point<int>(0, -170));
    float yPos = m_location.y() -130.0F;
    size_t entryIndex = getFirstEntryToDisplay();
    for (auto &entry : m_gameEntries | std::views::drop(entryIndex) | std::views::take(MaxVisibleEntries)) {
        entry->setLocation({entry->getLocation().x(), yPos});
        entry->setSelected(entryIndex == m_cursorPosition);
        entry->generateGLElements();
        yPos += 70.0F;
        entryIndex++;
    }
    if (m_gameEntries.size() > MaxVisibleEntries) {
        generateScrollBar();
    }
}

void GLGameStateList::onRender() {
    m_playerHeaderLabel.render();
    m_levelHeaderLabel.render();
    m_dateSavedHeaderLabel.render();
    drawGLObject(TextureMainMenuItemSeparator);
    for (auto &entry : m_gameEntries | std::views::drop(getFirstEntryToDisplay()) | std::views::take(MaxVisibleEntries)) {
        entry->render();
    }
    if (m_gameEntries.size() > MaxVisibleEntries) {
        drawGLObject(TextureMainMenuScrollBar);
        drawGLObject(TextureMainMenuScrollBarCursor);
    }
}

void GLGameStateList::onGameWindowSizeChanged(const Size<> &size) {
    m_playerHeaderLabel.gameWindowSizeChanged(size);
    m_levelHeaderLabel.gameWindowSizeChanged(size);
    m_dateSavedHeaderLabel.gameWindowSizeChanged(size);
    std::for_each(m_gameEntries.begin(), m_gameEntries.end(), [&size](auto &entry) {
            entry->gameWindowSizeChanged(size); });
}

void GLGameStateList::onButtonUpPressed() {
    if (m_cursorPosition > 0) {
        m_cursorPosition--;
        playMoveSound();
    }
}

void GLGameStateList::onButtonDownPressed() {
    if (m_cursorPosition < m_gameEntries.size() - 1) {
        m_cursorPosition++;
        playMoveSound();
    }
}

void GLGameStateList::onButtonCancelPressed() {
    onCancelButtonPressed();
}

void GLGameStateList::onButtonActionPressed() {
    onOKButtonPressed();
}

void GLGameStateList::generateScrollBar() {
    generateGLObject(TextureMainMenuScrollBar,
                     Size<int>(87, 410),
                     HorizontalAlignment::Center,
                     VerticalAlignment::Center,
                     Point<int>(412, 45));
    // Calculate the scrollbar cursor height
    float trackHeightPx   = 296.0F;
    const float VERTICALOFFSET = 51.0F;
    float totalEntries = static_cast<float>(m_gameEntries.size());
    float firstEntryToDisplay = static_cast<float>(getFirstEntryToDisplay());
    // if everything fits, thumb is full height
    float thumbHeightPx = trackHeightPx;

    if (totalEntries > MaxVisibleEntries) {
        float ratio = MaxVisibleEntries / totalEntries;  // 6/7, 6/16, etc.
        thumbHeightPx = trackHeightPx * ratio;
    }
    thumbHeightPx = std::clamp(thumbHeightPx, 24.0f, trackHeightPx);
    float scrollOffsetPerItem = (trackHeightPx - thumbHeightPx) / (static_cast<float>(totalEntries) - MaxVisibleEntries);
    if (scrollOffsetPerItem < 0.0F) {
        scrollOffsetPerItem = 0.0F;
    }
    float thumbPosition = -((trackHeightPx / 2.0F) - (thumbHeightPx / 2.0F));
    generateGLObject(TextureMainMenuScrollBarCursor,
                     Size<int>(16, static_cast<int>(thumbHeightPx)),
                     HorizontalAlignment::Center,
                     VerticalAlignment::Center,
                     Point<int>(410, static_cast<int>(thumbPosition + VERTICALOFFSET + (scrollOffsetPerItem * firstEntryToDisplay))));
}

size_t GLGameStateList::getFirstEntryToDisplay() const {
    constexpr auto MiddleIndex = MaxVisibleEntries / 2;
    // List will start to scrolling from the middle index
    if (m_cursorPosition < MiddleIndex || m_gameEntries.size() <= MaxVisibleEntries) {
        return 0;
    }
    if (m_cursorPosition + MiddleIndex > m_gameEntries.size()) {
        return m_gameEntries.size() - MaxVisibleEntries;
    } else {
        return m_cursorPosition - MiddleIndex;
    }
}

}  // namespace thewarrior::ui::components
