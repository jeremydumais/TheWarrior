#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <QtWidgets>
#include <fmt/format.h>
#include <stb_image.h>
#include <GL/gl.h>
#include <qnamespace.h>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "glColor.hpp"
#include "mapOpenGLWidget.hpp"
#include "mapView.hpp"
#include "monsterZone.hpp"
#include "pickerToolSelection.hpp"
#include "point.hpp"
#include "selectionMode.hpp"

using namespace thewarrior::models;
using thewarrior::ui::getVec3FromRGBString;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
: QOpenGLWidget(parent),
m_resourcesPath(""),
m_texturesGLMap(std::map<std::string, unsigned int>()),
m_texturesObjMap(std::map<std::string, const Texture &>()),
m_currentMap(nullptr),
m_pasteResult({}),
m_pasteDragInProgress(false),
m_pasteDragStartPosition(QPoint(0, 0)),
m_pasteDragEndPosition(QPoint(0, 0)) {
    connect(&m_repaintTimer, SIGNAL(timeout()), this, SLOT(updateFrameState()));
    setMouseTracking(true);
    setAutoFillBackground(false);
    QSurfaceFormat fmt;
    fmt.setSwapInterval(1);  // Enable VSync
    setFormat(fmt);
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_currentMap = map;
    m_selection.selectedTileIndices = {};
    m_pasteResult = {};
    m_pasteResultIndices = {};
    m_pasteDragInProgress = false;
    m_pasteDragStartPosition = QPoint(0, 0);
    m_pasteDragEndPosition = QPoint(0, 0);
    m_pasteSelectionStartPosition = QPoint(0, 0);
    m_pasteSelectionEndPosition = QPoint(0, 0);
}

void MapOpenGLWidget::setGridEnabled(bool enabled) {
    m_config.gridEnabled = enabled;
}

void MapOpenGLWidget::setShowNPCsEnabled(bool enabled) {
    m_config.showNPCEnabled = enabled;
}

void MapOpenGLWidget::setZoom(int zoomPercentage) {
    m_config.zoomPercentage = zoomPercentage;
    recalculateTileSize();
}

void MapOpenGLWidget::setZoomLimit(int min, int max) {
    m_config.zoomMin = min;
    m_config.zoomMax = max;
}

QSize MapOpenGLWidget::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize MapOpenGLWidget::sizeHint() const {
    return QSize(400, 400);
}

void MapOpenGLWidget::initializeGL() {
    reloadTextures();

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClearDepth(1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Configure for 60FPS
    startAutoUpdate();
}

void MapOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0F, 0.0F, -10.0F);
    draw();
}

