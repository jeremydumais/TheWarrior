#pragma once

#include <qdockwidget.h>
#include <qtablewidget.h>
#include "mapOpenGLWidget.hpp"
#include "ui_debugInfoDockWidget.h"

class DebugInfoDockWidget : public QDockWidget {
Q_OBJECT
 public:
    DebugInfoDockWidget(QWidget *parent, MapOpenGLWidget *mapOpenGLWidget);
    void connectUIActions();
 private:
    Ui::DebugInfoDockWidget ui;
    MapOpenGLWidget *m_mapOpenGLWidget;
    ResizeGLComponentInfo m_componentSizeInfo;
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
