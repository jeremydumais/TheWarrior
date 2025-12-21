#include <fmt/format.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cctype>
#include <memory>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include "glGameStateList.hpp"
#include "../../theWarrior.Storage/include/gameStateMetadata.hpp"
#include "glComponentBase.hpp"
#include "glContext.hpp"
#include "glGameStateListEntry.hpp"
#include "mainMenuCommons.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLGameStateList::GLGameStateList(GLContext &glContext, Point<float> location)
: GLComponentBase(glContext, location, Size<float>(700.0F, 700.0F)),
m_focusPosition(0, 0),
m_playerHeaderLabel(glContext, "Player", Point<float>(-280.0F, -200.0F), GLColor::Brown),
m_levelHeaderLabel(glContext, "Level", Point<float>(0.0F, -200.0F), GLColor::Brown),
m_dateSavedHeaderLabel(glContext, "Date Saved", Point<float>(250.0F, -200.0F), GLColor::Brown),
m_gameEntries(std::vector<std::unique_ptr<GLGameStateListEntry>>()) {}

void GLGameStateList::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
    m_playerHeaderLabel.initialize(info);
    m_levelHeaderLabel.initialize(info);
    m_dateSavedHeaderLabel.initialize(info);
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {1, "Jed", t, 1, "test.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {2, "Ragnar", t, 15, "testRag.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {3, "Sir Garrett", t, 7, "testGarrett.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {4, "Lady Elyra", t, 7, "testElyra.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {5, "Mame Ragnilieah", t, 7, "testMame.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {6, "This is a test", t, 7, "test1.bkp"}));
    m_gameEntries.push_back(std::make_unique<GLGameStateListEntry>(m_glContext, storage::GameStateMetadata {7, "Also a test", t, 7, "test2.bkp"}));
    std::for_each(m_gameEntries.begin(), m_gameEntries.end(), [&info](auto &entry) {
            entry->initialize(info); });
    if (!loadTextures()) {
        throw std::runtime_error(getLastError());
    }
    generateGLElements();
}

bool GLGameStateList::loadTextures() {
    TextureInfo textureMainMenuItemSeparator {
        .name = TextureMainMenuItemSeparator,
        .filename = "mainmenu_itemseparator.png",
        .width = 777,
        .height = 13,
        .tileWidth = 777,
        .tileHeight = 13
    };
    if (!loadTexture(textureMainMenuItemSeparator)) {
        return false;
    }
    TextureInfo textureMainMenuItemSelected {
        .name = TextureMainMenuItemSelected,
        .filename = "mainmenu_itemselected.png",
        .width = 800,
        .height = 90,
        .tileWidth = 800,
        .tileHeight = 90
    };
    return loadTexture(textureMainMenuItemSelected);
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
    std::for_each(m_gameEntries.begin(), m_gameEntries.end(), [&yPos](auto &entry) {
            entry->setLocation({entry->getLocation().x(), yPos});
            entry->generateGLElements();
            yPos += 70.0F;
    });
    m_gameEntries.at(1)->setSelected(true);
}

void GLGameStateList::onRender() {
    m_playerHeaderLabel.render();
    m_levelHeaderLabel.render();
    m_dateSavedHeaderLabel.render();
    drawGLObject(TextureMainMenuItemSeparator);
    std::for_each(m_gameEntries.begin(), m_gameEntries.end(), [](auto &entry) {
            entry->render(); });
}

void GLGameStateList::onGameWindowSizeChanged(const Size<> &size) {
    m_playerHeaderLabel.gameWindowSizeChanged(size);
    m_levelHeaderLabel.gameWindowSizeChanged(size);
    m_dateSavedHeaderLabel.gameWindowSizeChanged(size);
    std::for_each(m_gameEntries.begin(), m_gameEntries.end(), [&size](auto &entry) {
            entry->gameWindowSizeChanged(size); });
}

void GLGameStateList::buttonUpPress() {
}

void GLGameStateList::buttonDownPress() {
}

void GLGameStateList::buttonLeftPress() {
}

void GLGameStateList::buttonRightPress() {
}

void GLGameStateList::buttonCancelPress() {
}

void GLGameStateList::buttonActionPress() {
}

}  // namespace thewarrior::ui::components