void MapOpenGLWidget::resizeGL(int width, int height) {
    m_metrics.width = width;
    m_metrics.height = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float HALFGLORTHOSIZE = m_metrics.glOrthoSize / 2.0F;
#ifdef QT_OPENGL_ES_1
    glOrthof(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#else
    glOrtho(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
    recalculateTileSize();
}

void MapOpenGLWidget::updateFrameState() {
    const auto &monsterZones = m_currentMap->getMonsterZones();
    std::vector<std::string> zoneColors = {};
    std::transform(monsterZones.begin(),
                   monsterZones.end(),
                   std::back_inserter(zoneColors),
                   [](const MonsterZone &zone) -> std::string { return zone.getColor().getValue(); });
    m_frame.zoneColors = zoneColors;

    // Load the npcs in an unordered_map to be able to find them by spawn positition O(1)
    // spawn position will be converted to TileIndex
    const auto &npcs = m_currentMap->getNPCs();
    std::unordered_map<int, const NPC *> npcsBySpawnLocation {};
    std::transform(npcs.begin(),
                   npcs.end(),
                   std::inserter(npcsBySpawnLocation, npcsBySpawnLocation.end()),
                   [this](const NPC &npc) -> std::pair<int, const NPC *> {
                        const auto &spawnPosition = npc.getSpawnPosition();
                        const auto spawnPositionConverted = Point<int>(static_cast<int>(spawnPosition.x()),
                                                                       static_cast<int>(spawnPosition.y()));
                        const auto tileIndex = m_currentMap->getTileIndexFromCoord(spawnPositionConverted);
                        return std::make_pair(tileIndex, &npc);
                   });
    m_frame.npcsBySpawnLocation = npcsBySpawnLocation;
    this->update();
}

const std::string& MapOpenGLWidget::getResourcesPath() const {
    return this->m_resourcesPath;
}

void MapOpenGLWidget::setResourcesPath(const std::string &path) {
    this->m_resourcesPath = path;
}

SelectionMode MapOpenGLWidget::getSelectionMode() const {
    return m_selection.currentMode;
}

void MapOpenGLWidget::setSelectionMode(SelectionMode mode) {
    m_selection.previousMode = m_selection.currentMode;
    m_selection.currentMode = mode;
}

void MapOpenGLWidget::restorePreviousSelectionMode() {
    m_selection.currentMode = m_selection.previousMode.value_or(SelectionMode::Select);
    m_selection.previousMode.reset();
}

void MapOpenGLWidget::setMapView(MapView view) {
    m_config.mapView = view;
}

void MapOpenGLWidget::setSelectedNPC(const std::string &npcId) {
    m_selection.selectedNPCId = npcId;
}

void MapOpenGLWidget::clearSelectedNPC() {
    m_selection.selectedNPCId.clear();
}

unsigned int MapOpenGLWidget::getMapWidth() const {
    return this->m_currentMap->getWidth();
}

unsigned int MapOpenGLWidget::getMapHeight() const {
    return this->m_currentMap->getHeight();
}

void MapOpenGLWidget::reloadTextures() {
    // Clear existing textures in graphics memory
    for (auto &glTexture : m_texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_texturesGLMap.clear();
    m_texturesObjMap.clear();
    // Load texture in graphics memory
    for (const auto &texture : m_currentMap->getTextures()) {
        const auto &textureName { texture.getName() };
        glGenTextures(1, &m_texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[textureName]);
        m_texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        std::string texFileName { texture.getFilename() };
        std::string fullResourcePath = fmt::format("{0}/textures/{1}", m_resourcesPath, texFileName);
        unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (imageBytes) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
        } else {
            throw std::runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(imageBytes);
    }
}

void MapOpenGLWidget::startAutoUpdate() {
    m_repaintTimer.start(100);
}

void MapOpenGLWidget::stopAutoUpdate() {
    m_repaintTimer.stop();
}

void MapOpenGLWidget::resetMapMovePosition() {
    m_camera.translationX = 0.0F;
    m_camera.translationY = 0.0F;
    m_camera.dragAndDropX = 0.0F;
    m_camera.dragAndDropY = 0.0F;
    m_camera.translationXGL = 0.0F;
    m_camera.translationYGL = 0.0F;
}

void MapOpenGLWidget::pasteClipboard(const std::vector<thewarrior::models::MapTile> &tiles,
                                     const std::set<int> &clipboardSelectedTileIndices) {
    m_pasteResult = tiles;
    m_pasteResultIndices = clipboardSelectedTileIndices;
    m_pasteDragStartPosition = QPoint(0, 0);
    m_pasteDragEndPosition = QPoint(0, 0);
    calculatePasteSelectionZone(m_pasteDragEndPosition, true);
}

void MapOpenGLWidget::wheelEvent(QWheelEvent *event) {
    m_config.zoomPercentage += event->angleDelta().y() / 40;
    if (m_config.zoomPercentage < m_config.zoomMin) {
        m_config.zoomPercentage = m_config.zoomMin;
    } else if (m_config.zoomPercentage > m_config.zoomMax) {
        m_config.zoomPercentage = m_config.zoomMax;
    }
    emit onZoomChanged(m_config.zoomPercentage);
    recalculateTileSize();
}

void MapOpenGLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit onNPCSpawnPositionPickerToolCanceled();
    } else {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    this->setFocus();
    bool altPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::AltModifier);
    if (altPressed) {
        m_selection.preMapDragMode = m_selection.currentMode;
        m_selection.currentMode = SelectionMode::MoveMap;
    } else if (!m_input.mousePressed &&
         m_selection.currentMode == SelectionMode::MoveMap) {
        m_camera.dragAndDropX = 0.0f;
        m_camera.dragAndDropY = 0.0f;
    } else if (!m_input.mousePressed && m_selection.currentMode == SelectionMode::Paste) {
        if (isCursorInPasteSelectionZone(event->pos())) {
            m_pasteDragStartPosition = QPoint(event->pos());
            m_pasteDragInProgress = true;
        }
    }
    m_input.lastCursorPosition = event->pos();
    m_input.currentCursorPosition = event->pos();
    m_input.mousePressed = true;
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    m_input.mousePressed = false;
    m_camera.translationX += m_camera.dragAndDropX;
    m_camera.dragAndDropX = 0;
    m_camera.translationY += m_camera.dragAndDropY;
    m_camera.dragAndDropY = 0;
    m_camera.translationXGL = m_camera.translationX * m_metrics.translationXToPixel;
    m_camera.translationYGL = m_camera.translationY * m_metrics.translationYToPixel;
    emit onMapMoved(m_camera.translationX, m_camera.translationY);
    if (isMultiTileSelectionMode()) {
        if (!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_selection.selectedTileIndices.clear();
        }
        // Calculate the list of index selected
        QPoint startCoord;
        QPoint endCoord;
        // We are always managing selection from top left to bottom right
        if (m_input.currentCursorPosition.x() < m_input.lastCursorPosition.x()) {
            startCoord = m_input.currentCursorPosition;
            endCoord = m_input.lastCursorPosition;
        } else {
            startCoord = m_input.lastCursorPosition;
            endCoord = m_input.currentCursorPosition;
        }
        if (endCoord.y() < startCoord.y()) {
            int tempY = startCoord.y();
            startCoord.setY(endCoord.y());
            endCoord.setY(tempY);
        }
        if (startCoord.x() < 0) {
            startCoord.setX(0);
        }
        if (startCoord.y() < 0) {
            startCoord.setY(0);
        }
        //----------------------------------------------------------------
        QPoint calculatedCoord { startCoord };
        QPoint endCoordToTileBorder(endCoord.x() + (static_cast<int>(m_metrics.onScreenTileSizePx) - (endCoord.x() % static_cast<int>(m_metrics.onScreenTileSizePx))),
                endCoord.y() + (static_cast<int>(m_metrics.onScreenTileSizePx) - (endCoord.y() % static_cast<int>(m_metrics.onScreenTileSizePx))));
        while (calculatedCoord.x() < endCoordToTileBorder.x() &&
               calculatedCoord.y() < endCoordToTileBorder.y()) {
            QPoint realCoord(calculatedCoord);
            // Ensure that we don't go one tile after the selection
            if (calculatedCoord.x() > endCoord.x()) {
                realCoord.setX(endCoord.x());
            }
            if (calculatedCoord.y() > endCoord.y()) {
                realCoord.setY(endCoord.y());
            }
            auto tileIndex = getTileIndex(realCoord.x(), realCoord.y());
            if (tileIndex != -1) {
                m_selection.selectedTileIndices.insert(tileIndex);
            }
            calculatedCoord.setX(calculatedCoord.x() + static_cast<int>(m_metrics.onScreenTileSizePx));
            if (calculatedCoord.x() >= endCoordToTileBorder.x()) {
                calculatedCoord.setX(startCoord.x());
                calculatedCoord.setY(calculatedCoord.y() + static_cast<int>(m_metrics.onScreenTileSizePx));
            }
        }
        m_selection.selectedTileColor = 100;
        m_selection.selectedTileColorGrowing = true;
        emit onTileClicked(m_selection.selectedTileIndices, event->x(), event->y());
    } else if (m_selection.currentMode == SelectionMode::PickerTool) {
        auto currentTileIndex = getTileIndex(event->pos().x(), event->pos().y());
        if (currentTileIndex != -1) {
            const auto &tile = m_currentMap->getTileForEditing(currentTileIndex);
            emit onPickerToolTileSelected(PickerToolSelection {
                tile.getTextureName(),
                tile.getTextureIndex(),
                tile.getObjectTextureName(),
                tile.getObjectTextureIndex()
            });
        }
    } else if (m_selection.currentMode == SelectionMode::Paste) {
        if (m_pasteDragInProgress) {
            calculatePasteSelectionZone(event->pos());
        } else {
            emit onClipboardPasted();
            m_selection.currentMode = SelectionMode::Select;
            size_t tileIndexInSelection = 0;
            for (auto mapTile : m_currentMap->getTilesForEditing(m_pasteResultIndices)) {
                *mapTile = m_pasteResult.at(tileIndexInSelection);
                tileIndexInSelection++;
            }
            m_pasteResult = {};
            m_pasteResultIndices = {};
        }
        m_pasteDragInProgress = false;
    } else if (m_selection.currentMode == SelectionMode::NPCSpawnPositionPickerTool) {
        auto currentTileIndex = getTileIndex(event->pos().x(), event->pos().y());
        if (currentTileIndex != -1) {
            const auto &tile = m_currentMap->getTileForEditing(currentTileIndex);
            const auto position = m_currentMap->getCoordFromTileIndex(currentTileIndex);
            emit onNPCSpawnPositionPickerToolTileSelected(tile, position);
        }
    }
    if (m_selection.preMapDragMode.has_value()) {
        m_selection.currentMode = m_selection.preMapDragMode.value();
        m_selection.preMapDragMode = {};
    }
}

void MapOpenGLWidget::leaveEvent(QEvent *) {
    setCursor(Qt::ArrowCursor);
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_input.mousePressed &&
            m_selection.currentMode == SelectionMode::MoveMap) {
        m_camera.dragAndDropX = static_cast<float>(event->pos().x() - m_input.lastCursorPosition.x()) / (static_cast<float>(m_metrics.onScreenTileSizePx) * m_metrics.translationXToPixel);
        m_camera.dragAndDropY = static_cast<float>(m_input.lastCursorPosition.y() - event->pos().y()) / (static_cast<float>(m_metrics.onScreenTileSizePx) * m_metrics.translationYToPixel);
        if (m_camera.translationX + m_camera.dragAndDropX > 0) {
            m_camera.dragAndDropX = m_camera.translationX * -1.0F;
        }
        if (m_camera.translationY + m_camera.dragAndDropY < 0) {
            m_camera.dragAndDropY = m_camera.translationY * -1.0F;
        }
    }
    m_input.currentCursorPosition = event->pos();
    updateCursor(event);
    emit onTileMouseMoveEvent(m_input.mousePressed, getTileIndex(event->pos().x(), event->pos().y()));
}

