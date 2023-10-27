#pragma once

#include <QtWidgets/QWidget>
#include <qtablewidget.h>
#include "mapOpenGLWidget.hpp"

struct MainForm_DebugInfoComponent_Objects {
    QTableWidget *tableWidgetDebugInfo = nullptr;
    MapOpenGLWidget *mapOpenGLWidget = nullptr;
};

class MainForm_DebugInfoComponent : public QWidget {
Q_OBJECT
 public:
    MainForm_DebugInfoComponent();
    void initializeUIObjects(const MainForm_DebugInfoComponent_Objects &objects);
    void connectUIActions();
 private:
    QTableWidget *m_tableWidgetDebugInfo;
    MapOpenGLWidget *m_mapOpenGLWidget;
    ResizeGLComponentInfo m_ComponentSizeInfo;
    float m_translationX = 0;
    float m_translationY = 0;
    int m_lastSelectedTileIndex = 0;
    int m_lastScreenXClicked = 0;
    int m_lastScreenYClicked = 0;
    void refreshTable();
    void addItem(const QString &item, const QString &value);
    void onMapResized(ResizeGLComponentInfo info);
    void onMapMoved(float translationX, float translationY);
    void onTileClicked(int tileIndex, int screenX, int screenY);
};
