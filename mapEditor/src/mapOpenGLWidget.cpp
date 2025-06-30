#include "mapOpenGLWidget.hpp"
#include <GL/gl.h>
#include <qnamespace.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <QtWidgets>
#include <fmt/format.h>
#include <stb_image.h>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "glColor.hpp"
#include "mapView.hpp"
#include "monsterZone.hpp"
#include "pickerToolSelection.hpp"
#include "point.hpp"
#include "selectionMode.hpp"

using namespace thewarrior::models;
using thewarrior::ui::getVec3FromRGBString;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
    m_isGridEnabled(true),
    m_selectionMode(SelectionMode::Select),
    m_mapView(MapView::Standard),
    m_resourcesPath(""),
    m_mousePressed(false),
    m_translationX(0.0F),
    m_translationDragAndDropX(0.0F),
    m_translationY(0.0F),
    m_translationDragAndDropY(0.0F),
    m_selectedTileIndices({}),
    m_selectedTileColor(0),
    m_selectedTileColorGrowing(false),
    m_texturesGLMap(std::map<std::string, unsigned int>()),
    m_texturesObjMap(std::map<std::string, const Texture &>()),
    m_currentMap(nullptr),
    m_lastCursorPosition(QPoint(0, 0)),
    m_currentCursorPosition(QPoint(0, 0)),
    m_pasteResult({}),
    m_pasteDragInProgress(false),
    m_pasteDragStartPosition(QPoint(0, 0)),
    m_pasteDragEndPosition(QPoint(0, 0)) {
    connect(&m_repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    setMouseTracking(true);
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_currentMap = map;
    m_selectedTileIndices = {};
    m_pasteResult = {};
    m_pasteResultIndices = {};
    m_pasteDragInProgress = false;
    m_pasteDragStartPosition = QPoint(0, 0);
    m_pasteDragEndPosition = QPoint(0, 0);
    m_pasteSelectionStartPosition = QPoint(0, 0);
    m_pasteSelectionEndPosition = QPoint(0, 0);
}

void MapOpenGLWidget::setGridEnabled(bool enabled) {
    this->m_isGridEnabled = enabled;
}

void MapOpenGLWidget::setZoom(int zoomPercentage) {
    m_zoomPercentage = zoomPercentage;
    recalculateTileSize();
}

void MapOpenGLWidget::setZoomLimit(int min, int max) {
    m_zoomPercentageMin = min;
    m_zoomPercentageMax = max;
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
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float HALFGLORTHOSIZE = GLORTHOSIZE / 2.0F;
#ifdef QT_OPENGL_ES_1
    glOrthof(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#else
    glOrtho(-HALFGLORTHOSIZE, HALFGLORTHOSIZE, -HALFGLORTHOSIZE, HALFGLORTHOSIZE, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
    recalculateTileSize();
}

const std::string& MapOpenGLWidget::getResourcesPath() const {
    return this->m_resourcesPath;
}

void MapOpenGLWidget::setResourcesPath(const std::string &path) {
    this->m_resourcesPath = path;
}

SelectionMode MapOpenGLWidget::getSelectionMode() const {
    return m_selectionMode;
}

void MapOpenGLWidget::setSelectionMode(SelectionMode mode) {
    m_selectionMode = mode;
}

void MapOpenGLWidget::setMapView(MapView view) {
    m_mapView = view;
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
    m_repaintTimer.start(static_cast<int>(1000.0F / 10.0F));
}

void MapOpenGLWidget::stopAutoUpdate() {
    m_repaintTimer.stop();
}

void MapOpenGLWidget::resetMapMovePosition() {
    m_translationX = 0.0F;
    m_translationY = 0.0F;
    m_translationDragAndDropX = 0.0F;
    m_translationDragAndDropY = 0.0F;
    m_translationXGL = 0.0F;
    m_translationYGL = 0.0F;
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
    m_zoomPercentage += event->angleDelta().y() / 40;
    if (m_zoomPercentage < m_zoomPercentageMin) {
        m_zoomPercentage = m_zoomPercentageMin;
    } else if (m_zoomPercentage > m_zoomPercentageMax) {
        m_zoomPercentage = m_zoomPercentageMax;
    }
    emit onZoomChanged(m_zoomPercentage);
    recalculateTileSize();
}

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    bool altPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::AltModifier);
    if (altPressed) {
        m_oldSelectionMode = m_selectionMode;
        m_selectionMode = SelectionMode::MoveMap;
    } else if (!m_mousePressed &&
         m_selectionMode == SelectionMode::MoveMap) {
        m_translationDragAndDropX = 0.0f;
        m_translationDragAndDropY = 0.0f;
    } else if (!m_mousePressed && m_selectionMode == SelectionMode::Paste) {
        if (isCursorInPasteSelectionZone(event->pos())) {
            m_pasteDragStartPosition = QPoint(event->pos());
            m_pasteDragInProgress = true;
        }
    }
    m_lastCursorPosition = event->pos();
    m_currentCursorPosition = event->pos();
    m_mousePressed = true;
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    m_mousePressed = false;
    m_translationX += m_translationDragAndDropX;
    m_translationDragAndDropX = 0;
    m_translationY += m_translationDragAndDropY;
    m_translationDragAndDropY = 0;
    m_translationXGL = m_translationX * m_translationXToPixel;
    m_translationYGL = m_translationY * m_translationYToPixel;
    emit onMapMoved(m_translationX, m_translationY);
    if (isMultiTileSelectionMode()) {
        if (!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_selectedTileIndices.clear();
        }
        // Calculate the list of index selected
        QPoint startCoord;
        QPoint endCoord;
        // We are always managing selection from top left to bottom right
        if (m_currentCursorPosition.x() < m_lastCursorPosition.x()) {
            startCoord = m_currentCursorPosition;
            endCoord = m_lastCursorPosition;
        } else {
            startCoord = m_lastCursorPosition;
            endCoord = m_currentCursorPosition;
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
        QPoint endCoordToTileBorder(endCoord.x() + (static_cast<int>(ONSCREENTILESIZE) - (endCoord.x() % static_cast<int>(ONSCREENTILESIZE))),
                endCoord.y() + (static_cast<int>(ONSCREENTILESIZE) - (endCoord.y() % static_cast<int>(ONSCREENTILESIZE))));
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
                m_selectedTileIndices.insert(tileIndex);
            }
            calculatedCoord.setX(calculatedCoord.x() + static_cast<int>(ONSCREENTILESIZE));
            if (calculatedCoord.x() >= endCoordToTileBorder.x()) {
                calculatedCoord.setX(startCoord.x());
                calculatedCoord.setY(calculatedCoord.y() + static_cast<int>(ONSCREENTILESIZE));
            }
        }
        m_selectedTileColor = 100;
        m_selectedTileColorGrowing = true;
        emit onTileClicked(m_selectedTileIndices, event->x(), event->y());
    } else if (m_selectionMode == SelectionMode::PickerTool) {
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
    } else if (m_selectionMode == SelectionMode::Paste) {
        if (m_pasteDragInProgress) {
            calculatePasteSelectionZone(event->pos());
        } else {
            emit onClipboardPasted();
            m_selectionMode = SelectionMode::Select;
            size_t tileIndexInSelection = 0;
            for (auto mapTile : m_currentMap->getTilesForEditing(m_pasteResultIndices)) {
                *mapTile = m_pasteResult.at(tileIndexInSelection);
                tileIndexInSelection++;
            }
            m_pasteResult = {};
            m_pasteResultIndices = {};
        }
        m_pasteDragInProgress = false;
    }
    if (m_oldSelectionMode.has_value()) {
        m_selectionMode = m_oldSelectionMode.value();
        m_oldSelectionMode = {};
    }
}