bool MapOpenGLWidget::isMultiTileSelectionMode() const {
    return m_selection.currentMode == SelectionMode::Select;
}

void MapOpenGLWidget::recalculateTileSize() {
    const float NOZOOMSCREENTILESIZE = 40.0F;
    const float ZOOMFLOATVALUE = static_cast<float>(m_config.zoomPercentage) / 100.0F;
    m_metrics.onScreenTileSizePx = (40.0F * ZOOMFLOATVALUE);
    float nbOfTilesForWidth = static_cast<float>(m_metrics.width) / static_cast<float>(NOZOOMSCREENTILESIZE);
    float nbOfTilesForHeight = static_cast<float>(m_metrics.height) / static_cast<float>(NOZOOMSCREENTILESIZE);
    m_metrics.glTileWidth = (static_cast<float>(m_metrics.width) / 10.0F / nbOfTilesForWidth / nbOfTilesForWidth) * ZOOMFLOATVALUE;
    m_metrics.glTileHeight = (static_cast<float>(m_metrics.height) / 10.0F / nbOfTilesForHeight / nbOfTilesForHeight) * ZOOMFLOATVALUE;
    m_metrics.glTileHalfWidth = m_metrics.glTileWidth / 2.0F;
    m_metrics.glTileHalfHeight = m_metrics.glTileHeight / 2.0F;
    m_metrics.translationXToPixel = static_cast<float>(m_metrics.width) / static_cast<float>(m_metrics.onScreenTileSizePx) / m_metrics.glOrthoSize;
    m_metrics.translationYToPixel = static_cast<float>(m_metrics.height) / static_cast<float>(m_metrics.onScreenTileSizePx) / m_metrics.glOrthoSize;
    m_camera.translationX = m_camera.translationXGL / m_metrics.translationXToPixel;
    m_camera.translationY = m_camera.translationYGL / m_metrics.translationYToPixel;
    ResizeGLComponentInfo info {
        .componentWidth = m_metrics.width,
        .componentHeight = m_metrics.height,
        .glTileWidth = m_metrics.glTileWidth,
        .glTileHeight = m_metrics.glTileHeight,
        .translationXToPixel = m_metrics.translationXToPixel,
        .translationYToPixel = m_metrics.translationYToPixel,
        .tileSizeInPx = static_cast<unsigned int>(m_metrics.onScreenTileSizePx)
    };
    emit onRecalculateTileSize(info);
}

