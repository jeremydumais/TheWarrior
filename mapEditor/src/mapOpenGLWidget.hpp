#ifndef MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_
#define MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_

#include <qevent.h>
#include <QTimer>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <qpoint.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/optional/optional.hpp>
#include <glm/glm.hpp>
#include "fadeLoopAnimation.hpp"
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "mapView.hpp"
#include "pickerToolSelection.hpp"
#include "selectionMode.hpp"

struct ResizeGLComponentInfo {
    int componentWidth;
    int componentHeight;
    float glTileWidth;
    float glTileHeight;
    float translationXToPixel;
    float translationYToPixel;
    unsigned int tileSizeInPx;
};

class MapOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

 public:
    explicit MapOpenGLWidget(QWidget *parent = nullptr);
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void setGridEnabled(bool enabled);
    void setShowNPCsEnabled(bool enabled);
    void setZoom(int zoomPercentage);
    void setZoomLimit(int min, int max);
    void resizeGL(int width, int height) override;
    const std::string &getResourcesPath() const;
    void setResourcesPath(const std::string &path);
    SelectionMode getSelectionMode() const;
    void setSelectionMode(SelectionMode mode);
    void restorePreviousSelectionMode();
    void setMapView(MapView view);
    void setSelectedNPC(const std::string &npcId);
    void clearSelectedNPC();
    unsigned int getMapWidth() const;
    unsigned int getMapHeight() const;
    void reloadTextures();
    void startAutoUpdate();
    void stopAutoUpdate();
    void resetMapMovePosition();
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void pasteClipboard(const std::vector<thewarrior::models::MapTile> &tiles,
                        const std::set<int> &clipboardSelectedTileIndices);

 protected:
    void initializeGL() override;
    void paintGL() override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

 private slots:
      void updateFrameState();

 private:
    struct ViewportMetrics {
        int width = 0;
        int height = 0;

        float glOrthoSize = 4.0F;
        float onScreenTileSizePx = 40.0F;
        float tileSpacing = 0.0F;

        float glTileWidth = 0.0F;
        float glTileHeight = 0.0F;
        float glTileHalfWidth = 0.0F;
        float glTileHalfHeight = 0.0F;

        float translationXToPixel = 0.0F;
        float translationYToPixel = 0.0F;
    };

    struct CameraState {
        float translationX = 0.0F;
        float translationY = 0.0F;

        float dragAndDropX = 0.0F;
        float dragAndDropY = 0.0F;

        float translationXGL = 0.0F;
        float translationYGL = 0.0F;
    };

    struct InputState {
        bool mousePressed = false;
        QPoint lastCursorPosition = QPoint(0, 0);
        QPoint currentCursorPosition = QPoint(0, 0);
    };

    struct SelectionState {
        SelectionMode currentMode = SelectionMode::Select;

        // Used to restore the selection mode after a temporary mode like NPC Spawn Picker
        boost::optional<SelectionMode> previousMode = {};
        // Used when using alt key to move the map
        boost::optional<SelectionMode> preMapDragMode = {};

        std::set<int> selectedTileIndices = {};
        GLubyte selectedTileColor = 0;
        bool selectedTileColorGrowing = false;

        std::string selectedNPCId = "";
    };

    struct WidgetConfig {
        MapView mapView = MapView::Standard;

        bool gridEnabled = true;
        bool showNPCEnabled = true;

        int zoomPercentage = 100;
        int zoomMin = 20;
        int zoomMax = 250;
    };

    struct MapRendererContext {
        std::vector<std::string> zoneColors = {};
        std::unordered_map<int, const thewarrior::models::NPC *> npcsBySpawnLocation = {};
        FadeLoopAnimation selectedNPCGlowAnimation = FadeLoopAnimation(0.0F, 4.0F, 0.5F);
    };

    QTimer m_repaintTimer;
    ViewportMetrics m_metrics;
    CameraState m_camera;
    InputState m_input;
    SelectionState m_selection;
    WidgetConfig m_config;
    MapRendererContext m_frame;

    std::string m_resourcesPath;
    std::map<std::string, unsigned int> m_texturesGLMap;  // Mapping between texture name and OpenGL texture id
    std::map<std::string, const thewarrior::models::Texture &> m_texturesObjMap;  // Mapping between texture name and texture object
    std::shared_ptr<thewarrior::models::GameMap> m_currentMap;
    //
    // Copy paste section
    std::vector<thewarrior::models::MapTile> m_pasteResult;
    std::set<int> m_pasteResultIndices;
    bool m_pasteDragInProgress;
    QPoint m_pasteDragStartPosition;
    QPoint m_pasteDragEndPosition;
    QPoint m_pasteSelectionStartPosition;
    QPoint m_pasteSelectionEndPosition;

    bool isMultiTileSelectionMode() const;
    void recalculateTileSize();
    void updateCursor(QMouseEvent *event);
    void draw();
    void drawTile(const thewarrior::models::MapTile &tile,
                  int index,
                  const MapRendererContext &ctx);
    void drawTileWithTexture(const std::string &textureName, int textureIndex);
    void drawTileSilhouette(const std::string &textureName, int textureIndex, float outlineWidth);
    void drawColoredTile() const;
    void drawSelectionZone() const;
    void drawPasteResult();
    void drawGrid() const;
    void drawBlockBorderLeft();
    void drawBlockBorderTop();
    void drawBlockBorderRight();
    void drawBlockBorderBottom();
    int getTileIndex(int onScreenX, int onScreenY);
    QPoint getTileLeftUpperCornerScreenCoord(int tileIndex) const;
    QPoint getTileRightLowerCornerScreenCoord(int tileIndex) const;
    glm::vec2 convertScreenCoordToGlCoord(QPoint coord) const;
    void updateSelectedTileColor();
    void calculatePasteSelectionZone(QPoint dragEndPosition, bool initialCalculation = false);
    bool isCursorInPasteSelectionZone(QPoint cursorPosition) const;

 signals:
    void onRecalculateTileSize(ResizeGLComponentInfo info);
    void onTileClicked(const std::set<int> &tileIndex, int screenX, int screenY);
    void onTileMouseMoveEvent(bool mousePressed, int tileIndex);
    void onMapMoved(float translationX, float translationY);
    void onPickerToolTileSelected(PickerToolSelection selection);
    void onNPCSpawnPositionPickerToolTileSelected(const thewarrior::models::MapTile &tile,
                                                  const thewarrior::models::Point<> &coordinate);
    void onNPCSpawnPositionPickerToolCanceled();
    void onZoomChanged(int zoomPercentage);
    void onClipboardPasted();
};

#endif  // MAPEDITOR_SRC_MAPOPENGLWIDGET_HPP_
