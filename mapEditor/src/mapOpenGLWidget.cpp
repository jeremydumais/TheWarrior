#include "mapOpenGLWidget.hpp"
#include <fmt/format.h>
#include <GL/glut.h>
#include <QtWidgets>
#include <QtOpenGL/QtOpenGL>
#include <string>

using namespace std;

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
      mousePressed(false),
      translationX(0.0f),
      translationTempX(0.0f),
      translationY(0.0f),
      translationTempY(0.0f)
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

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void MapOpenGLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    //static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void MapOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    /*glRotatef(90 / 16.0, 1.0, 0.0, 0.0);
    glRotatef(45 / 16.0, 0.0, 1.0, 0.0);
    glRotatef(22 / 16.0, 0.0, 0.0, 1.0);*/
    draw();
}

void MapOpenGLWidget::resizeGL(int width, int height)
{
    /*int side = qMin(width, height);
    /*glViewport((width - side) / 2, (height - side) / 2, side, side);*/
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
        translationTempX = 0.0f;
        translationTempY = 0.0f;
        lastPos = event->pos();
        mousePressed = true;
    }
    updateGL();
}

void MapOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    translationX += translationTempX;
    translationTempX = 0;
    translationY += translationTempY;
    translationTempY = 0;
    if (event->pos().x() == lastPos.x() && 
        event->pos().y() == lastPos.y()) {
        //Found which tile was clicked
        emit onTileClicked(getTileIndex(lastPos.x(), lastPos.y()));
    }
    else {
        //emit onTileClicked(translationX);
    }
    updateGL();
}

void MapOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        translationTempX = (float)(event->pos().x() - lastPos.x()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
        /*if (translationX + translationTempX > 0) {
            translationTempX = 0;
        }*/
        translationTempY = (float)(lastPos.y() - event->pos().y()) / ((float)ONSCREENTILESIZE * TRANSLATIONTOPIXEL);
        /*if (translationTempY > 0) {
            translationTempY = 0;
        }*/
    }
    updateGL();
}

void MapOpenGLWidget::draw()
{
    float x { -1.9f };
    float y { 1.9f };
    glTranslatef(x + translationX + translationTempX, y + translationY + translationTempY, 0.0);
    unsigned int index {0};
    for(const auto &row : currentMap->getTiles()) {
        for(const auto &col : row) {
            
            qglColor(Qt::green);
            glBegin(GL_QUADS);
                glNormal3f(0, 0, -1);
                glVertex3f(-TILEHALFSIZE, -TILEHALFSIZE, 0);
                glVertex3f(-TILEHALFSIZE, TILEHALFSIZE, 0);
                glVertex3f(TILEHALFSIZE, TILEHALFSIZE, 0);
                glVertex3f(TILEHALFSIZE, -TILEHALFSIZE, 0);
            glEnd();
            glColor3f (1.0, 0.0, 0.0);
            string indexStr { fmt::format("{}", index) };
            glRasterPos3f(-0.02 * indexStr.size(), 0, 0.1);
            for(int i = 0; i < indexStr.size(); i++) {
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
