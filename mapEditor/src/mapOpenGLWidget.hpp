#ifndef MAPOPENGLWIDGET_H
#define MAPOPENGLWIDGET_H

#include "gameMap.hpp"
#include "selectionMode.hpp"
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <QTimer>
#include <string>
#include <vector>

class MapOpenGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MapOpenGLWidget(QWidget *parent = 0);
    ~MapOpenGLWidget() = default;
    void setCurrentMap(std::shared_ptr<GameMap> map);
    void setGridEnabled(bool enabled);
    void resizeGL(int width, int height);
    const std::string &getResourcesPath() const;
    void setResourcesPath(const std::string &path);
    SelectionMode getSelectionMode() const;
    void setSelectionMode(SelectionMode mode);
    void reloadTextures();
    void startAutoUpdate();
    void stopAutoUpdate();
protected:
    void initializeGL();
    void paintGL();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
signals:
    void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> tileIndex);
    void onTileMouseMoveEvent(bool mousePressed, int tileIndex);
private:
    QTimer repaintTimer;
    bool isGridEnabled;
	SelectionMode selectionMode;
    std::string resourcesPath;
    bool mousePressed;
    float translationX;
    float translationDragAndDropX; 
    float translationY;     
    float translationDragAndDropY;
    int selectedTileIndex;
	int selectedTileColor;
    bool selectedTileColorGrowing;
    std::map<std::string, unsigned int> texturesGLMap; //Mapping between texture name and OpenGL texture id
    std::map<std::string, const Texture &> texturesObjMap; //Mapping between texture name and texture object
    std::shared_ptr<GameMap> currentMap;
    QPoint lastCursorPosition;
    QPoint currentCursorPosition;
    const float TILESIZE { 0.2f };
    const float TILEHALFSIZE { TILESIZE / 2.0f };
    const unsigned int ONSCREENTILESIZE { 40 };
    const float TRANSLATIONTOPIXEL { 5.0f };
    const float TILESPACING { 0.0f };
    void updateCursor();
    void draw();
    void drawTileWithTexture(const std::string &textureName, int textureIndex);
    void drawSelectionZone() const;
    void drawGrid() const;
    int getTileIndex(int onScreenX, int onScreenY);
    glm::vec2 convertScreenCoordToGlCoord(QPoint coord) const;
    void updateSelectedTileColor();
};

#endif // MAPOPENGLWIDGET_H