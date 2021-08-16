#include "mapOpenGLWidget.hpp"
#include <fmt/format.h>
#include <GL/glut.h>
#include <QtWidgets>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>

using namespace std;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
      isGridEnabled(true),
      selectionMode(SelectionMode::Select),
      mousePressed(false),
      translationX(0.0f),
      translationDragAndDropX(0.0f),
      translationY(0.0f),
      translationDragAndDropY(0.0f),
      selectedTileIndex(-1)
{
    connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    setMouseTracking(true);
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    currentMap = map;
}

void MapOpenGLWidget::setGridEnabled(bool enabled) 
{
    this->isGridEnabled = enabled;
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

    qglClearColor(Qt::black);
    glClearDepth(1.0f);
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
    glTranslatef(0.0, 0.0, -10.0);
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
    return this->resourcesPath;
}

void MapOpenGLWidget::setResourcesPath(const std::string &path) 
{
    this->resourcesPath = path;
}

SelectionMode MapOpenGLWidget::getSelectionMode() const
{
    return selectionMode;
}

void MapOpenGLWidget::setSelectionMode(SelectionMode mode) 
{
    selectionMode = mode;
}

void MapOpenGLWidget::reloadTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    texturesGLMap.clear();
    texturesObjMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : currentMap->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, texturesGLMap[textureName]); 
        texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        string texFileName { texture.getFilename() };
        string fullResourcePath = fmt::format("{0}/{1}", resourcesPath, texFileName);
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
    repaintTimer.start(1000.0f / 60.0f);
}

