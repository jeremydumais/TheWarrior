#include "mapOpenGLWidget.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <QtWidgets>
#include <fmt/format.h>
#include <stb_image.h>
#include <algorithm>
#include <set>
#include <string>
#include <vector>
#include "glColor.hpp"
#include "mapView.hpp"
#include "monsterZone.hpp"
#include "pickerToolSelection.hpp"
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
    m_currentCursorPosition(QPoint(0, 0)) {
    connect(&m_repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    setMouseTracking(true);
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_currentMap = map;
    m_selectedTileIndices = {};
}

void MapOpenGLWidget::setGridEnabled(bool enabled) {
    this->m_isGridEnabled = enabled;
}

void MapOpenGLWidget::setZoom(int zoomPercentage) {
    m_zoomPercentage = zoomPercentage;
    recalculateTileSize();
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
    m_repaintTimer.start(static_cast<int>(1000.0F / 60.0F));
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

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (!m_mousePressed &&
         m_selectionMode == SelectionMode::MoveMap) {
        m_translationDragAndDropX = 0.0f;
        m_translationDragAndDropY = 0.0f;
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
    updateCursor();
    emit onTileMouseMoveEvent(m_mousePressed, getTileIndex(event->pos().x(), event->pos().y()));
}

bool MapOpenGLWidget::isMultiTileSelectionMode() const {
    return m_selectionMode == SelectionMode::Select;
}

void MapOpenGLWidget::recalculateTileSize() {
    const float NOZOOMSCREENTILESIZE = 40.0F;
    const float ZOOMFLOATVALUE = static_cast<float>(m_zoomPercentage) / 100.0F;
    ONSCREENTILESIZE = static_cast<unsigned int>(40.0F * ZOOMFLOATVALUE);
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
        .tileSizeInPx = ONSCREENTILESIZE
    };
    emit onRecalculateTileSize(info);
}

void MapOpenGLWidget::updateCursor() {
    if (m_selectionMode == SelectionMode::MoveMap) {
        setCursor(m_mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
    } else if (m_selectionMode == SelectionMode::Select) {
        setCursor(Qt::ArrowCursor);
    } else if (isMultiTileSelectionMode()) {
        setCursor(Qt::CrossCursor);
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

    if (m_selectedTileIndices.size() > 0) {
        updateSelectedTileColor();
    }
    for (const auto &row : m_currentMap->getTiles()) {
        for (const auto &tile : row) {
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

            if (m_selectedTileIndices.contains(index)) {
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
    glPushMatrix();
    if (m_mousePressed && isMultiTileSelectionMode()) {
        drawSelectionZone();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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
    if (onScreenX / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(m_currentMap->getWidth()) - 1) {
        return -1;
    }
    if (onScreenY / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(m_currentMap->getHeight()) - 1) {
        return -1;
    }
    int x = onScreenX - static_cast<int>(m_translationX * m_translationXToPixel * static_cast<float>(ONSCREENTILESIZE));
    int y = onScreenY + static_cast<int>(m_translationY * m_translationYToPixel * static_cast<float>(ONSCREENTILESIZE));
    int indexX = x / static_cast<int>(ONSCREENTILESIZE);
    int indexY = y / static_cast<int>(ONSCREENTILESIZE);
    int tileIndex { indexX + (indexY * static_cast<int>(m_currentMap->getWidth())) };
    if (tileIndex < 0 || (tileIndex > static_cast<int>(m_currentMap->getWidth() * m_currentMap->getHeight() -1))) {
        return -1;
    }
    return tileIndex;
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
        m_selectedTileColor += 3;
    } else {
        m_selectedTileColor -= 3;
    }

    if (m_selectedTileColor >= 200) {
        m_selectedTileColorGrowing = false;
        m_selectedTileColor--;
    } else if (m_selectedTileColor <= 100) {
        m_selectedTileColorGrowing = true;
    }
}
