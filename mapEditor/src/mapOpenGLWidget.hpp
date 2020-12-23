#ifndef MAPOPENGLWIDGET_H
#define MAPOPENGLWIDGET_H

#include "gameMap.hpp"
#include <map>
#include <memory>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <string>

class MapOpenGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MapOpenGLWidget(QWidget *parent = 0);
    ~MapOpenGLWidget();
    void setCurrentMap(std::shared_ptr<GameMap> map);
    void resizeGL(int width, int height);
    void setResourcesPath(const std::string &path);
protected:
    void initializeGL();
    void paintGL();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void onTileClicked(int tileIndex);
private:
    std::string resourcesPath;
    bool mousePressed;
    float translationX;
    float translationDragAndDropX; 
    float translationY;     
    float translationDragAndDropY;
    int selectedTileIndex;
    std::map<std::string, unsigned int> texturesGLMap; //Mapping between texture name and OpenGL texture id
    std::map<std::string, const Texture &> texturesObjMap; //Mapping between texture name and texture object
    std::shared_ptr<GameMap> currentMap;
    QPoint lastCursorPosition;
    const float TILESIZE { 0.2f };
    const float TILEHALFSIZE { TILESIZE / 2.0f };
    const unsigned int ONSCREENTILESIZE { 40 };
    const float TRANSLATIONTOPIXEL { 5.0f };
    const float TILESPACING { 0.0f };
    void draw();
    void drawTileWithTexture(const std::string &textureName, int textureIndex);
    int getTileIndex(unsigned int onScreenX, unsigned int onScreenY);
};

#endif // MAPOPENGLWIDGET_H