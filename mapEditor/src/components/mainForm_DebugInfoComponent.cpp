#include "mainForm_DebugInfoComponent.hpp"
#include "mapOpenGLWidget.hpp"

MainForm_DebugInfoComponent::MainForm_DebugInfoComponent()
    : m_tableWidgetDebugInfo(nullptr),
      m_mapOpenGLWidget(nullptr) {
}

void MainForm_DebugInfoComponent::initializeUIObjects(const MainForm_DebugInfoComponent_Objects &objects) {
    this->m_tableWidgetDebugInfo = objects.tableWidgetDebugInfo;
    this->m_mapOpenGLWidget = objects.mapOpenGLWidget;
    m_tableWidgetDebugInfo->setHorizontalHeaderItem(0, new QTableWidgetItem("Item"));
    m_tableWidgetDebugInfo->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
}

void MainForm_DebugInfoComponent::connectUIActions() {
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onRecalculateTileSize, this, &MainForm_DebugInfoComponent::onMapResized);
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onMapMoved, this, &MainForm_DebugInfoComponent::onMapMoved);
    connect(m_mapOpenGLWidget, &MapOpenGLWidget::onTileClicked, this, &MainForm_DebugInfoComponent::onTileClicked);
}

void MainForm_DebugInfoComponent::refreshTable() {
    m_tableWidgetDebugInfo->model()->removeRows(0, m_tableWidgetDebugInfo->rowCount());
    addItem("Map width (px)", QString::number(m_ComponentSizeInfo.componentWidth));
    addItem("Map height (px)", QString::number(m_ComponentSizeInfo.componentHeight));
    addItem("Tile width", QString::number(m_ComponentSizeInfo.glTileWidth));
    addItem("Tile height", QString::number(m_ComponentSizeInfo.glTileHeight));
    addItem("Tile size (px)", QString::number(m_ComponentSizeInfo.tileSizeInPx));
    addItem("Trans. X to px", QString::number(m_ComponentSizeInfo.translationXToPixel));
    addItem("Trans. Y to px", QString::number(m_ComponentSizeInfo.translationYToPixel));
    addItem("Translation X", QString::number(m_translationX));
    addItem("Translation Y", QString::number(m_translationY));
    addItem("Last selected tile index", QString::number(m_lastSelectedTileIndex));
    addItem("Last clicked screen X", QString::number(m_lastScreenXClicked));
    addItem("Last clicked screen Y", QString::number(m_lastScreenYClicked));
}

void MainForm_DebugInfoComponent::addItem(const QString &item,
        const QString &value) {
    int index = m_tableWidgetDebugInfo->rowCount();
    m_tableWidgetDebugInfo->insertRow(index);
    m_tableWidgetDebugInfo->setItem(index, 0, new QTableWidgetItem(item));
    m_tableWidgetDebugInfo->setItem(index, 1, new QTableWidgetItem(value));
}

void MainForm_DebugInfoComponent::onMapResized(ResizeGLComponentInfo info) {
    m_ComponentSizeInfo = info;
    refreshTable();
}

void MainForm_DebugInfoComponent::onMapMoved(float translationX, float translationY) {
    m_translationX = translationX;
    m_translationY = translationY;
    refreshTable();
}

void MainForm_DebugInfoComponent::onTileClicked(int tileIndex,
        int screenX,
        int screenY) {
    m_lastSelectedTileIndex = tileIndex;
    m_lastScreenXClicked = screenX;
    m_lastScreenYClicked = screenY;
    refreshTable();
}
