#include "mainForm_MapTabComponent.hpp"
#include "errorMessage.hpp"

using namespace std;
using namespace commoneditor::ui;

MainForm_MapTabComponent::MainForm_MapTabComponent()
    : m_glComponent(nullptr),
    m_lineEditMapWidth(nullptr),
    m_lineEditMapHeight(nullptr),
    m_spinBoxMapSizeTop(nullptr),
    m_spinBoxMapSizeLeft(nullptr),
    m_spinBoxMapSizeRight(nullptr),
    m_spinBoxMapSizeBottom(nullptr),
    m_pushButtonApplySizeChange(nullptr)
{
}

void MainForm_MapTabComponent::initializeUIObjects(const MainForm_MapTabComponent_Objects &objects)
{
    this->m_glComponent = objects.glComponent;
    this->m_lineEditMapWidth = objects.lineEditMapWidth;
    this->m_lineEditMapHeight = objects.lineEditMapHeight;
    this->m_spinBoxMapSizeTop = objects.spinBoxMapSizeTop;
    this->m_spinBoxMapSizeLeft = objects.spinBoxMapSizeLeft;
    this->m_spinBoxMapSizeRight = objects.spinBoxMapSizeRight;
    this->m_spinBoxMapSizeBottom = objects.spinBoxMapSizeBottom;
    this->m_pushButtonApplySizeChange = objects.pushButtonApplySizeChange;
}

void MainForm_MapTabComponent::connectUIActions()
{
    connect(m_pushButtonApplySizeChange, &QPushButton::clicked, this, &MainForm_MapTabComponent::onPushButtonApplySizeChangeClick);
}

void MainForm_MapTabComponent::reset()
{
    this->m_lineEditMapWidth->setText(to_string(m_glComponent->getMapWidth()).c_str());
    this->m_lineEditMapHeight->setText(to_string(m_glComponent->getMapHeight()).c_str());
    this->m_spinBoxMapSizeLeft->setValue(0);
    this->m_spinBoxMapSizeTop->setValue(0);
    this->m_spinBoxMapSizeRight->setValue(0);
    this->m_spinBoxMapSizeBottom->setValue(0);
}

void MainForm_MapTabComponent::onPushButtonApplySizeChangeClick()
{
    int offsetLeft { m_spinBoxMapSizeLeft->value() };
    int offsetTop { m_spinBoxMapSizeTop->value() };
    int offsetRight { m_spinBoxMapSizeRight->value() };
    int offsetBottom { m_spinBoxMapSizeBottom->value() };
    if (offsetLeft < 0 ||
            offsetTop < 0 ||
            offsetRight < 0 ||
            offsetBottom < 0) {
        //Check if there's tiles that are already assigned in the ones we will remove
        if (m_glComponent->isShrinkMapImpactAssignedTiles(offsetLeft,
                    offsetTop,
                    offsetRight,
                    offsetBottom)) {
            ErrorMessage::show("Cannot resize the map because some tile are\n"
                    "already assigned in the ones you try to remove.");
            return;
        }
    }
    //Apply new size
    try {
        m_glComponent->resizeMap(offsetLeft,
                offsetTop,
                offsetRight,
                offsetBottom);
        reset();

    }
    catch(invalid_argument &err) {
        ErrorMessage::show(err.what());
    }
}