void MapOpenGLWidget::updateCursor(QMouseEvent *event) {
    if (m_selection.currentMode == SelectionMode::MoveMap) {
        setCursor(m_input.mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
    } else if (m_selection.currentMode == SelectionMode::Select) {
        setCursor(Qt::ArrowCursor);
    } else if (isMultiTileSelectionMode()) {
        setCursor(Qt::CrossCursor);
    } else if (m_selection.currentMode == SelectionMode::Paste) {
        if (isCursorInPasteSelectionZone(event->pos())) {
            setCursor(Qt::DragMoveCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void MapOpenGLWidget::draw() {
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glEnable(GL_TEXTURE_2D);

    float x { -(m_metrics.glOrthoSize / 2.0F) + m_metrics.glTileHalfWidth };
    float y { (m_metrics.glOrthoSize / 2.0F) - m_metrics.glTileHalfHeight  };
    glTranslatef(x, y, 0.0F);
    glPushMatrix();
    glTranslatef(m_camera.translationX + m_camera.dragAndDropX, m_camera.translationY + m_camera.dragAndDropY, 0.0F);
    int index {0};

    m_frame.selectedNPCGlowAnimation.process();

    if (m_selection.currentMode == SelectionMode::Select || m_selection.currentMode == SelectionMode::Paste) {
        updateSelectedTileColor();
    }
    int yIndexPos = 0;
    int firstHorizontalTileToDisplay = static_cast<int>(std::abs((m_camera.translationX + m_camera.dragAndDropX) * m_metrics.translationXToPixel));
    int lastHorizontalTileToDisplay = firstHorizontalTileToDisplay + static_cast<int>(std::ceil(static_cast<float>(this->width()) / m_metrics.onScreenTileSizePx));
    int firstVerticalTileToDisplay = static_cast<int>(std::abs((m_camera.translationY + m_camera.dragAndDropY) * m_metrics.translationYToPixel));
    int lastVerticalTileToDisplay = firstVerticalTileToDisplay + static_cast<int>(std::ceil(static_cast<float>(this->height()) / m_metrics.onScreenTileSizePx));
    glPushMatrix();
    for (const auto &row : m_currentMap->getTiles()) {
        int xIndexPos = 0;
        for (const auto &tile : row) {
            if (yIndexPos >= firstVerticalTileToDisplay && yIndexPos <= lastVerticalTileToDisplay &&
                xIndexPos >= firstHorizontalTileToDisplay && xIndexPos <= lastHorizontalTileToDisplay) {
                drawTile(tile, index, m_frame);
            }
            x += m_metrics.glTileWidth + m_metrics.tileSpacing;
            glTranslatef(m_metrics.glTileWidth + m_metrics.tileSpacing, 0, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            index++;
            xIndexPos++;
        }
        x += static_cast<float>(row.size()) * -(m_metrics.glTileWidth + m_metrics.tileSpacing);
        y += -(m_metrics.glTileHeight + m_metrics.tileSpacing);
        glTranslatef(static_cast<float>(row.size()) * -(m_metrics.glTileWidth + m_metrics.tileSpacing), -(m_metrics.glTileHeight + m_metrics.tileSpacing), 0.0f);
        yIndexPos++;
    }
    glPopMatrix();
    if (m_selection.currentMode == SelectionMode::Paste || m_selection.preMapDragMode == SelectionMode::Paste) {
        drawPasteResult();
    }
    glPopMatrix();
    glPushMatrix();
    if (m_input.mousePressed && isMultiTileSelectionMode()) {
        drawSelectionZone();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void MapOpenGLWidget::drawTile(const MapTile &tile,
                               int index,
                               const MapRendererContext &ctx) {
    bool hasTexture { false };
    if (m_texturesGLMap.find(tile.getTextureName()) != m_texturesGLMap.end()) {
        hasTexture = true;
        glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.getTextureName()]);
    }

    float transparency = 1.0F;
    switch (m_config.mapView) {
        case MapView::MonsterZones:
            transparency = 0.2F;
            glColor4f(1.0F, 1.0F, 1.0F, 0.2F);
            break;
        case MapView::CanStep:
            transparency = 0.5F;
            glColor4f(1.0F, 1.0F, 1.0F, 0.5F);
            break;
        case MapView::NPCWanderingZones:
            transparency = 0.2F;
            glColor4f(1.0F, 1.0F, 1.0F, 0.2F);
            break;
    }

    if ((m_selection.currentMode == SelectionMode::Select && m_selection.selectedTileIndices.contains(index)) ||
        (m_selection.currentMode == SelectionMode::Paste && m_pasteResultIndices.contains(index))) {
        glColor4ub(m_selection.selectedTileColor, m_selection.selectedTileColor, m_selection.selectedTileColor, static_cast<GLubyte>(transparency * 255.0F));
    } else {
        glColor4f(1.0F, 1.0F, 1.0F, transparency);
    }

    if (hasTexture) {
        drawTileWithTexture(tile.getTextureName(), tile.getTextureIndex());
        // Check if it has an optionnal object
        // TODO: create a method and test for bool hasAndObjectDefined() on the mapTile
        if (tile.getObjectTextureName() != "" && tile.getObjectTextureIndex() != -1) {
            if (tile.getTextureName() != tile.getObjectTextureName()) {
                if (m_texturesGLMap.find(tile.getObjectTextureName()) != m_texturesGLMap.end()) {
                    glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.getObjectTextureName()]);
                    glPushMatrix();
                    drawTileWithTexture(tile.getObjectTextureName(), tile.getObjectTextureIndex());
                    glPopMatrix();
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            } else {
                drawTileWithTexture(tile.getObjectTextureName(), tile.getObjectTextureIndex());
            }
        glBindTexture(GL_TEXTURE_2D, 0);
        }
    } else {
        // not defined tile (no texture)
        glPushMatrix();
        glColor3f(0.5F, 0.5F, 0.5F);
        glBegin(GL_QUADS);
        glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
        glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
        glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
        glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
        glEnd();
        glPopMatrix();
    }
    // Display the NPC in the configured direction
    if (m_config.showNPCEnabled) {
        if (ctx.npcsBySpawnLocation.contains(index)) {
            const auto &npc = ctx.npcsBySpawnLocation.at(index);
            if (m_texturesGLMap.find(npc->getTextureName()) != m_texturesGLMap.end()) {
                glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[npc->getTextureName()]);
                glPushMatrix();
                const int baseTextureIndex = npc->getCurrentFacingTextureIndex();
                if (npc->getId() == m_selection.selectedNPCId) {
                    drawTileSilhouette(npc->getTextureName(), baseTextureIndex, ctx.selectedNPCGlowAnimation.getValue());
                }
                drawTileWithTexture(npc->getTextureName(), baseTextureIndex);
                glPopMatrix();
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }

    // Filter to apply/clear monster zone
    if (m_config.mapView == MapView::MonsterZones) {
        if (tile.getMonsterZoneIndex() != -1) {
            const auto zoneColor = getVec3FromRGBString(ctx.zoneColors[static_cast<size_t>(tile.getMonsterZoneIndex())]);
            glColor4f(zoneColor.r, zoneColor.g, zoneColor.b, 0.4F);
            drawColoredTile();
        }
    }
    //TODO: v0.6 Detect if the tile is part of the NPC Wandering zones
    // Filter to enable/disable can step on tile
    if (m_config.mapView == MapView::CanStep) {
        if (tile.canPlayerSteppedOn()) {
            glColor4f(0.25F, 1.0F, 0.25F, 0.4F);
        } else {
            glColor4f(1.0F, 0.25F, 0.25F, 0.4F);
        }
        drawColoredTile();
    }

    // If we are in block border mode
    if (m_config.mapView == MapView::BlockedBorders) {
        auto triggers { tile.getTriggers() };
        for (const auto &trigger : triggers) {
            if (trigger.getAction() == MapTileTriggerAction::DenyMove) {
                switch (trigger.getEvent()) {
                    case MapTileTriggerEvent::MoveLeftPressed:
                        drawBlockBorderLeft();
                        break;
                    case MapTileTriggerEvent::MoveUpPressed:
                        drawBlockBorderTop();
                        break;
                    case MapTileTriggerEvent::MoveRightPressed:
                        drawBlockBorderRight();
                        break;
                    case MapTileTriggerEvent::MoveDownPressed:
                        drawBlockBorderBottom();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    if (m_config.gridEnabled) {
        drawGrid();
    }
}

void MapOpenGLWidget::drawTileWithTexture(const std::string &textureName, int textureIndex) {
    float indexTile { static_cast<float>(textureIndex) };
    const Texture &currentTexture { m_texturesObjMap.find(textureName)->second };
    const int NBTEXTUREPERLINE { currentTexture.getWidth() / currentTexture.getTileWidth() };
    float lineIndex = floor(indexTile / static_cast<float>(NBTEXTUREPERLINE));
    const float TEXTURETILEWIDTH { currentTexture.getTileWidthGL() };
    const float TEXTURETILEHEIGHT { currentTexture.getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / static_cast<float>(m_metrics.onScreenTileSizePx) };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / static_cast<float>(m_metrics.onScreenTileSizePx) };

    const float TEXTUREX { static_cast<float>((static_cast<int>(indexTile) % NBTEXTUREPERLINE)) };
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawTileSilhouette(const std::string &textureName, int textureIndex, float outlineWidth) {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    const float onePixelX = (2.0f * m_metrics.glTileHalfWidth)  / static_cast<float>(m_metrics.onScreenTileSizePx);
    const float onePixelY = (2.0f * m_metrics.glTileHalfHeight) / static_cast<float>(m_metrics.onScreenTileSizePx);
    const float outlinePx = outlineWidth;
    const float ox = onePixelX * outlinePx;
    const float oy = onePixelY * outlinePx;

    // Force RGB to constant, keep alpha from texture
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);  // alpha from texture

    const GLfloat red[4] = {1.f, 1.f, 1.f, 1.f};
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, red);

    const float offsets[8][2] = {
        {-ox, 0}, {ox, 0}, {0, -oy}, {0, oy},
        {-ox, -oy}, {-ox, oy}, {ox, -oy}, {ox, oy}
    };

    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(offsets[i][0], offsets[i][1], 0.0f);
        drawTileWithTexture(textureName, textureIndex);
        glPopMatrix();
    }

    // Restore defaults
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_DEPTH_TEST);
    glPopAttrib();
}

void MapOpenGLWidget::drawColoredTile() const {
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawSelectionZone() const {
    glm::vec2 startCoord = convertScreenCoordToGlCoord(m_input.lastCursorPosition);
    glm::vec2 endCoord = convertScreenCoordToGlCoord(m_input.currentCursorPosition - m_input.lastCursorPosition);
    glTranslatef(startCoord.x, -startCoord.y, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(endCoord.x -m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(endCoord.x -m_metrics.glTileHalfWidth, -endCoord.y + m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -endCoord.y + m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
}

void MapOpenGLWidget::drawPasteResult() {
    size_t index = 0;
    std::vector<int> tileIndices = {};
    std::transform(m_pasteResultIndices.begin(),
            m_pasteResultIndices.end(),
            std::back_inserter(tileIndices),
            [](int indice) -> int { return indice; });

    for (const auto &tile : m_pasteResult) {
        auto tileIndice = tileIndices[index];
        // From indice compute the row and col
        const auto point = m_currentMap->getCoordFromTileIndex(tileIndice);
        glPushMatrix();
        glTranslatef(static_cast<float>(point.x()) * (m_metrics.glTileWidth + m_metrics.tileSpacing),
                     static_cast<float>(point.y()) * -(m_metrics.glTileHeight + m_metrics.tileSpacing), 0.0f);
        drawTile(tile, tileIndice, m_frame);
        glPopMatrix();
        index++;
    }
    // Draw a selection zone around the pasted elements
    const auto selectionBoxWidth = m_metrics.glTileWidth * (static_cast<float>(m_pasteSelectionEndPosition.x()) - static_cast<float>(m_pasteSelectionStartPosition.x()));
    const auto selectionBoxHeight = m_metrics.glTileHeight * (static_cast<float>(m_pasteSelectionEndPosition.y()) - static_cast<float>(m_pasteSelectionStartPosition.y()));
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(static_cast<float>(m_pasteSelectionStartPosition.x()) * (m_metrics.glTileWidth + m_metrics.tileSpacing),
            static_cast<float>(m_pasteSelectionStartPosition.y()) * -(m_metrics.glTileHeight + m_metrics.tileSpacing), 0.0f);
    glColor4f(1.0F, 1.0F, 1.0F, 0.7F);
    glBegin(GL_LINES);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight - selectionBoxHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_metrics.glTileHalfWidth + selectionBoxWidth, -m_metrics.glTileHalfHeight - selectionBoxHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight - selectionBoxHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_metrics.glTileHalfWidth + selectionBoxWidth, -m_metrics.glTileHalfHeight - selectionBoxHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth + selectionBoxWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth + selectionBoxWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawGrid() const {
    glColor3f(0.0F, 0.0F, 0.0F);
    glBegin(GL_LINES);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
}

void MapOpenGLWidget::drawBlockBorderLeft() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/6.0f), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/6.0f), m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/3.0f), -m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/3.0f), m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight/8.0f, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderTop() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight /6.0f), 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth/8.0f, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight /3.0f), 0);
    glVertex3f(m_metrics.glTileHalfWidth/8.0f, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0f), m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0f), m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight/3.0f), 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderRight() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/6.0f), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/6.0f), m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/3.0f), -m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/3.0f), m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight/8.0f, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight/8.0f, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderBottom() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight /6.0f), 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth/8.0f, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight /3.0f), 0);
    glVertex3f(m_metrics.glTileHalfWidth/8.0f, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0f), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0f), -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight/3.0f), 0);
    glEnd();
    glPopMatrix();
}

