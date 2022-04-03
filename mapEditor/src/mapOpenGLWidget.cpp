#include "mapOpenGLWidget.hpp"
#include <GL/glut.h>
#include <QtWidgets>
#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>

using namespace std;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_isGridEnabled(true),
      m_selectionMode(SelectionMode::Select),
      m_resourcesPath(""),
      m_mousePressed(false),
      m_translationX(0.0F),
      m_translationDragAndDropX(0.0F),
      m_translationY(0.0F),
      m_translationDragAndDropY(0.0F),
      m_selectedTileIndex(-1),
      m_selectedTileColor(0),
      m_selectedTileColorGrowing(false),
      m_texturesGLMap(map<string, unsigned int>()),
      m_texturesObjMap(map<string, const Texture &>()),
      m_currentMap(nullptr),
      m_lastCursorPosition(QPoint(0, 0)),
      m_currentCursorPosition(QPoint(0, 0))

{
    connect(&m_repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    setMouseTracking(true);
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    m_currentMap = map;
    m_selectedTileIndex = -1;
}

void MapOpenGLWidget::setGridEnabled(bool enabled) 
{
    this->m_isGridEnabled = enabled;
}

QSize MapOpenGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MapOpenGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void MapOpenGLWidget::initializeGL()
{
    reloadTextures();

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClearDepth(1.0F);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    


    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);*/

    //Configure for 60FPS
    startAutoUpdate();
}

void MapOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0F, 0.0F, -10.0F);
    draw();
}

void MapOpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

const std::string& MapOpenGLWidget::getResourcesPath() const
{
    return this->m_resourcesPath;
}

void MapOpenGLWidget::setResourcesPath(const std::string &path) 
{
    this->m_resourcesPath = path;
}

SelectionMode MapOpenGLWidget::getSelectionMode() const
{
    return m_selectionMode;
}

void MapOpenGLWidget::setSelectionMode(SelectionMode mode) 
{
    m_selectionMode = mode;
}

unsigned int MapOpenGLWidget::getMapWidth() const
{
    return this->m_currentMap->getWidth();
}

unsigned int MapOpenGLWidget::getMapHeight() const
{
    return this->m_currentMap->getHeight();
}

void MapOpenGLWidget::reloadTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : m_texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_texturesGLMap.clear();
    m_texturesObjMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : m_currentMap->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &m_texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[textureName]); 
        m_texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        string texFileName { texture.getFilename() };
        string fullResourcePath = fmt::format("{0}/{1}", m_resourcesPath, texFileName);
        unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (imageBytes) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
        }
        else {
            throw runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(imageBytes);
    }
}

void MapOpenGLWidget::startAutoUpdate() 
{
    m_repaintTimer.start(static_cast<int>(1000.0F / 60.0F));
}

void MapOpenGLWidget::stopAutoUpdate() 
{
    m_repaintTimer.stop();
}

