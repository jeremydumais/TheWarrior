#pragma once

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <qobjectdefs.h>
#include <string>
#include "mainForm_GLComponent.hpp"
#include "ui_mapPropsComponent.h"

class MapPropsComponent : public QWidget {
Q_OBJECT
 public:
    MapPropsComponent(QWidget *parent,
            MainForm_GLComponent *glComponent);
    void connectUIActions();
    void reset();
    void refresh();
    void setResourcesPath(const std::string &resourcesPath);

 signals:
    void onBeforeApplyChange();
    void onMusicChanged(const std::string &filename);

 private:
    Ui::MapPropsComponent ui;
    MainForm_GLComponent *m_glComponent;
    std::string m_resourcesPath;
    void onPushButtonApplySizeChangeClick();
    void onPushButtonOpenMusicFileClick();
    void onPushButtonClearMusicClick();
};