int MapOpenGLWidget::getTileIndex(int onScreenX, int onScreenY) {
    if (static_cast<float>(onScreenX) / m_metrics.onScreenTileSizePx > static_cast<float>(m_currentMap->getWidth())) {
        return -1;
    }
    if (static_cast<float>(onScreenY) / m_metrics.onScreenTileSizePx > static_cast<float>(m_currentMap->getHeight())) {
        return -1;
    }
    float x = static_cast<float>(onScreenX) - m_camera.translationX * m_metrics.translationXToPixel * static_cast<float>(m_metrics.onScreenTileSizePx);
    float y = static_cast<float>(onScreenY) + m_camera.translationY * m_metrics.translationYToPixel * static_cast<float>(m_metrics.onScreenTileSizePx);
    int indexX = static_cast<int>(x / m_metrics.onScreenTileSizePx);
    int indexY = static_cast<int>(y / m_metrics.onScreenTileSizePx);
    int tileIndex { indexX + (indexY * static_cast<int>(m_currentMap->getWidth())) };
    if (tileIndex < 0 || (tileIndex > static_cast<int>(m_currentMap->getWidth() * m_currentMap->getHeight()))) {
        return -1;
    }
    return tileIndex;
}

QPoint MapOpenGLWidget::getTileLeftUpperCornerScreenCoord(int tileIndex) const {
    if (!m_currentMap || tileIndex < 0 || tileIndex >= static_cast<int>(m_currentMap->getWidth()) * static_cast<int>(m_currentMap->getHeight())) {
        return QPoint(-1, -1);
    }

    int mapWidth = static_cast<int>(m_currentMap->getWidth());
    int indexX = tileIndex % mapWidth;
    int indexY = tileIndex / mapWidth;

    // Reverse the translation logic from getTileIndex
    float screenX = static_cast<float>(indexX) * m_metrics.onScreenTileSizePx +
                  (m_camera.translationX * m_metrics.translationXToPixel * m_metrics.onScreenTileSizePx);
    float screenY = static_cast<float>(indexY) * m_metrics.onScreenTileSizePx -
                  (m_camera.translationY * m_metrics.translationYToPixel * m_metrics.onScreenTileSizePx);

    return QPoint(static_cast<int>(screenX), static_cast<int>(screenY));
}