void MapOpenGLWidget::resetMapMovePosition() 
{
    m_translationX = 0.0F;
    m_translationY = 0.0F;
    m_translationDragAndDropX = 0.0F;
    m_translationDragAndDropY = 0.0F;
}

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_mousePressed && 
       (m_selectionMode == SelectionMode::MoveMap || 
        m_selectionMode == SelectionMode::ViewBorderMode)) {
        m_translationDragAndDropX = 0.0f;
        m_translationDragAndDropY = 0.0f;
    }
    m_lastCursorPosition = event->pos();
    m_currentCursorPosition = event->pos();
    m_mousePressed = true;
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressed = false;
    m_translationX += m_translationDragAndDropX;
    m_translationDragAndDropX = 0;
    m_translationY += m_translationDragAndDropY;
    m_translationDragAndDropY = 0;
    if (m_selectionMode == SelectionMode::Select) {
        //Found which tile was clicked
        m_selectedTileIndex = getTileIndex(m_lastCursorPosition.x(), m_lastCursorPosition.y());
        m_selectedTileColor = 150;
        m_selectedTileColorGrowing = true;
        emit onTileClicked(m_selectedTileIndex);
    }
    else if (m_selectionMode == SelectionMode::ApplyTexture || 
             m_selectionMode == SelectionMode::ApplyObject || 
             m_selectionMode == SelectionMode::EnableCanStep ||
             m_selectionMode == SelectionMode::DisableCanStep ||
             m_selectionMode == SelectionMode::BlockBorderLeft ||
             m_selectionMode == SelectionMode::BlockBorderTop ||
             m_selectionMode == SelectionMode::BlockBorderRight ||
             m_selectionMode == SelectionMode::BlockBorderBottom ||
             m_selectionMode == SelectionMode::ClearBlockedBorders) {
        //Calculate the list of index selected
        vector<int> selectedTileIndexes;
        QPoint startCoord;
        QPoint endCoord;
        if (m_currentCursorPosition.x() < m_lastCursorPosition.x()) {
            startCoord = m_currentCursorPosition;
            endCoord = m_lastCursorPosition;
        } 
        else {
            startCoord = m_lastCursorPosition;
            endCoord = m_currentCursorPosition;
        }
        QPoint calculatedCoord { startCoord };
        int yDirection { 1 };
        if (startCoord.y() > endCoord.y()) {
            yDirection = -1;
        }
        while(calculatedCoord.x() < endCoord.x() && 
             ((yDirection == 1 && calculatedCoord.y() < endCoord.y()) || (yDirection == -1 && calculatedCoord.y() > endCoord.y()))) {
            selectedTileIndexes.emplace_back(getTileIndex(calculatedCoord.x(), calculatedCoord.y()));
            calculatedCoord.setX(calculatedCoord.x() + static_cast<int>(ONSCREENTILESIZE));
            if (calculatedCoord.x() > endCoord.x()) {
                calculatedCoord.setX(startCoord.x());
                calculatedCoord.setY(calculatedCoord.y() + (static_cast<int>(ONSCREENTILESIZE) * yDirection));
            }
        }
        emit onTileMouseReleaseEvent(selectedTileIndexes);
    }
    auto currentTileIndex { getTileIndex(event->pos().x(), event->pos().y()) };
    if (currentTileIndex != -1) {
        emit onTileMouseReleaseEvent(vector<int> { currentTileIndex });
    }
}

void MapOpenGLWidget::leaveEvent(QEvent *) 
{
    setCursor(Qt::ArrowCursor);
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressed && 
       (m_selectionMode == SelectionMode::MoveMap  || 
        m_selectionMode == SelectionMode::ViewBorderMode)) {
        m_translationDragAndDropX = static_cast<float>(event->pos().x() - m_lastCursorPosition.x()) / (static_cast<float>(ONSCREENTILESIZE) * TRANSLATIONTOPIXEL);
        m_translationDragAndDropY = static_cast<float>(m_lastCursorPosition.y() - event->pos().y()) / (static_cast<float>(ONSCREENTILESIZE) * TRANSLATIONTOPIXEL);
    }
    m_currentCursorPosition = event->pos();
    updateCursor();
    emit onTileMouseMoveEvent(m_mousePressed, getTileIndex(event->pos().x(), event->pos().y()));
}

