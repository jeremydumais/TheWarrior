#pragma once

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "mainForm_GLComponent.hpp"
#include "ui_mapPropsComponent.h"

class MapPropsComponent : public QWidget {
Q_OBJECT
 public:
    MapPropsComponent(QWidget *parent,
            MainForm_GLComponent *glComponent);
    void connectUIActions();
    void reset();

 private:
    Ui::MapPropsComponent ui;
    MainForm_GLComponent *m_glComponent;
    void onPushButtonApplySizeChangeClick();
};
