#include <fmt/core.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <QtWidgets>
#include <fmt/format.h>
#include <stb_image.h>
#include <GL/gl.h>
#include <qnamespace.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
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
: QOpenGLWidget(parent) {
    connect(&m_repaintTimer, SIGNAL(timeout()), this, SLOT(updateScene()));
    setMouseTracking(true);
    setAutoFillBackground(false);
    QSurfaceFormat fmt;
    fmt.setSwapInterval(1);  // Enable VSync
    setFormat(fmt);
}

const std::string& MapOpenGLWidget::getResourcesPath() const {
    return m_resources.resourcesPath;
}

SelectionMode MapOpenGLWidget::getSelectionMode() const {
    return m_selection.currentMode;
}

const std::string &MapOpenGLWidget::getLastSelectedNPC() const {
    return m_selection.selectedNPCId;
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_resources.currentMap = std::move(map);
    m_selection.selectedTileIndices = {};
    m_paste.pasteResult = {};
    m_paste.pasteResultIndices = {};
    m_paste.dragInProgress = false;
    m_paste.dragStartPosition = QPoint(0, 0);
    m_paste.dragEndPosition = QPoint(0, 0);
    m_paste.selectionStartPosition = QPoint(0, 0);
    m_paste.selectionEndPosition = QPoint(0, 0);
}

void MapOpenGLWidget::setGridEnabled(bool enabled) {
    m_config.gridEnabled = enabled;
}

void MapOpenGLWidget::setShowNPCsEnabled(bool enabled) {
    m_config.showNPCsEnabled = enabled;
}

void MapOpenGLWidget::setZoom(int zoomPercentage) {
    m_config.zoomPercentage = zoomPercentage;

    recomputeTileMetrics();
}

void MapOpenGLWidget::setZoomLimit(int min, int max) {
    m_config.zoomMin = min;
    m_config.zoomMax = max;
}

QSize MapOpenGLWidget::minimumSizeHint() const {
    return {50, 50};
}

QSize MapOpenGLWidget::sizeHint() const {
    return {400, 400};
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
    renderScene();
}

