#ifndef MAPOPENGLWIDGET_H
#define MAPOPENGLWIDGET_H

#include "map.hpp"
#include <memory>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>

class MapOpenGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MapOpenGLWidget(QWidget *parent = 0);
    ~MapOpenGLWidget();
    void setCurrentMap(std::shared_ptr<GameMap> map);
    void resizeGL(int width, int height);
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
    bool mousePressed;
    float translationX;
    float translationTempX;
    float translationY;
    float translationTempY;
    unsigned int tex1;
    std::shared_ptr<GameMap> currentMap;
    QPoint lastPos;
    const float TILESIZE { 0.2f };
    const float TILEHALFSIZE { TILESIZE / 2.0f };
    const unsigned int ONSCREENTILESIZE { 42 };
    const float TRANSLATIONTOPIXEL { 4.7615f };
    void draw();
    int getTileIndex(unsigned int onScreenX, unsigned int onScreenY);
};

#endif // MAPOPENGLWIDGET_H