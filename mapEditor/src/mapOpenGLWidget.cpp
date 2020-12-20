#include "mapOpenGLWidget.hpp"
#include <fmt/format.h>
#include <GL/glut.h>
#include <QtWidgets>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace std;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
      mousePressed(false),
      translationX(0.0f),
      translationDragAndDropX(0.0f),
      translationY(0.0f),
      translationDragAndDropY(0.0f),
      selectedTileIndex(-1)
{
}

MapOpenGLWidget::~MapOpenGLWidget()
{
}

void MapOpenGLWidget::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    currentMap = map;
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
    string executablePath = getExecutablePath();

    for(const auto &texture : currentMap->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, texturesGLMap[textureName]); 
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        string texFileName { texture.getFilename() };
        string fullResourcePath = fmt::format("{0}/resources/{1}", executablePath, texFileName);
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

    qglClearColor(Qt::black);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);*/
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

void MapOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!mousePressed) {
        translationDragAndDropX = 0.0f;
        translationDragAndDropY = 0.0f;
        lastCursorPosition = event->pos();
        mousePressed = true;
    }
    updateGL();
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    translationX += translationDragAndDropX;
    translationDragAndDropX = 0;
    translationY += translationDragAndDropY;
    translationDragAndDropY = 0;
    if (event->pos().x() == lastCursorPosition.x() && 
        event->pos().y() == lastCursorPosition.y()) {
        //Found which tile was clicked
        selectedTileIndex = getTileIndex(lastCursorPosition.x(), lastCursorPosition.y());
        emit onTileClicked(selectedTileIndex);
    }
    else {
        //emit onTileClicked(translationX);
    }
    updateGL();
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        translationDragAndDropX = (float)(event->pos().x() - lastCursorPosition.x()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
        /*if (translationX + translationTempX > 0) {
            translationTempX = 0;
        }*/
        translationDragAndDropY = (float)(lastCursorPosition.y() - event->pos().y()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
        /*if (translationTempY > 0) {
            translationTempY = 0;
        }*/
    }
    updateGL();
}

void MapOpenGLWidget::draw()
{
    glEnable(GL_TEXTURE_2D);

    float x { -1.9f };
    float y { 1.9f };
    glTranslatef(x + translationX + translationDragAndDropX, y + translationY + translationDragAndDropY, 0.0);
    unsigned int index {0};
    for(const auto &row : currentMap->getTiles()) {
        for(const auto &tile : row) {        
            glBindTexture(GL_TEXTURE_2D, texturesGLMap[tile.getTextureName()]);
            if (index == selectedTileIndex) {
                qglColor(Qt::green);
            }
            else {
                qglColor(Qt::white);
            }
            glBegin(GL_QUADS);
                glTexCoord2f(0.125f, 1.0f-0.045454545f);
                glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
                glTexCoord2f(0.125f, 1.0f-0.0f);
                glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
                glTexCoord2f(0.0f, 1.0f-0.0f);
                glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
                glTexCoord2f(0.0f, 1.0f-0.045454545f);
                glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
            glEnd();
            glColor3f (1.0, 0.0, 0.0);
            string indexStr { fmt::format("{}", index) };
            glRasterPos3f(-0.02 * indexStr.size(), 0, 0.1);
            for(size_t i = 0; i < indexStr.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, indexStr[i]);
            }
            x += TILESIZE + 0.01f;
            glTranslatef(TILESIZE + 0.01f, 0, 0);
            index++;
        }
        x += row.size() * -(TILESIZE + 0.01f);
        y += -(TILESIZE + 0.01f);
        glTranslatef(row.size() * -(TILESIZE + 0.01f), -(TILESIZE + 0.01f), 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

}

int MapOpenGLWidget::getTileIndex(unsigned int onScreenX, unsigned int onScreenY) 
{
    if (onScreenX / ONSCREENTILESIZE > currentMap->getWidth() - 1) {
        return -1;
    }
    if (onScreenY / ONSCREENTILESIZE > currentMap->getWidth() - 1) {
        return -1;
    }
    unsigned int x = onScreenX - (translationX * (float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
    unsigned int y = onScreenY + (translationY * (float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
    unsigned int indexX = x / ONSCREENTILESIZE;
    unsigned int indexY = y / ONSCREENTILESIZE;
    unsigned int tileIndex { indexX + (indexY * currentMap->getWidth()) };
    return tileIndex;
}

std::string MapOpenGLWidget::getExecutablePath() 
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        return dirname(result);
    }
    else {
        return "";
    }
}
