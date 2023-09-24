#ifndef MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_
#define MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_

#include <QTimer>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "gameMap.hpp"
#include "selectionMode.hpp"

class MapOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

 public:
    explicit MapOpenGLWidget(QWidget *parent = nullptr);
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void setGridEnabled(bool enabled);
    void resizeGL(int width, int height) override;
    const std::string &getResourcesPath() const;
    void setResourcesPath(const std::string &path);
    SelectionMode getSelectionMode() const;
    void setSelectionMode(SelectionMode mode);
    unsigned int getMapWidth() const;
    unsigned int getMapHeight() const;
    void reloadTextures();
    void startAutoUpdate();
    void stopAutoUpdate();
    void resetMapMovePosition();

 protected:
    void initializeGL() override;
    void paintGL() override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

 private:
    QTimer m_repaintTimer;
    bool m_isGridEnabled;
    SelectionMode m_selectionMode;
    std::string m_resourcesPath;
    bool m_mousePressed;
    float m_translationX;
    float m_translationDragAndDropX;
    float m_translationY;
    float m_translationDragAndDropY;
    int m_selectedTileIndex;
    GLubyte m_selectedTileColor;
    bool m_selectedTileColorGrowing;
    std::map<std::string, unsigned int> m_texturesGLMap;  // Mapping between texture name and OpenGL texture id
    std::map<std::string, const thewarrior::models::Texture &> m_texturesObjMap;  // Mapping between texture name and texture object
    std::shared_ptr<thewarrior::models::GameMap> m_currentMap;
    QPoint m_lastCursorPosition;
    QPoint m_currentCursorPosition;
    const float TILESIZE { 0.2F };
    const float TILEHALFSIZE { TILESIZE / 2.0F };
    const unsigned int ONSCREENTILESIZE { 40 };
    const float TRANSLATIONTOPIXEL { 5.0F };
    const float TILESPACING { 0.0F };
    void updateCursor();
    void draw();
    void drawTileWithTexture(const std::string &textureName, int textureIndex);
    void drawSelectionZone() const;
    void drawGrid() const;
    void drawBlockBorderLeft();
    void drawBlockBorderTop();
    void drawBlockBorderRight();
    void drawBlockBorderBottom();
    int getTileIndex(int onScreenX, int onScreenY);
    glm::vec2 convertScreenCoordToGlCoord(QPoint coord) const;
    void updateSelectedTileColor();

 signals:
    void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> tileIndex);
    void onTileMouseMoveEvent(bool mousePressed, int tileIndex);
};

#endif  // MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_
