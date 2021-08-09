#pragma once

#include "mainForm_GLComponent.hpp"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

struct MainForm_MapTabComponent_Objects
{
    MainForm_GLComponent *glComponent = nullptr;
    QLineEdit *lineEditMapWidth = nullptr;
    QLineEdit *lineEditMapHeight = nullptr;
    QSpinBox *spinBoxMapSizeTop = nullptr;
    QSpinBox *spinBoxMapSizeLeft = nullptr;
    QSpinBox *spinBoxMapSizeRight = nullptr;
    QSpinBox *spinBoxMapSizeBottom = nullptr;
    QPushButton *pushButtonApplySizeChange = nullptr;
};

class MainForm_MapTabComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_MapTabComponent();
    void initializeUIObjects(const MainForm_MapTabComponent_Objects &objects);
    void connectUIActions();
signals:
private:
    MainForm_GLComponent *glComponent;
    QLineEdit *lineEditMapWidth;
    QLineEdit *lineEditMapHeight;
    QSpinBox *spinBoxMapSizeTop;
    QSpinBox *spinBoxMapSizeLeft;
    QSpinBox *spinBoxMapSizeRight;
    QSpinBox *spinBoxMapSizeBottom;
    QPushButton *pushButtonApplySizeChange;
	void onPushButtonApplySizeChangeClick();
};