void MapOpenGLWidget::stopAutoUpdate() 
{
    repaintTimer.stop();
}

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!mousePressed && 
       (selectionMode == SelectionMode::MoveMap || 
        selectionMode == SelectionMode::ViewBorderMode)) {
        translationDragAndDropX = 0.0f;
        translationDragAndDropY = 0.0f;
    }
    lastCursorPosition = event->pos();
    currentCursorPosition = event->pos();
    mousePressed = true;
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    translationX += translationDragAndDropX;
    translationDragAndDropX = 0;
    translationY += translationDragAndDropY;
    translationDragAndDropY = 0;
    if (selectionMode == SelectionMode::Select) {
        //Found which tile was clicked
        selectedTileIndex = getTileIndex(lastCursorPosition.x(), lastCursorPosition.y());
        selectedTileColor = 200;
        selectedTileColorGrowing = true;
        emit onTileClicked(selectedTileIndex);
    }
    else if (selectionMode == SelectionMode::ApplyTexture || 
             selectionMode == SelectionMode::ApplyObject || 
             selectionMode == SelectionMode::EnableCanStep ||
             selectionMode == SelectionMode::DisableCanStep ||
             selectionMode == SelectionMode::BlockBorderLeft ||
             selectionMode == SelectionMode::BlockBorderTop ||
             selectionMode == SelectionMode::BlockBorderRight ||
             selectionMode == SelectionMode::BlockBorderBottom ||
             selectionMode == SelectionMode::ClearBlockedBorders) {
        //Calculate the list of index selected
        vector<int> selectedTileIndexes;
        QPoint startCoord;
        QPoint endCoord;
        if (currentCursorPosition.x() < lastCursorPosition.x()) {
            startCoord = currentCursorPosition;
            endCoord = lastCursorPosition;
        } 
        else {
            startCoord = lastCursorPosition;
            endCoord = currentCursorPosition;
        }
        QPoint calculatedCoord { startCoord };
        int yDirection { 1 };
        if (startCoord.y() > endCoord.y()) {
            yDirection = -1;
        }
        while(calculatedCoord.x() < endCoord.x() && 
             ((yDirection == 1 && calculatedCoord.y() < endCoord.y()) || (yDirection == -1 && calculatedCoord.y() > endCoord.y()))) {
            selectedTileIndexes.emplace_back(getTileIndex(calculatedCoord.x(), calculatedCoord.y()));
            calculatedCoord.setX(calculatedCoord.x() + ONSCREENTILESIZE);
            if (calculatedCoord.x() > endCoord.x()) {
                calculatedCoord.setX(startCoord.x());
                calculatedCoord.setY(calculatedCoord.y() + (ONSCREENTILESIZE * yDirection));
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
    if (mousePressed && 
       (selectionMode == SelectionMode::MoveMap  || 
        selectionMode == SelectionMode::ViewBorderMode)) {
        translationDragAndDropX = (float)(event->pos().x() - lastCursorPosition.x()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
        translationDragAndDropY = (float)(lastCursorPosition.y() - event->pos().y()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
    }
    currentCursorPosition = event->pos();
    updateCursor();
    emit onTileMouseMoveEvent(mousePressed, getTileIndex(event->pos().x(), event->pos().y()));
}

void MapOpenGLWidget::updateCursor() 
{
    if (selectionMode == SelectionMode::MoveMap || 
        selectionMode == SelectionMode::ViewBorderMode) {
		setCursor(mousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
	}
    else if (selectionMode == SelectionMode::Select) {
		setCursor(Qt::PointingHandCursor);
	}
    else if (selectionMode == SelectionMode::ApplyTexture || 
             selectionMode == SelectionMode::ApplyObject ||
             selectionMode == SelectionMode::EnableCanStep ||
             selectionMode == SelectionMode::DisableCanStep ||
             selectionMode == SelectionMode::BlockBorderLeft ||
             selectionMode == SelectionMode::BlockBorderTop ||
             selectionMode == SelectionMode::BlockBorderRight ||
             selectionMode == SelectionMode::BlockBorderBottom ||
             selectionMode == SelectionMode::ClearBlockedBorders) {
		setCursor(Qt::CrossCursor);
	}
	else {
		setCursor(Qt::ArrowCursor);
	}
}

void MapOpenGLWidget::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    float x { -1.9f };
    float y { 1.9f };
    glTranslatef(x, y, 0.0);
    glPushMatrix();
    glTranslatef(translationX + translationDragAndDropX, translationY + translationDragAndDropY, 0.0);
    int index {0};
    for(const auto &row : currentMap->getTiles()) {
        for(const auto &tile : row) {      
            bool hasTexture { false };  
            if (texturesGLMap.find(tile.getTextureName()) != texturesGLMap.end()) {
                hasTexture = true;
                glBindTexture(GL_TEXTURE_2D, texturesGLMap[tile.getTextureName()]);
            }
            if (index == selectedTileIndex) {
                qglColor(QColor(selectedTileColor, selectedTileColor, selectedTileColor));
                updateSelectedTileColor();
            }
            else {
                qglColor(Qt::white);
            }
            //Filter to enable/disable can step on tile
            if (selectionMode == SelectionMode::EnableCanStep || selectionMode == SelectionMode::DisableCanStep) {
                if (tile.canPlayerSteppedOn()) {
                    qglColor(QColor(64, 255, 64));
                }
                else {
                    qglColor(QColor(255, 64, 64));
                }
            }

            if (hasTexture) {
                drawTileWithTexture(tile.getTextureName(), tile.getTextureIndex());
                //Check if it has an optionnal object
                //TODO create a method and test for bool hasAndObjectDefined() on the mapTile
                if (tile.getObjectTextureName() != "" && tile.getObjectTextureIndex() != -1) {
                    if (tile.getTextureName() != tile.getObjectTextureName()) {
                        if (texturesGLMap.find(tile.getObjectTextureName()) != texturesGLMap.end()) {
                            glBindTexture(GL_TEXTURE_2D, texturesGLMap[tile.getObjectTextureName()]);
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
                qglColor(QColor(128, 128, 128));
                glBegin(GL_QUADS);
                    glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
                    glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
                    glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
                    glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
                glEnd();
                glPopMatrix();
            }
            //If we are in block border mode
            if (selectionMode == SelectionMode::ViewBorderMode ||
                selectionMode == SelectionMode::BlockBorderLeft ||
                selectionMode == SelectionMode::BlockBorderTop ||
                selectionMode == SelectionMode::BlockBorderRight ||
                selectionMode == SelectionMode::BlockBorderBottom ||
                selectionMode == SelectionMode::ClearBlockedBorders) {
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
            if (isGridEnabled) {
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
    if (mousePressed && (selectionMode == SelectionMode::ApplyTexture || 
                         selectionMode == SelectionMode::ApplyObject ||
                         selectionMode == SelectionMode::EnableCanStep ||
                         selectionMode == SelectionMode::DisableCanStep ||
                         selectionMode == SelectionMode::BlockBorderLeft ||
                         selectionMode == SelectionMode::BlockBorderTop ||
                         selectionMode == SelectionMode::BlockBorderRight ||
                         selectionMode == SelectionMode::BlockBorderBottom ||
                         selectionMode == SelectionMode::ClearBlockedBorders)) {
        drawSelectionZone();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void MapOpenGLWidget::drawTileWithTexture(const std::string &textureName, int textureIndex) 
{
    float indexTile { static_cast<float>(textureIndex) };
    const Texture &currentTexture { texturesObjMap.find(textureName)->second };
    const int NBTEXTUREPERLINE { currentTexture.getWidth() / currentTexture.getTileWidth() };
    float lineIndex = static_cast<int>(indexTile / NBTEXTUREPERLINE);
    const float TEXTURETILEWIDTH { currentTexture.getTileWidthGL() };
    const float TEXTURETILEHEIGHT { currentTexture.getTileHeightGL() };
    const float TEXTUREWIDTHADJUSTMENT { TEXTURETILEWIDTH / 40.0f };
    const float TEXTUREHEIGHTADJUSTMENT { TEXTURETILEHEIGHT / 40.0f };

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
    glm::vec2 startCoord = convertScreenCoordToGlCoord(lastCursorPosition);
    glm::vec2 endCoord = convertScreenCoordToGlCoord(currentCursorPosition - lastCursorPosition);
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
    qglColor(Qt::black);
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
    qglColor(QColor(255, 0, 0));
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
    qglColor(QColor(255, 0, 0));
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
    qglColor(QColor(255, 0, 0));
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
    qglColor(QColor(255, 0, 0));
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
    if (onScreenX / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(currentMap->getWidth()) - 1) {
        return -1;
    }
    if (onScreenY / static_cast<int>(ONSCREENTILESIZE) > static_cast<int>(currentMap->getHeight()) - 1) {
        return -1;
    }
    int x = onScreenX - ((translationX * TRANSLATIONTOPIXEL) * static_cast<float>(ONSCREENTILESIZE));
    int y = onScreenY + ((translationY * TRANSLATIONTOPIXEL) * static_cast<float>(ONSCREENTILESIZE));
    int indexX = x / static_cast<int>(ONSCREENTILESIZE);
    int indexY = y / static_cast<int>(ONSCREENTILESIZE);
    int tileIndex { indexX + (indexY * static_cast<int>(currentMap->getWidth())) };
    if (tileIndex < 0 || (tileIndex > static_cast<int>(currentMap->getWidth() * currentMap->getHeight() -1))) {
        return -1;
    }
    return tileIndex;
}

glm::vec2 MapOpenGLWidget::convertScreenCoordToGlCoord(QPoint coord) const
{
    float x = static_cast<float>(coord.x()) - ((translationX / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    float y = static_cast<float>(coord.y()) + ((translationY / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE)) * static_cast<float>(ONSCREENTILESIZE));
    glm::vec2 retVal;
    retVal.x = x / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE);
    retVal.y = y / TRANSLATIONTOPIXEL / static_cast<float>(ONSCREENTILESIZE);
    return retVal;
}

void MapOpenGLWidget::updateSelectedTileColor() 
{
    if (selectedTileColorGrowing) {
        selectedTileColor++;
    }
    else {
        selectedTileColor--;
    }
    
    if (selectedTileColor == 256) {
        selectedTileColorGrowing = false;
        selectedTileColor--;
        
    }
    else if (selectedTileColor == 200) {
        selectedTileColorGrowing = true;
    }
}