QPoint MapOpenGLWidget::getTileRightLowerCornerScreenCoord(int tileIndex) const {
    if (!m_currentMap || tileIndex < 0 || tileIndex >= static_cast<int>(m_currentMap->getWidth()) * static_cast<int>(m_currentMap->getHeight())) {
        return QPoint(-1, -1);
    }

    int mapWidth = static_cast<int>(m_currentMap->getWidth());
    int indexX = tileIndex % mapWidth;
    int indexY = tileIndex / mapWidth;

    // Reverse the translation logic from getTileIndex
    float screenX = static_cast<float>(indexX) * m_metrics.onScreenTileSizePx +
                  ((m_camera.translationX * m_metrics.translationXToPixel * m_metrics.onScreenTileSizePx) + m_metrics.onScreenTileSizePx);
    float screenY = static_cast<float>(indexY) * m_metrics.onScreenTileSizePx -
                  ((m_camera.translationY * m_metrics.translationYToPixel * m_metrics.onScreenTileSizePx) - m_metrics.onScreenTileSizePx);

    return QPoint(static_cast<int>(screenX), static_cast<int>(screenY));
}

glm::vec2 MapOpenGLWidget::convertScreenCoordToGlCoord(QPoint coord) const {
    float x = static_cast<float>(coord.x()) - ((m_camera.translationX / m_metrics.translationXToPixel / static_cast<float>(m_metrics.onScreenTileSizePx)) * static_cast<float>(m_metrics.onScreenTileSizePx));
    float y = static_cast<float>(coord.y()) + ((m_camera.translationY / m_metrics.translationYToPixel / static_cast<float>(m_metrics.onScreenTileSizePx)) * static_cast<float>(m_metrics.onScreenTileSizePx));
    glm::vec2 retVal;
    retVal.x = x / m_metrics.translationXToPixel / static_cast<float>(m_metrics.onScreenTileSizePx);
    retVal.y = y / m_metrics.translationYToPixel / static_cast<float>(m_metrics.onScreenTileSizePx);
    return retVal;
}