void MapOpenGLWidget::updateCursor() 
{
    if (m_selectionMode == SelectionMode::MoveMap || 
        m_selectionMode == SelectionMode::ViewBorderMode) {
		setCursor(m_mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
	}
    else if (m_selectionMode == SelectionMode::Select) {
		setCursor(Qt::PointingHandCursor);
	}
    else if (m_selectionMode == SelectionMode::ApplyTexture || 
             m_selectionMode == SelectionMode::ApplyObject ||
             m_selectionMode == SelectionMode::EnableCanStep ||
             m_selectionMode == SelectionMode::DisableCanStep ||
             m_selectionMode == SelectionMode::BlockBorderLeft ||
             m_selectionMode == SelectionMode::BlockBorderTop ||
             m_selectionMode == SelectionMode::BlockBorderRight ||
             m_selectionMode == SelectionMode::BlockBorderBottom ||
             m_selectionMode == SelectionMode::ClearBlockedBorders) {
		setCursor(Qt::CrossCursor);
	}
	else {
		setCursor(Qt::ArrowCursor);
	}
}

void MapOpenGLWidget::draw()
{
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glEnable(GL_TEXTURE_2D);

    float x { -1.9F };
    float y { 1.9F };
    glTranslatef(x, y, 0.0F);
    glPushMatrix();
    glTranslatef(m_translationX + m_translationDragAndDropX, m_translationY + m_translationDragAndDropY, 0.0F);
    int index {0};
    for(const auto &row : m_currentMap->getTiles()) {
        for(const auto &tile : row) {      
            bool hasTexture { false };  
            if (m_texturesGLMap.find(tile.getTextureName()) != m_texturesGLMap.end()) {
                hasTexture = true;
                glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.getTextureName()]);
            }
            if (index == m_selectedTileIndex) {
                glColor3ub(m_selectedTileColor, m_selectedTileColor, m_selectedTileColor);
                //qglColor(QColor(m_selectedTileColor, m_selectedTileColor, m_selectedTileColor));
                updateSelectedTileColor();
            }
            else {
                glColor3f(1.0F, 1.0F, 1.0F);
            }
            //Filter to enable/disable can step on tile
            if (m_selectionMode == SelectionMode::EnableCanStep || m_selectionMode == SelectionMode::DisableCanStep) {
                if (tile.canPlayerSteppedOn()) {
                    glColor3f(0.25F, 1.0F, 0.25F);
                }
                else {
                    glColor3f(1.0F, 0.25F, 0.25F);
                }
            }

            if (hasTexture) {
                drawTileWithTexture(tile.getTextureName(), tile.getTextureIndex());
                //Check if it has an optionnal object
                //TODO create a method and test for bool hasAndObjectDefined() on the mapTile
                if (tile.getObjectTextureName() != "" && tile.getObjectTextureIndex() != -1) {
                    if (tile.getTextureName() != tile.getObjectTextureName()) {
                        if (m_texturesGLMap.find(tile.getObjectTextureName()) != m_texturesGLMap.end()) {
                            glBindTexture(GL_TEXTURE_2D, m_texturesGLMap[tile.getObjectTextureName()]);
                            glPushMatrix();
                            drawTileWithTexture(tile.getObjectTextureName(), tile.getObjectTextureIndex());
                            glPopMatrix();
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }
                    }
                    else {
                        drawTileWithTexture(tile.getObjectTextureName(), tile.getObjectTextureIndex());
                    }
                }
            }
            else {
                //not defined tile (no texture)
                glPushMatrix();
                glColor3f(0.5F, 0.5F, 0.5F);
                glBegin(GL_QUADS);
                    glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
                    glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
                    glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
                    glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
                glEnd();
                glPopMatrix();
            }
            //If we are in block border mode
            if (m_selectionMode == SelectionMode::ViewBorderMode ||
                m_selectionMode == SelectionMode::BlockBorderLeft ||
                m_selectionMode == SelectionMode::BlockBorderTop ||
                m_selectionMode == SelectionMode::BlockBorderRight ||
                m_selectionMode == SelectionMode::BlockBorderBottom ||
                m_selectionMode == SelectionMode::ClearBlockedBorders) {
                auto triggers { tile.getTriggers() };
                for(const auto &trigger : triggers) {
                    if (trigger.getAction() == MapTileTriggerAction::DenyMove) {
                        switch(trigger.getEvent()) {
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
            
            /*glColor3f (1.0f, 0.0f, 0.0f);
            string indexStr { fmt::format("{}", index) };
            glRasterPos3f(-0.02 * indexStr.size(), 0, 0.1);
            for(size_t i = 0; i < indexStr.size(); i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, indexStr[i]);
            }*/
            x += TILESIZE + TILESPACING;
            glTranslatef(TILESIZE + TILESPACING, 0, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            index++;
        }
        x += static_cast<float>(row.size()) * -(TILESIZE + TILESPACING);
        y += -(TILESIZE + TILESPACING);
        glTranslatef(static_cast<float>(row.size()) * -(TILESIZE + TILESPACING), -(TILESIZE + TILESPACING), 0.0f);
    }
    glPopMatrix();
    glPushMatrix();
    if (m_mousePressed && (m_selectionMode == SelectionMode::ApplyTexture || 
                         m_selectionMode == SelectionMode::ApplyObject ||
                         m_selectionMode == SelectionMode::EnableCanStep ||
                         m_selectionMode == SelectionMode::DisableCanStep ||
                         m_selectionMode == SelectionMode::BlockBorderLeft ||
                         m_selectionMode == SelectionMode::BlockBorderTop ||
                         m_selectionMode == SelectionMode::BlockBorderRight ||
                         m_selectionMode == SelectionMode::BlockBorderBottom ||
                         m_selectionMode == SelectionMode::ClearBlockedBorders)) {
        drawSelectionZone();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void MapOpenGLWidget::drawTileWithTexture(const std::string &textureName, int textureIndex) 
{
    float indexTile { static_cast<float>(textureIndex) };
    const Texture &currentTexture { m_texturesObjMap.find(textureName)->second };
    const int NBTEXTUREPERLINE { currentTexture.getWidth() / currentTexture.getTileWidth() };
    float lineIndex = floor(indexTile / static_cast<float>(NBTEXTUREPERLINE));
    const float TEXTURETILEWIDTH { currentTexture.getTileWidthGL() };
    const float TEXTURETILEHEIGHT { currentTexture.getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / 40.0F };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / 40.0F };

    const float TEXTUREX { static_cast<float>((static_cast<int>(indexTile) % NBTEXTUREPERLINE)) };
    glPushMatrix();
    glBegin(GL_QUADS);
        glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
        glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX) + TEXTURETILEWIDTH - TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
        glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTUREHEIGHTADJUSTMENT);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
        glTexCoord2f((TEXTURETILEWIDTH * TEXTUREX)  + TEXTUREWIDTHADJUSTMENT, 1.0f-(TEXTURETILEHEIGHT * (lineIndex + 1.0f)) + TEXTUREHEIGHTADJUSTMENT);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawSelectionZone() const
{
    glm::vec2 startCoord = convertScreenCoordToGlCoord(m_lastCursorPosition);
    glm::vec2 endCoord = convertScreenCoordToGlCoord(m_currentCursorPosition - m_lastCursorPosition);
    glTranslatef(startCoord.x, -startCoord.y, 0.0f);
    glColor4f (1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_QUADS);
        glVertex3f(endCoord.x -TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(endCoord.x -TILEHALFSIZE, -endCoord.y + TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, -endCoord.y + TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
    glEnd();
}

void MapOpenGLWidget::drawGrid() const
{
    glColor3f(0.0F, 0.0F, 0.0F);
    glBegin(GL_LINES);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
    glEnd();
     glBegin(GL_LINES);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
    glEnd();
}

void MapOpenGLWidget::drawBlockBorderLeft() 
{
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(-TILEHALFSIZE + (TILEHALFSIZE/6.0f), -TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE + (TILEHALFSIZE/6.0f), TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(-TILEHALFSIZE + (TILEHALFSIZE/3.0f), -TILEHALFSIZE/8.0f, 0);
        glVertex3f(-TILEHALFSIZE + (TILEHALFSIZE/3.0f), TILEHALFSIZE/8.0f, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE/8.0f, 0);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE/8.0f, 0);
    glEnd();
    glPopMatrix();    
}

void MapOpenGLWidget::drawBlockBorderTop() 
{
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE - (TILEHALFSIZE /6.0f), 0);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, TILEHALFSIZE - (TILEHALFSIZE/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE/8.0f, TILEHALFSIZE - (TILEHALFSIZE /3.0f), 0);
        glVertex3f(TILEHALFSIZE/8.0f, TILEHALFSIZE, 0);
        glVertex3f(-(TILEHALFSIZE/8.0f), TILEHALFSIZE, 0);
        glVertex3f(-(TILEHALFSIZE/8.0f), TILEHALFSIZE - (TILEHALFSIZE/3.0f), 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderRight() 
{
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE - (TILEHALFSIZE/6.0f), -TILEHALFSIZE, 0);
        glVertex3f(TILEHALFSIZE - (TILEHALFSIZE/6.0f), TILEHALFSIZE, 0);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE - (TILEHALFSIZE/3.0f), -TILEHALFSIZE/8.0f, 0);
        glVertex3f(TILEHALFSIZE - (TILEHALFSIZE/3.0f), TILEHALFSIZE/8.0f, 0);
        glVertex3f(TILEHALFSIZE, TILEHALFSIZE/8.0f, 0);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE/8.0f, 0);
    glEnd();
    glPopMatrix();
}

void MapOpenGLWidget::drawBlockBorderBottom() 
{
    glColor3f(1.0F, 0.0F, 0.0F);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE + (TILEHALFSIZE /6.0f), 0);
        glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
        glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE + (TILEHALFSIZE/6.0f), 0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glBegin(GL_QUADS);
        glVertex3f(TILEHALFSIZE/8.0f, -TILEHALFSIZE + (TILEHALFSIZE /3.0f), 0);
        glVertex3f(TILEHALFSIZE/8.0f, -TILEHALFSIZE, 0);
        glVertex3f(-(TILEHALFSIZE/8.0f), -TILEHALFSIZE, 0);
        glVertex3f(-(TILEHALFSIZE/8.0f), -TILEHALFSIZE + (TILEHALFSIZE/3.0f), 0);
    glEnd();
    glPopMatrix();
}
int MapOpenGLWidget::getTileIndex(int onScreenX, int onScreenY) 
{
    if (onScreenX / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(m_currentMap->getWidth()) - 1) {
        return -1;
    }
    if (onScreenY / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(m_currentMap->getHeight()) - 1) {
        return -1;
    }
    int x = onScreenX - static_cast<int>(m_translationX * TRANSLATIONTOPIXEL * static_cast<float>(ONSCREENTILESIZE));
    int y = onScreenY + static_cast<int>(m_translationY * TRANSLATIONTOPIXEL * static_cast<float>(ONSCREENTILESIZE));
    int indexX = x / static_cast<int>(ONSCREENTILESIZE);
    int indexY = y / static_cast<int>(ONSCREENTILESIZE);
    int tileIndex { indexX + (indexY * static_cast<int>(m_currentMap->getWidth())) };
    if (tileIndex < 0 || (tileIndex > static_cast<int>(m_currentMap->getWidth() * m_currentMap->getHeight() -1))) {
        return -1;
    }
    return tileIndex;
}

glm::vec2 MapOpenGLWidget::convertScreenCoordToGlCoord(QPoint coord) const
{
    float x = static_cast<float>(coord.x()) - ((m_translationX / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    float y = static_cast<float>(coord.y()) + ((m_translationY / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    glm::vec2 retVal;
    retVal.x = x / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE);
    retVal.y = y / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE);
    return retVal;
}

void MapOpenGLWidget::updateSelectedTileColor() 
{
    if (m_selectedTileColorGrowing) {
        m_selectedTileColor++;
    }
    else {
        m_selectedTileColor--;
    }
    
    if (m_selectedTileColor == 255) {
        m_selectedTileColorGrowing = false;
        m_selectedTileColor--;
        
    }
    else if (m_selectedTileColor == 150) {
        m_selectedTileColorGrowing = true;
    }
}