void MapOpenGLWidget::leaveEvent(QEvent *) {
    setCursor(Qt::ArrowCursor);
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_mousePressed &&
            m_selectionMode == SelectionMode::MoveMap) {
        m_translationDragAndDropX = static_cast<float>(event->pos().x() - m_lastCursorPosition.x()) / (static_cast<float>(ONSCREENTILESIZE) * m_translationXToPixel);
        m_translationDragAndDropY = static_cast<float>(m_lastCursorPosition.y() - event->pos().y()) / (static_cast<float>(ONSCREENTILESIZE) * m_translationYToPixel);
    }
    m_currentCursorPosition = event->pos();
    updateCursor(event);
    emit onTileMouseMoveEvent(m_mousePressed, getTileIndex(event->pos().x(), event->pos().y()));
}

bool MapOpenGLWidget::isMultiTileSelectionMode() const {
    return m_selectionMode == SelectionMode::Select;
}

void MapOpenGLWidget::recalculateTileSize() {
    const float NOZOOMSCREENTILESIZE = 40.0F;
    const float ZOOMFLOATVALUE = static_cast<float>(m_zoomPercentage) / 100.0F;
    ONSCREENTILESIZE = (40.0F * ZOOMFLOATVALUE);
    float nbOfTilesForWidth = static_cast<float>(m_width) / static_cast<float>(NOZOOMSCREENTILESIZE);
    float nbOfTilesForHeight = static_cast<float>(m_height) / static_cast<float>(NOZOOMSCREENTILESIZE);
    m_glTileWidth = (static_cast<float>(m_width) / 10.0F / nbOfTilesForWidth / nbOfTilesForWidth) * ZOOMFLOATVALUE;
    m_glTileHeight = (static_cast<float>(m_height) / 10.0F / nbOfTilesForHeight / nbOfTilesForHeight) * ZOOMFLOATVALUE;
    m_glTileHalfWidth = m_glTileWidth / 2.0F;
    m_glTileHalfHeight = m_glTileHeight / 2.0F;
    m_translationXToPixel = static_cast<float>(m_width) / static_cast<float>(ONSCREENTILESIZE) / GLORTHOSIZE;
    m_translationYToPixel = static_cast<float>(m_height) / static_cast<float>(ONSCREENTILESIZE) / GLORTHOSIZE;
    m_translationX = m_translationXGL / m_translationXToPixel;
    m_translationY = m_translationYGL / m_translationYToPixel;
    ResizeGLComponentInfo info {
        .componentWidth = m_width,
        .componentHeight = m_height,
        .glTileWidth = m_glTileWidth,
        .glTileHeight = m_glTileHeight,
        .translationXToPixel = m_translationXToPixel,
        .translationYToPixel = m_translationYToPixel,
        .tileSizeInPx = static_cast<unsigned int>(ONSCREENTILESIZE)
    };
    emit onRecalculateTileSize(info);
}