void MapOpenGLWidget::updateSelectedTileColor() {
    if (m_selection.selectedTileColorGrowing) {
        m_selection.selectedTileColor += 10;
    } else {
        m_selection.selectedTileColor -= 10;
    }

    if (m_selection.selectedTileColor >= 200) {
        m_selection.selectedTileColorGrowing = false;
        m_selection.selectedTileColor--;
    } else if (m_selection.selectedTileColor <= 100) {
        m_selection.selectedTileColorGrowing = true;
    }
}

void MapOpenGLWidget::calculatePasteSelectionZone(QPoint dragEndPosition, bool initialCalculation) {
    auto startTileIndex = getTileIndex(m_pasteDragStartPosition.x(), m_pasteDragStartPosition.y());
    auto endTileIndex = getTileIndex(dragEndPosition.x(), dragEndPosition.y());
    auto lastMapTileIndex = m_currentMap->getTileIndexFromCoord(Point<>(static_cast<int>(m_currentMap->getWidth()) - 1,
                                                                        static_cast<int>(m_currentMap->getHeight()) - 1));
    auto firstMapTilePosition = getTileLeftUpperCornerScreenCoord(0);
    auto lastMapTilePosition = getTileRightLowerCornerScreenCoord(lastMapTileIndex);
    if (dragEndPosition.x() < firstMapTilePosition.x() ||
        dragEndPosition.x() > lastMapTilePosition.x() ||
        dragEndPosition.y() < firstMapTilePosition.y() ||
        dragEndPosition.y() > lastMapTilePosition.y()) {
        return;
    }
    std::set<int> newIndices;
    for (auto indice : m_pasteResultIndices) {
        auto newIndex = indice + (endTileIndex - startTileIndex);
        // Check that new tile index is in map range
        if (newIndex < 0 || newIndex > lastMapTileIndex) {
            return;
        }
        newIndices.emplace(newIndex);
    }
    auto firstIndiceCoord = m_currentMap->getCoordFromTileIndex(*(newIndices.begin()));
    auto tempSelectionStartPosition = m_pasteSelectionStartPosition;
    auto tempSelectionEndPosition = m_pasteSelectionEndPosition;
    tempSelectionStartPosition.setX(firstIndiceCoord.x());
    tempSelectionStartPosition.setY(firstIndiceCoord.y());
    tempSelectionEndPosition.setX(firstIndiceCoord.x());
    tempSelectionEndPosition.setY(firstIndiceCoord.y());
    for (const auto indice : newIndices) {
        const auto indicePoint = m_currentMap->getCoordFromTileIndex(indice);
        if (indicePoint.x() < tempSelectionStartPosition.x()) {
            tempSelectionStartPosition.setX(indicePoint.x());
        }
        if (indicePoint.x() > tempSelectionEndPosition.x()) {
            tempSelectionEndPosition.setX(indicePoint.x());
        }
        if (indicePoint.y() < tempSelectionStartPosition.y()) {
            tempSelectionStartPosition.setY(indicePoint.y());
        }
        if (indicePoint.y() > tempSelectionEndPosition.y()) {
            tempSelectionEndPosition.setY(indicePoint.y());
        }
    }
    // If we are in the map limit accept the change
    if (!initialCalculation && (tempSelectionEndPosition.x() - tempSelectionStartPosition.x() != m_pasteSelectionEndPosition.x() - m_pasteSelectionStartPosition.x()
                || (tempSelectionEndPosition.y() - tempSelectionStartPosition.y() != m_pasteSelectionEndPosition.y() - m_pasteSelectionStartPosition.y()))) {
        return;
    }
    m_pasteDragEndPosition = dragEndPosition;
    m_pasteResultIndices = newIndices;
    m_pasteSelectionStartPosition = tempSelectionStartPosition;
    m_pasteSelectionEndPosition = tempSelectionEndPosition;
}

bool MapOpenGLWidget::isCursorInPasteSelectionZone(QPoint cursorPosition) const {
    auto selectionZoneStartTileIndex = m_currentMap->getTileIndexFromCoord(Point<int>(m_pasteSelectionStartPosition.x(), m_pasteSelectionStartPosition.y()));
    auto selectionZoneStartCoord = getTileLeftUpperCornerScreenCoord(selectionZoneStartTileIndex);
    auto selectionZoneEndTileIndex = m_currentMap->getTileIndexFromCoord(Point<int>(m_pasteSelectionEndPosition.x(), m_pasteSelectionEndPosition.y()));
    auto selectionZoneEndCoord = getTileRightLowerCornerScreenCoord(selectionZoneEndTileIndex);
    if (cursorPosition.x() > selectionZoneStartCoord.x() &&
        cursorPosition.x() < selectionZoneEndCoord.x() &&
        cursorPosition.y() > selectionZoneStartCoord.y() &&
        cursorPosition.y() < selectionZoneEndCoord.y()) {
        return true;
    } else {
        return false;
    }
}
