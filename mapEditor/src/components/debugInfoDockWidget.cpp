#include "debugInfoDockWidget.hpp"
#include "mapOpenGLWidget.hpp"

DebugInfoDockWidget::DebugInfoDockWidget(QWidget *parent,
        MapOpenGLWidget *mapOpenGLWidget)
    : QClosableDockWidget(parent),
      ui(Ui::DebugInfoDockWidget()),
      m_mapOpenGLWidget(mapOpenGLWidget) {
    ui.setupUi(this);
    ui.tableWidgetDebugInfo->setHorizontalHeaderItem(0, new QTableWidgetItem("Item"));
    ui.tableWidgetDebugInfo->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    connectUIActions();
}

void DebugInfoDockWidget::connectUIActions() {
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onRecalculateTileSize, this, &DebugInfoDockWidget::onMapResized);
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onMapMoved, this, &DebugInfoDockWidget::onMapMoved);
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onTileClicked, this, &DebugInfoDockWidget::onTileClicked);
}

void DebugInfoDockWidget::refreshTable() {
    ui.tableWidgetDebugInfo->model()->removeRows(0, ui.tableWidgetDebugInfo->rowCount());
    addItem("Map width (px)", QString::number(m_componentSizeInfo.componentWidth));
    addItem("Map height (px)", QString::number(m_componentSizeInfo.componentHeight));
    addItem("Tile width", QString::number(static_cast<double>(m_componentSizeInfo.glTileWidth)));
    addItem("Tile height", QString::number(static_cast<double>(m_componentSizeInfo.glTileHeight)));
    addItem("Tile size (px)", QString::number(m_componentSizeInfo.tileSizeInPx));
    addItem("Trans. X to px", QString::number(static_cast<double>(m_componentSizeInfo.translationXToPixel)));
    addItem("Trans. Y to px", QString::number(static_cast<double>(m_componentSizeInfo.translationYToPixel)));
    addItem("Translation X", QString::number(static_cast<double>(m_translationX)));
    addItem("Translation Y", QString::number(static_cast<double>(m_translationY)));
    QString tileIndices = std::accumulate(m_lastSelectedTileIndices.begin(),
            m_lastSelectedTileIndices.end(),
            QString {},
         [](const QString &acc, int value) {
            return acc.isEmpty() ? QString::number(value) : acc + ", " + QString::number(value);
        });
    addItem("Last selected tile indices", tileIndices);
    addItem("Last clicked screen X", QString::number(m_lastScreenXClicked));
    addItem("Last clicked screen Y", QString::number(m_lastScreenYClicked));
}

void DebugInfoDockWidget::addItem(const QString &item,
        const QString &value) {
    int index = ui.tableWidgetDebugInfo->rowCount();
    ui.tableWidgetDebugInfo->insertRow(index);
    ui.tableWidgetDebugInfo->setItem(index, 0, new QTableWidgetItem(item));
    ui.tableWidgetDebugInfo->setItem(index, 1, new QTableWidgetItem(value));
}

void DebugInfoDockWidget::onMapResized(ResizeGLComponentInfo info) {
    m_componentSizeInfo = info;
    refreshTable();
}

void DebugInfoDockWidget::onMapMoved(float translationX, float translationY) {
    m_translationX = translationX;
    m_translationY = translationY;
    refreshTable();
}

void DebugInfoDockWidget::onTileClicked(const std::set<int> &tileIndex,
        int screenX,
        int screenY) {
    m_lastSelectedTileIndices = tileIndex;
    m_lastScreenXClicked = screenX;
    m_lastScreenYClicked = screenY;
    refreshTable();
}