void MapOpenGLWidget::updateCursor(QMouseEvent *event) {
    if (m_selectionMode == SelectionMode::MoveMap) {
        setCursor(m_mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
    } else if (m_selectionMode == SelectionMode::Select) {
        setCursor(Qt::ArrowCursor);
    } else if (isMultiTileSelectionMode()) {
        setCursor(Qt::CrossCursor);
    } else if (m_selectionMode == SelectionMode::Paste) {
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

    float x { -(GLORTHOSIZE / 2.0F) + m_glTileHalfWidth };
    float y { (GLORTHOSIZE / 2.0F) - m_glTileHalfHeight  };
    glTranslatef(x, y, 0.0F);
    glPushMatrix();
    glTranslatef(m_translationX + m_translationDragAndDropX, m_translationY + m_translationDragAndDropY, 0.0F);
    int index {0};
    const auto &monsterZones = m_currentMap->getMonsterZones();
    std::vector<std::string> zoneColors = {};
    std::transform(monsterZones.begin(),
                   monsterZones.end(),
                   std::back_inserter(zoneColors),
                   [](const MonsterZone &zone) -> std::string { return zone.getColor().getValue(); });

    if (m_selectionMode == SelectionMode::Select || m_selectionMode == SelectionMode::Paste) {
        updateSelectedTileColor();
    }
    glPushMatrix();
    for (const auto &row : m_currentMap->getTiles()) {
        for (const auto &tile : row) {
            drawTile(tile, index, zoneColors);

            x += m_glTileWidth + TILESPACING;
            glTranslatef(m_glTileWidth + TILESPACING, 0, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            index++;
        }
        x += static_cast<float>(row.size()) * -(m_glTileWidth + TILESPACING);
        y += -(m_glTileHeight + TILESPACING);
        glTranslatef(static_cast<float>(row.size()) * -(m_glTileWidth + TILESPACING), -(m_glTileHeight + TILESPACING), 0.0f);
    }
    glPopMatrix();
    if (m_selectionMode == SelectionMode::Paste || m_oldSelectionMode == SelectionMode::Paste) {
        drawPasteResult();
    }
    glPopMatrix();
    glPushMatrix();
    if (m_mousePressed && isMultiTileSelectionMode()) {
        drawSelectionZone();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void MapOpenGLWidget::drawTile(const MapTile &tile, int index, const std::vector<std::string> &zoneColors) {
    bool hasTexture { false };
    if (m_texturesGLMap.find(tile.getTextureName()) != m_texturesGLMap.end()) {
        hasTexture = true;
        glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.getTextureName()]);
    }

    float transparency = 1.0F;
    // Filter to apply/clear monster zone
    if (m_mapView == MapView::MonsterZones) {
        transparency = 0.2F;
        glColor4f(1.0F, 1.0F, 1.0F, 0.2F);
    }
    if (m_mapView == MapView::CanStep) {
        transparency = 0.5F;
        glColor4f(1.0F, 1.0F, 1.0F, 0.5F);
    }

    if ((m_selectionMode == SelectionMode::Select && m_selectedTileIndices.contains(index)) ||
        (m_selectionMode == SelectionMode::Paste && m_pasteResultIndices.contains(index))) {
        glColor4ub(m_selectedTileColor, m_selectedTileColor, m_selectedTileColor, static_cast<GLubyte>(transparency * 255.0F));
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
        glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
        glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
        glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
        glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
        glEnd();
        glPopMatrix();
    }

    // Filter to apply/clear monster zone
    if (m_mapView == MapView::MonsterZones) {
        if (tile.getMonsterZoneIndex() != -1) {
            const auto zoneColor = getVec3FromRGBString(zoneColors[static_cast<size_t>(tile.getMonsterZoneIndex())]);
            glColor4f(zoneColor.r, zoneColor.g, zoneColor.b, 0.4F);
            drawColoredTile();
        }
    }
    // Filter to enable/disable can step on tile
    if (m_mapView == MapView::CanStep) {
        if (tile.canPlayerSteppedOn()) {
            glColor4f(0.25F, 1.0F, 0.25F, 0.4F);
        } else {
            glColor4f(1.0F, 0.25F, 0.25F, 0.4F);
        }
        drawColoredTile();
    }

    // If we are in block border mode
    if (m_mapView == MapView::BlockedBorders) {
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
    if (m_isGridEnabled) {
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
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / static_cast<float>(ONSCREENTILESIZE) };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / static_cast<float>(ONSCREENTILESIZE) };

    const float TEXTUREX { static_cast<float>((static_cast<int>(indexTile) % NBTEXTUREPERLINE)) };
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawColoredTile() const {
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawSelectionZone() const {
    glm::vec2 startCoord = convertScreenCoordToGlCoord(m_lastCursorPosition);
    glm::vec2 endCoord = convertScreenCoordToGlCoord(m_currentCursorPosition - m_lastCursorPosition);
    glTranslatef(startCoord.x, -startCoord.y, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(endCoord.x -m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(endCoord.x -m_glTileHalfWidth, -endCoord.y + m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -endCoord.y + m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
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
        glTranslatef(static_cast<float>(point.x()) * (m_glTileWidth + TILESPACING),
                static_cast<float>(point.y()) * -(m_glTileHeight + TILESPACING), 0.0f);
        drawTile(tile, tileIndice, {});
        glPopMatrix();
        index++;
    }
    // Draw a selection zone around the pasted elements
    const auto selectionBoxWidth = m_glTileWidth * (static_cast<float>(m_pasteSelectionEndPosition.x()) - static_cast<float>(m_pasteSelectionStartPosition.x()));
    const auto selectionBoxHeight = m_glTileHeight * (static_cast<float>(m_pasteSelectionEndPosition.y()) - static_cast<float>(m_pasteSelectionStartPosition.y()));
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(static_cast<float>(m_pasteSelectionStartPosition.x()) * (m_glTileWidth + TILESPACING),
            static_cast<float>(m_pasteSelectionStartPosition.y()) * -(m_glTileHeight + TILESPACING), 0.0f);
    glColor4f(1.0F, 1.0F, 1.0F, 0.7F);
    glBegin(GL_LINES);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight - selectionBoxHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_glTileHalfWidth + selectionBoxWidth, -m_glTileHalfHeight - selectionBoxHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight - selectionBoxHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_glTileHalfWidth + selectionBoxWidth, -m_glTileHalfHeight - selectionBoxHeight, 0);
    glVertex3f(m_glTileHalfWidth + selectionBoxWidth, m_glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth + selectionBoxWidth, m_glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawGrid() const {
    glColor3f(0.0F, 0.0F, 0.0F);
    glBegin(GL_LINES);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glEnd();
}

void MapOpenGLWidget::drawBlockBorderLeft() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_glTileHalfWidth + (m_glTileHalfWidth/6.0f), -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth + (m_glTileHalfWidth/6.0f), m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(-m_glTileHalfWidth + (m_glTileHalfWidth/3.0f), -m_glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_glTileHalfWidth + (m_glTileHalfWidth/3.0f), m_glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight/8.0f, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight/8.0f, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderTop() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight - (m_glTileHalfHeight /6.0f), 0);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, m_glTileHalfHeight - (m_glTileHalfHeight/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth/8.0f, m_glTileHalfHeight - (m_glTileHalfHeight /3.0f), 0);
    glVertex3f(m_glTileHalfWidth/8.0f, m_glTileHalfHeight, 0);
    glVertex3f(-(m_glTileHalfWidth/8.0f), m_glTileHalfHeight, 0);
    glVertex3f(-(m_glTileHalfWidth/8.0f), m_glTileHalfHeight - (m_glTileHalfHeight/3.0f), 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderRight() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth - (m_glTileHalfWidth/6.0f), -m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth - (m_glTileHalfWidth/6.0f), m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight, 0);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth - (m_glTileHalfWidth/3.0f), -m_glTileHalfHeight/8.0f, 0);
    glVertex3f(m_glTileHalfWidth - (m_glTileHalfWidth/3.0f), m_glTileHalfHeight/8.0f, 0);
    glVertex3f(m_glTileHalfWidth, m_glTileHalfHeight/8.0f, 0);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight/8.0f, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderBottom() {
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight + (m_glTileHalfHeight /6.0f), 0);
    glVertex3f(m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight, 0);
    glVertex3f(-m_glTileHalfWidth, -m_glTileHalfHeight + (m_glTileHalfHeight/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(m_glTileHalfWidth/8.0f, -m_glTileHalfHeight + (m_glTileHalfHeight /3.0f), 0);
    glVertex3f(m_glTileHalfWidth/8.0f, -m_glTileHalfHeight, 0);
    glVertex3f(-(m_glTileHalfWidth/8.0f), -m_glTileHalfHeight, 0);
    glVertex3f(-(m_glTileHalfWidth/8.0f), -m_glTileHalfHeight + (m_glTileHalfHeight/3.0f), 0);
    glEnd();
    glPopMatrix();
}

int MapOpenGLWidget::getTileIndex(int onScreenX, int onScreenY) {
    if (static_cast<float>(onScreenX) / ONSCREENTILESIZE > static_cast<float>(m_currentMap->getWidth())) {
        return -1;
    }
    if (static_cast<float>(onScreenY) / ONSCREENTILESIZE > static_cast<float>(m_currentMap->getHeight())) {
        return -1;
    }
    float x = static_cast<float>(onScreenX) - m_translationX * m_translationXToPixel * static_cast<float>(ONSCREENTILESIZE);
    float y = static_cast<float>(onScreenY) + m_translationY * m_translationYToPixel * static_cast<float>(ONSCREENTILESIZE);
    int indexX = static_cast<int>(x / ONSCREENTILESIZE);
    int indexY = static_cast<int>(y / ONSCREENTILESIZE);
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
    float screenX = static_cast<float>(indexX) * ONSCREENTILESIZE +
                  (m_translationX * m_translationXToPixel * ONSCREENTILESIZE);
    float screenY = static_cast<float>(indexY) * ONSCREENTILESIZE -
                  (m_translationY * m_translationYToPixel * ONSCREENTILESIZE);

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
    float screenX = static_cast<float>(indexX) * ONSCREENTILESIZE +
                  ((m_translationX * m_translationXToPixel * ONSCREENTILESIZE) + ONSCREENTILESIZE);
    float screenY = static_cast<float>(indexY) * ONSCREENTILESIZE -
                  ((m_translationY * m_translationYToPixel * ONSCREENTILESIZE) - ONSCREENTILESIZE);

    return QPoint(static_cast<int>(screenX), static_cast<int>(screenY));
}

glm::vec2 MapOpenGLWidget::convertScreenCoordToGlCoord(QPoint coord) const {
    float x = static_cast<float>(coord.x()) - ((m_translationX / m_translationXToPixel / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    float y = static_cast<float>(coord.y()) + ((m_translationY / m_translationYToPixel / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    glm::vec2 retVal;
    retVal.x = x / m_translationXToPixel / static_cast<float>(ONSCREENTILESIZE);
    retVal.y = y / m_translationYToPixel / static_cast<float>(ONSCREENTILESIZE);
    return retVal;
}

void MapOpenGLWidget::updateSelectedTileColor() {
    if (m_selectedTileColorGrowing) {
        m_selectedTileColor += 10;
    } else {
        m_selectedTileColor -= 10;
    }

    if (m_selectedTileColor >= 200) {
        m_selectedTileColorGrowing = false;
        m_selectedTileColor--;
    } else if (m_selectedTileColor <= 100) {
        m_selectedTileColorGrowing = true;
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