void MapOpenGLWidget::resizeGL(int width, int height) {
    m_metrics.width = width;
    m_metrics.height = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double HALFGLORTHOSIZE = static_cast<double>(m_metrics.glOrthoSize) / 2.0;
#ifdef QT_OPENGL_ES_1
    glOrthof(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#else
    glOrtho(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
    recomputeTileMetrics();
}

void MapOpenGLWidget::updateScene() {
    const auto &monsterZones = m_resources.currentMap->getMonsterZones();
    std::vector<std::string> zoneColors = {};
    std::ranges::transform(monsterZones,
                           std::back_inserter(zoneColors),
                           [](const MonsterZone &zone) -> std::string {
                               return zone.getColor().getValue();
                           });
    m_frame.monsterZoneColors = zoneColors;

    // Load the npcs in an unordered_map to be able to find them by spawn positition O(1)
    // spawn position will be converted to TileIndex
    std::unordered_map<int, NPC> npcsBySpawnLocation;
    std::ranges::transform(m_resources.currentMap->getNPCs(),
                   std::inserter(npcsBySpawnLocation, npcsBySpawnLocation.end()),
                   [this](const NPC &npc) -> std::pair<int, NPC> {
                        const auto &spawnPosition = npc.getSpawnPosition();
                        const auto spawnPositionConverted = Point<int>(static_cast<int>(spawnPosition.x()),
                                                                       static_cast<int>(spawnPosition.y()));
                        const auto tileIndex = m_resources.currentMap->getTileIndexFromCoord(spawnPositionConverted);
                        return std::make_pair(tileIndex, npc);
                   });
    m_frame.npcsBySpawnLocation = npcsBySpawnLocation;

    // Load the selected NPC wandering zones and convert the coords in indices
    const auto selectedNPC = m_resources.currentMap->getNPCById(m_selection.selectedNPCId);
    std::set<size_t> npcWanderingZoneMapIndices;
    if (selectedNPC.has_value()) {
        for (const auto &point : selectedNPC->get().getWanderZone()) {
            try {
                const auto coordInt = Point<int>(static_cast<int>(point.x()),
                                                 static_cast<int>(point.y()));
                npcWanderingZoneMapIndices.emplace(m_resources.currentMap->getTileIndexFromCoord(coordInt));
            } catch (const std::invalid_argument &err) {}
        }
    }
    m_frame.selectedNPCWanderingZoneMapIndices = npcWanderingZoneMapIndices;

    m_frame.selectedNPCGlowAnimation.process();

    if (m_selection.currentMode == SelectionMode::Select || m_selection.currentMode == SelectionMode::Paste) {
        updateSelectedTileColor();
    }
    this->update();
}

void MapOpenGLWidget::setResourcesPath(const std::string &path) {
    m_resources.resourcesPath = path;
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
    return m_resources.currentMap->getWidth();
}

unsigned int MapOpenGLWidget::getMapHeight() const {
    return m_resources.currentMap->getHeight();
}

void MapOpenGLWidget::reloadTextures() {
    // Clear existing textures in graphics memory
    for (auto &glTexture : m_resources.texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_resources.texturesGLMap.clear();
    m_resources.texturesObjMap.clear();
    // Load texture in graphics memory
    for (const auto &texture : m_resources.currentMap->getTextures()) {
        const auto &textureName { texture.getName() };
        glGenTextures(1, &m_resources.texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, m_resources.texturesGLMap[textureName]);
        m_resources.texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width;
        int height;
        int nrChannels;
        std::string texFileName { texture.getFilename() };
        std::string fullResourcePath = fmt::format("{0}/textures/{1}", m_resources.resourcesPath, texFileName);
        unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (imageBytes != nullptr) {
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
    m_paste.pasteResult = tiles;
    m_paste.pasteResultIndices = clipboardSelectedTileIndices;
    m_paste.dragStartPosition = QPoint(0, 0);
    m_paste.dragEndPosition = QPoint(0, 0);
    calculatePasteSelectionZone(m_paste.dragEndPosition, true);
}

void MapOpenGLWidget::wheelEvent(QWheelEvent *event) {
    m_config.zoomPercentage += event->angleDelta().y() / 40;
    if (m_config.zoomPercentage < m_config.zoomMin) {
        m_config.zoomPercentage = m_config.zoomMin;
    } else if (m_config.zoomPercentage > m_config.zoomMax) {
        m_config.zoomPercentage = m_config.zoomMax;
    }
    emit onZoomChanged(m_config.zoomPercentage);
    recomputeTileMetrics();
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
        m_camera.dragAndDropX = 0.0F;
        m_camera.dragAndDropY = 0.0F;
    } else if (!m_input.mousePressed && m_selection.currentMode == SelectionMode::Paste) {
        if (isCursorInPasteSelectionZone(event->pos())) {
            m_paste.dragStartPosition = QPoint(event->pos());
            m_paste.dragInProgress = true;
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
            auto tileIndex = tileIndexAtScreenPos(realCoord.x(), realCoord.y());
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
        auto currentTileIndex = tileIndexAtScreenPos(event->pos().x(), event->pos().y());
        if (currentTileIndex != -1) {
            const auto &tile = m_resources.currentMap->getTileForEditing(currentTileIndex);
            emit onPickerToolTileSelected(PickerToolSelection {
                .textureName = tile.getTextureName(),
                .textureIndex = tile.getTextureIndex(),
                .objectTextureName = tile.getObjectTextureName(),
                .objectTextureIndex = tile.getObjectTextureIndex()
            });
        }
    } else if (m_selection.currentMode == SelectionMode::Paste) {
        if (m_paste.dragInProgress) {
            calculatePasteSelectionZone(event->pos());
        } else {
            emit onClipboardPasted();
            m_selection.currentMode = SelectionMode::Select;
            size_t tileIndexInSelection = 0;
            for (auto *mapTile : m_resources.currentMap->getTilesForEditing(m_paste.pasteResultIndices)) {
                *mapTile = m_paste.pasteResult.at(tileIndexInSelection);
                tileIndexInSelection++;
            }
            m_paste.pasteResult = {};
            m_paste.pasteResultIndices = {};
        }
        m_paste.dragInProgress = false;
    } else if (m_selection.currentMode == SelectionMode::NPCSpawnPositionPickerTool) {
        auto currentTileIndex = tileIndexAtScreenPos(event->pos().x(), event->pos().y());
        if (currentTileIndex != -1) {
            const auto &tile = m_resources.currentMap->getTileForEditing(currentTileIndex);
            const auto position = m_resources.currentMap->getCoordFromTileIndex(currentTileIndex);
            emit onNPCSpawnPositionPickerToolTileSelected(tile, position);
        }
    }
    if (m_selection.preMapDragMode.has_value()) {
        m_selection.currentMode = m_selection.preMapDragMode.value();
        m_selection.preMapDragMode = {};
    }
}

void MapOpenGLWidget::leaveEvent(QEvent */*event*/) {
    setCursor(Qt::ArrowCursor);
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_input.mousePressed &&
            m_selection.currentMode == SelectionMode::MoveMap) {
        m_camera.dragAndDropX = static_cast<float>(event->pos().x() - m_input.lastCursorPosition.x()) / (m_metrics.onScreenTileSizePx * m_metrics.translationXToPixel);
        m_camera.dragAndDropY = static_cast<float>(m_input.lastCursorPosition.y() - event->pos().y()) / (m_metrics.onScreenTileSizePx * m_metrics.translationYToPixel);
        if (m_camera.translationX + m_camera.dragAndDropX > 0) {
            m_camera.dragAndDropX = m_camera.translationX * -1.0F;
        }
        if (m_camera.translationY + m_camera.dragAndDropY < 0) {
            m_camera.dragAndDropY = m_camera.translationY * -1.0F;
        }
    }
    m_input.currentCursorPosition = event->pos();
    updateCursorShape(event);
    emit onTileMouseMoveEvent(m_input.mousePressed, tileIndexAtScreenPos(event->pos().x(), event->pos().y()));
}

bool MapOpenGLWidget::isMultiTileSelectionMode() const {
    return m_selection.currentMode == SelectionMode::Select;
}

void MapOpenGLWidget::recomputeTileMetrics() {
    const float NOZOOMSCREENTILESIZE = 40.0F;
    const float ZOOMFLOATVALUE = static_cast<float>(m_config.zoomPercentage) / 100.0F;
    m_metrics.onScreenTileSizePx = (40.0F * ZOOMFLOATVALUE);
    float nbOfTilesForWidth = static_cast<float>(m_metrics.width) / static_cast<float>(NOZOOMSCREENTILESIZE);
    float nbOfTilesForHeight = static_cast<float>(m_metrics.height) / static_cast<float>(NOZOOMSCREENTILESIZE);
    m_metrics.glTileWidth = (static_cast<float>(m_metrics.width) / 10.0F / nbOfTilesForWidth / nbOfTilesForWidth) * ZOOMFLOATVALUE;
    m_metrics.glTileHeight = (static_cast<float>(m_metrics.height) / 10.0F / nbOfTilesForHeight / nbOfTilesForHeight) * ZOOMFLOATVALUE;
    m_metrics.glTileHalfWidth = m_metrics.glTileWidth / 2.0F;
    m_metrics.glTileHalfHeight = m_metrics.glTileHeight / 2.0F;
    m_metrics.translationXToPixel = static_cast<float>(m_metrics.width) / m_metrics.onScreenTileSizePx / m_metrics.glOrthoSize;
    m_metrics.translationYToPixel = static_cast<float>(m_metrics.height) / m_metrics.onScreenTileSizePx / m_metrics.glOrthoSize;
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

void MapOpenGLWidget::updateCursorShape(QMouseEvent *event) {
    if (m_selection.currentMode == SelectionMode::MoveMap) {
        setCursor(m_input.mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
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

void MapOpenGLWidget::renderScene() {
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glEnable(GL_TEXTURE_2D);

    float xPos { -(m_metrics.glOrthoSize / 2.0F) + m_metrics.glTileHalfWidth };
    float yPos { (m_metrics.glOrthoSize / 2.0F) - m_metrics.glTileHalfHeight  };
    glTranslatef(xPos, yPos, 0.0F);
    glPushMatrix();
    glTranslatef(m_camera.translationX + m_camera.dragAndDropX, m_camera.translationY + m_camera.dragAndDropY, 0.0F);
    const VisibleTileBounds bounds = computeVisibleTileBounds();
    renderVisibleTiles(bounds);

    if (m_selection.currentMode == SelectionMode::Paste || m_selection.preMapDragMode == SelectionMode::Paste) {
        drawPastePreview();
    }
    glPopMatrix();

    glPushMatrix();
    if (m_input.mousePressed && isMultiTileSelectionMode()) {
        drawSelectionRectOverlay();
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void MapOpenGLWidget::renderVisibleTiles(const VisibleTileBounds &bounds) {
    int index = 0;
    int rowIndex = 0;

    glPushMatrix();

    for (const auto& row : m_resources.currentMap->getTiles()) {
        int columnIndex = 0;

        for (const auto& tile : row) {
            const bool isVisible =
                rowIndex >= bounds.firstRow &&
                rowIndex <= bounds.lastRow &&
                columnIndex >= bounds.firstColumn &&
                columnIndex <= bounds.lastColumn;

            if (isVisible) {
                drawTile(tile, index, m_frame);
            }

            glTranslatef(m_metrics.glTileWidth + m_metrics.tileSpacing, 0.0F, 0.0F);
            glBindTexture(GL_TEXTURE_2D, 0);

            ++index;
            ++columnIndex;
        }

        glTranslatef(static_cast<float>(row.size()) * -(m_metrics.glTileWidth + m_metrics.tileSpacing),
                     -(m_metrics.glTileHeight + m_metrics.tileSpacing),
                     0.0F);

        ++rowIndex;
    }

    glPopMatrix();
}

MapOpenGLWidget::VisibleTileBounds MapOpenGLWidget::computeVisibleTileBounds() const {
    const float translatedX = m_camera.translationX + m_camera.dragAndDropX;
    const float translatedY = m_camera.translationY + m_camera.dragAndDropY;

    const int firstColumn = static_cast<int>(
        std::abs(translatedX * m_metrics.translationXToPixel));
    const int lastColumn = firstColumn + static_cast<int>(
        std::ceil(static_cast<float>(width()) / m_metrics.onScreenTileSizePx));

    const int firstRow = static_cast<int>(
        std::abs(translatedY * m_metrics.translationYToPixel));
    const int lastRow = firstRow + static_cast<int>(
        std::ceil(static_cast<float>(height()) / m_metrics.onScreenTileSizePx));

    return {
        .firstColumn = firstColumn,
        .lastColumn = lastColumn,
        .firstRow = firstRow,
        .lastRow = lastRow
    };
}

void MapOpenGLWidget::drawTile(const MapTile &tile,
                               int index,
                               const MapRendererContext &ctx) {
    bool hasTexture { false };
    if (m_resources.texturesGLMap.contains(tile.getTextureName())) {
        hasTexture = true;
        glBindTexture(GL_TEXTURE_2D, m_resources.texturesGLMap[tile.getTextureName()]);
    }

    applyTileBaseColor(index);

    if (hasTexture) {
        drawTileWithTexture(tile.getTextureName(), tile.getTextureIndex());
        // Tile has an optionnal object
        if (tile.hasObjectTexture()) {
            if (tile.getTextureName() != tile.getObjectTextureName()) {
                if (m_resources.texturesGLMap.contains(tile.getObjectTextureName())) {
                    glBindTexture(GL_TEXTURE_2D, m_resources.texturesGLMap[tile.getObjectTextureName()]);
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
        // Not defined tile (no texture)
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
    drawNpcOverlay(index, ctx);
    drawMapViewOverlay(tile, ctx);
    if (m_config.gridEnabled) {
        drawGrid();
    }
}

void MapOpenGLWidget::drawTileWithTexture(const std::string &textureName, int textureIndex) const {
    float indexTile { static_cast<float>(textureIndex) };
    const Texture &currentTexture { m_resources.texturesObjMap.find(textureName)->second };
    const int NBTEXTUREPERLINE { currentTexture.getWidth() / currentTexture.getTileWidth() };
    float lineIndex = floor(indexTile / static_cast<float>(NBTEXTUREPERLINE));
    const float TEXTURETILEWIDTH { currentTexture.getTileWidthGL() };
    const float TEXTURETILEHEIGHT { currentTexture.getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / m_metrics.onScreenTileSizePx };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / m_metrics.onScreenTileSizePx };

    const float TEXTUREX { static_cast<float>((static_cast<int>(indexTile) % NBTEXTUREPERLINE)) };
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0F-(TEXTURETILEHEIGHT * (lineIndex + 1.0F)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0F-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0F-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0F-(TEXTURETILEHEIGHT * (lineIndex + 1.0F)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawTileOutlinePass(const std::string &textureName, int textureIndex, float outlineWidth) const {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);


    const float onePixelX = (2.0F * m_metrics.glTileHalfWidth)  / m_metrics.onScreenTileSizePx;
    const float onePixelY = (2.0F * m_metrics.glTileHalfHeight) / m_metrics.onScreenTileSizePx;
    const float outlinePx = outlineWidth;
    const float outlineX = onePixelX * outlinePx;
    const float outlineY = onePixelY * outlinePx;

    // Force RGB to constant, keep alpha from texture
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);  // alpha from texture

    const std::array<GLfloat, 4> red = {1.F, 1.F, 1.F, 1.F};
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, red.data());

    const std::array<std::array<float, 2>, 8> offsets = {{
        {{-outlineX, 0.F}}, {{ outlineX, 0.F}},
        {{0.F, -outlineY}}, {{0.F,  outlineY}},
        {{-outlineX, -outlineY}}, {{-outlineX,  outlineY}},
        {{ outlineX, -outlineY}}, {{ outlineX,  outlineY}}
    }};

    for (auto offset : offsets) {
        glPushMatrix();
        glTranslatef(offset[0], offset[1], 0.0F);
        drawTileWithTexture(textureName, textureIndex);
        glPopMatrix();
    }

    // Restore defaults
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_DEPTH_TEST);
    glPopAttrib();
}

void MapOpenGLWidget::drawTileOverlayQuad() const {
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

float MapOpenGLWidget::getTileBaseTransparency(int index) const {
    switch (m_config.mapView) {
        case MapView::MonsterZones:
            return 0.2F;
        case MapView::CanStep:
            return 0.5F;
        case MapView::NPCWanderingZones:
            if (!m_frame.selectedNPCWanderingZoneMapIndices.contains(static_cast<size_t>(index))) {
                return 0.2F;
            }
            return 1.0F;
        case MapView::Standard:
        case MapView::BlockedBorders:
            return 1.0F;
    }
    return 1.0F;
}

void MapOpenGLWidget::applyTileBaseColor(int index) const {
    float transparency = getTileBaseTransparency(index);

    if ((m_selection.currentMode == SelectionMode::Select && m_selection.selectedTileIndices.contains(index)) ||
        (m_selection.currentMode == SelectionMode::Paste && m_paste.pasteResultIndices.contains(index))) {
        glColor4ub(m_selection.selectedTileColor,
                   m_selection.selectedTileColor,
                   m_selection.selectedTileColor,
                   static_cast<GLubyte>(transparency * 255.0F));
    } else {
        glColor4f(1.0F, 1.0F, 1.0F, transparency);
    }
}

void MapOpenGLWidget::drawSelectionRectOverlay() const {
    glm::vec2 startCoord = screenCoordToWorld(m_input.lastCursorPosition);
    glm::vec2 endCoord = screenCoordToWorld(m_input.currentCursorPosition - m_input.lastCursorPosition);
    glTranslatef(startCoord.x, -startCoord.y, 0.0F);
    glColor4f(1.0F, 1.0F, 1.0F, 0.3F);
    glBegin(GL_QUADS);
    glVertex3f(endCoord.x -m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(endCoord.x -m_metrics.glTileHalfWidth, -endCoord.y + m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -endCoord.y + m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glEnd();
}

void MapOpenGLWidget::drawPastePreview() {
    size_t index = 0;
    std::vector<int> tileIndices = {};
    std::ranges::transform(m_paste.pasteResultIndices,
                           std::back_inserter(tileIndices),
                           [](int indice) -> int { return indice; });

    for (const auto &tile : m_paste.pasteResult) {
        auto tileIndice = tileIndices[index];
        // From indice compute the row and col
        const auto point = m_resources.currentMap->getCoordFromTileIndex(tileIndice);
        glPushMatrix();
        glTranslatef(static_cast<float>(point.x()) * (m_metrics.glTileWidth + m_metrics.tileSpacing),
                     static_cast<float>(point.y()) * -(m_metrics.glTileHeight + m_metrics.tileSpacing), 0.0F);
        drawTile(tile, tileIndice, m_frame);
        glPopMatrix();
        index++;
    }
    // Draw a selection zone around the pasted elements
    const auto selectionBoxWidth = m_metrics.glTileWidth * (static_cast<float>(m_paste.selectionEndPosition.x()) - static_cast<float>(m_paste.selectionStartPosition.x()));
    const auto selectionBoxHeight = m_metrics.glTileHeight * (static_cast<float>(m_paste.selectionEndPosition.y()) - static_cast<float>(m_paste.selectionStartPosition.y()));
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(static_cast<float>(m_paste.selectionStartPosition.x()) * (m_metrics.glTileWidth + m_metrics.tileSpacing),
            static_cast<float>(m_paste.selectionStartPosition.y()) * -(m_metrics.glTileHeight + m_metrics.tileSpacing), 0.0F);
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

void MapOpenGLWidget::drawBlockedEdgeLeft() const {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/6.0F), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/6.0F), m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/3.0F), -m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(-m_metrics.glTileHalfWidth + (m_metrics.glTileHalfWidth/3.0F), m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight/8.0F, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockedEdgeTop() const {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight /6.0F), 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight/6.0F), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth/8.0F, m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight /3.0F), 0);
    glVertex3f(m_metrics.glTileHalfWidth/8.0F, m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0F), m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0F), m_metrics.glTileHalfHeight - (m_metrics.glTileHalfHeight/3.0F), 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockedEdgeRight() const {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/6.0F), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/6.0F), m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/3.0F), -m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(m_metrics.glTileHalfWidth - (m_metrics.glTileHalfWidth/3.0F), m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(m_metrics.glTileHalfWidth, m_metrics.glTileHalfHeight/8.0F, 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight/8.0F, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockedEdgeBottom() const {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight /6.0F), 0);
    glVertex3f(m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-m_metrics.glTileHalfWidth, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight/6.0F), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_metrics.glTileHalfWidth/8.0F, -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight /3.0F), 0);
    glVertex3f(m_metrics.glTileHalfWidth/8.0F, -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0F), -m_metrics.glTileHalfHeight, 0);
    glVertex3f(-(m_metrics.glTileHalfWidth/8.0F), -m_metrics.glTileHalfHeight + (m_metrics.glTileHalfHeight/3.0F), 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawNpcOverlay(int index, const MapRendererContext &ctx) const {
    if (m_config.showNPCsEnabled) {
        if (ctx.npcsBySpawnLocation.contains(index)) {
            const auto &npc = ctx.npcsBySpawnLocation.at(index);
            if (m_resources.texturesGLMap.contains(npc.getTextureName())) {
                glBindTexture(GL_TEXTURE_2D, m_resources.texturesGLMap.at(npc.getTextureName()));
                glPushMatrix();
                const int baseTextureIndex = npc.getCurrentFacingTextureIndex();
                if (npc.getId() == m_selection.selectedNPCId) {
                    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
                    drawTileOutlinePass(npc.getTextureName(), baseTextureIndex, ctx.selectedNPCGlowAnimation.getValue());
                }
                drawTileWithTexture(npc.getTextureName(), baseTextureIndex);
                glPopMatrix();
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }
}

void MapOpenGLWidget::drawMapViewOverlay(const MapTile &tile, const MapRendererContext &ctx) {
    switch (m_config.mapView) {
        case MapView::MonsterZones:
            if (tile.getMonsterZoneIndex() != -1) {
                const auto zoneColor = getVec3FromRGBString(ctx.monsterZoneColors[static_cast<size_t>(tile.getMonsterZoneIndex())]);
                glColor4f(zoneColor.r, zoneColor.g, zoneColor.b, 0.4F);
                drawTileOverlayQuad();
            }
            break;
        case MapView::CanStep:
            if (tile.canPlayerSteppedOn()) {
                glColor4f(0.25F, 1.0F, 0.25F, 0.4F);
            } else {
                glColor4f(1.0F, 0.25F, 0.25F, 0.4F);
            }
            drawTileOverlayQuad();
            break;
        case MapView::BlockedBorders:
            drawBlockedBordersOverlay(tile);
            break;
        case MapView::NPCWanderingZones:
        case MapView::Standard:
            break;
    }
}

void MapOpenGLWidget::drawBlockedBordersOverlay(const thewarrior::models::MapTile &tile) const {
    for (const auto &trigger : tile.getTriggers()) {
        if (trigger.getAction() == MapTileTriggerAction::DenyMove) {
            switch (trigger.getEvent()) {
                case MapTileTriggerEvent::MoveLeftPressed:
                    drawBlockedEdgeLeft();
                    break;
                case MapTileTriggerEvent::MoveUpPressed:
                    drawBlockedEdgeTop();
                    break;
                case MapTileTriggerEvent::MoveRightPressed:
                    drawBlockedEdgeRight();
                    break;
                case MapTileTriggerEvent::MoveDownPressed:
                    drawBlockedEdgeBottom();
                    break;
                case MapTileTriggerEvent::None:
                case MapTileTriggerEvent::SteppedOn:
                case MapTileTriggerEvent::ActionButtonPressed:
                    break;
            }
        }
    }
}

int MapOpenGLWidget::tileIndexAtScreenPos(int onScreenX, int onScreenY) {
    if (static_cast<float>(onScreenX) / m_metrics.onScreenTileSizePx > static_cast<float>(m_resources.currentMap->getWidth())) {
        return -1;
    }
    if (static_cast<float>(onScreenY) / m_metrics.onScreenTileSizePx > static_cast<float>(m_resources.currentMap->getHeight())) {
        return -1;
    }
    float xPos = static_cast<float>(onScreenX) - (m_camera.translationX * m_metrics.translationXToPixel * m_metrics.onScreenTileSizePx);
    float yPos = static_cast<float>(onScreenY) + (m_camera.translationY * m_metrics.translationYToPixel * m_metrics.onScreenTileSizePx);
    int indexX = static_cast<int>(xPos / m_metrics.onScreenTileSizePx);
    int indexY = static_cast<int>(yPos / m_metrics.onScreenTileSizePx);
    int tileIndex { indexX + (indexY * static_cast<int>(m_resources.currentMap->getWidth())) };
    if (tileIndex < 0 || (tileIndex > static_cast<int>(m_resources.currentMap->getWidth() * m_resources.currentMap->getHeight()))) {
        return -1;
    }
    return tileIndex;
}

QPoint MapOpenGLWidget::tileTopLeftScreenPos(int tileIndex) const {
    if (!m_resources.currentMap || tileIndex < 0 || tileIndex >= static_cast<int>(m_resources.currentMap->getWidth()) * static_cast<int>(m_resources.currentMap->getHeight())) {
        return {-1, -1};
    }

    int mapWidth = static_cast<int>(m_resources.currentMap->getWidth());
    int indexX = tileIndex % mapWidth;
    int indexY = tileIndex / mapWidth;

    // Reverse the translation logic from getTileIndex
    float screenX = (static_cast<float>(indexX) * m_metrics.onScreenTileSizePx) +
                  (m_camera.translationX * m_metrics.translationXToPixel * m_metrics.onScreenTileSizePx);
    float screenY = (static_cast<float>(indexY) * m_metrics.onScreenTileSizePx) -
                  (m_camera.translationY * m_metrics.translationYToPixel * m_metrics.onScreenTileSizePx);

    return {static_cast<int>(screenX), static_cast<int>(screenY)};
}

QPoint MapOpenGLWidget::tileBottomRightScreenPos(int tileIndex) const {
    if (!m_resources.currentMap || tileIndex < 0 || tileIndex >= static_cast<int>(m_resources.currentMap->getWidth()) * static_cast<int>(m_resources.currentMap->getHeight())) {
        return {-1, -1};
    }

    int mapWidth = static_cast<int>(m_resources.currentMap->getWidth());
    int indexX = tileIndex % mapWidth;
    int indexY = tileIndex / mapWidth;

    // Reverse the translation logic from getTileIndex
    float screenX = (static_cast<float>(indexX) * m_metrics.onScreenTileSizePx) +
                  ((m_camera.translationX * m_metrics.translationXToPixel * m_metrics.onScreenTileSizePx) + m_metrics.onScreenTileSizePx);
    float screenY = (static_cast<float>(indexY) * m_metrics.onScreenTileSizePx) -
                  ((m_camera.translationY * m_metrics.translationYToPixel * m_metrics.onScreenTileSizePx) - m_metrics.onScreenTileSizePx);

    return {static_cast<int>(screenX), static_cast<int>(screenY)};
}

glm::vec2 MapOpenGLWidget::screenCoordToWorld(QPoint coord) const {
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
    auto startTileIndex = tileIndexAtScreenPos(m_paste.dragStartPosition.x(), m_paste.dragStartPosition.y());
    auto endTileIndex = tileIndexAtScreenPos(dragEndPosition.x(), dragEndPosition.y());
    auto lastMapTileIndex = m_resources.currentMap->getTileIndexFromCoord(Point<>(static_cast<int>(m_resources.currentMap->getWidth()) - 1,
                                                                        static_cast<int>(m_resources.currentMap->getHeight()) - 1));
    auto firstMapTilePosition = tileTopLeftScreenPos(0);
    auto lastMapTilePosition = tileBottomRightScreenPos(lastMapTileIndex);
    if (dragEndPosition.x() < firstMapTilePosition.x() ||
        dragEndPosition.x() > lastMapTilePosition.x() ||
        dragEndPosition.y() < firstMapTilePosition.y() ||
        dragEndPosition.y() > lastMapTilePosition.y()) {
        return;
    }
    std::set<int> newIndices;
    for (auto indice : m_paste.pasteResultIndices) {
        auto newIndex = indice + (endTileIndex - startTileIndex);
        // Check that new tile index is in map range
        if (newIndex < 0 || newIndex > lastMapTileIndex) {
            return;
        }
        newIndices.emplace(newIndex);
    }
    auto firstIndiceCoord = m_resources.currentMap->getCoordFromTileIndex(*(newIndices.begin()));
    auto tempSelectionStartPosition = m_paste.selectionStartPosition;
    auto tempSelectionEndPosition = m_paste.selectionEndPosition;
    tempSelectionStartPosition.setX(firstIndiceCoord.x());
    tempSelectionStartPosition.setY(firstIndiceCoord.y());
    tempSelectionEndPosition.setX(firstIndiceCoord.x());
    tempSelectionEndPosition.setY(firstIndiceCoord.y());
    for (const auto indice : newIndices) {
        const auto indicePoint = m_resources.currentMap->getCoordFromTileIndex(indice);
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
    if (!initialCalculation && (tempSelectionEndPosition.x() - tempSelectionStartPosition.x() != m_paste.selectionEndPosition.x() - m_paste.selectionStartPosition.x()
                || (tempSelectionEndPosition.y() - tempSelectionStartPosition.y() != m_paste.selectionEndPosition.y() - m_paste.selectionStartPosition.y()))) {
        return;
    }
    m_paste.dragEndPosition = dragEndPosition;
    m_paste.pasteResultIndices = newIndices;
    m_paste.selectionStartPosition = tempSelectionStartPosition;
    m_paste.selectionEndPosition = tempSelectionEndPosition;
}

bool MapOpenGLWidget::isCursorInPasteSelectionZone(QPoint cursorPosition) const {
    auto selectionZoneStartTileIndex = m_resources.currentMap->getTileIndexFromCoord(Point<int>(m_paste.selectionStartPosition.x(), m_paste.selectionStartPosition.y()));
    auto selectionZoneStartCoord = tileTopLeftScreenPos(selectionZoneStartTileIndex);
    auto selectionZoneEndTileIndex = m_resources.currentMap->getTileIndexFromCoord(Point<int>(m_paste.selectionEndPosition.x(), m_paste.selectionEndPosition.y()));
    auto selectionZoneEndCoord = tileBottomRightScreenPos(selectionZoneEndTileIndex);
    return cursorPosition.x() > selectionZoneStartCoord.x() &&
        cursorPosition.x() < selectionZoneEndCoord.x() &&
        cursorPosition.y() > selectionZoneStartCoord.y() &&
        cursorPosition.y() < selectionZoneEndCoord.y();
}
