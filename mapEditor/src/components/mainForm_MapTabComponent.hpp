#pragma once

#include "mainForm_GLComponent.hpp"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

struct MainForm_MapTabComponent_Objects {
    MainForm_GLComponent *glComponent = nullptr;
    QLineEdit *lineEditMapWidth = nullptr;
    QLineEdit *lineEditMapHeight = nullptr;
    QSpinBox *spinBoxMapSizeTop = nullptr;
    QSpinBox *spinBoxMapSizeLeft = nullptr;
    QSpinBox *spinBoxMapSizeRight = nullptr;
    QSpinBox *spinBoxMapSizeBottom = nullptr;
    QPushButton *pushButtonApplySizeChange = nullptr;
};

class MainForm_MapTabComponent : public QWidget {
Q_OBJECT
 public:
    MainForm_MapTabComponent();
    void initializeUIObjects(const MainForm_MapTabComponent_Objects &objects);
    void connectUIActions();
    void reset();

 private:
    MainForm_GLComponent *m_glComponent;
    QLineEdit *m_lineEditMapWidth;
    QLineEdit *m_lineEditMapHeight;
    QSpinBox *m_spinBoxMapSizeTop;
    QSpinBox *m_spinBoxMapSizeLeft;
    QSpinBox *m_spinBoxMapSizeRight;
    QSpinBox *m_spinBoxMapSizeBottom;
    QPushButton *m_pushButtonApplySizeChange;
    void onPushButtonApplySizeChangeClick();
};
