#include "mainForm_MapTabComponent.hpp"
#include "errorMessage.hpp"

using namespace std;

MainForm_MapTabComponent::MainForm_MapTabComponent() 
    : glComponent(nullptr),
      lineEditMapWidth(nullptr),
      lineEditMapHeight(nullptr),
      spinBoxMapSizeTop(nullptr),
      spinBoxMapSizeLeft(nullptr),
      spinBoxMapSizeRight(nullptr),
      spinBoxMapSizeBottom(nullptr),
      pushButtonApplySizeChange(nullptr)
{
}

void MainForm_MapTabComponent::initializeUIObjects(const MainForm_MapTabComponent_Objects &objects) 
{
    this->glComponent = objects.glComponent;
    this->lineEditMapWidth = objects.lineEditMapWidth;
    this->lineEditMapHeight = objects.lineEditMapHeight;
    this->spinBoxMapSizeTop = objects.spinBoxMapSizeTop;
    this->spinBoxMapSizeLeft = objects.spinBoxMapSizeLeft;
    this->spinBoxMapSizeRight = objects.spinBoxMapSizeRight;
    this->spinBoxMapSizeBottom = objects.spinBoxMapSizeBottom;
    this->pushButtonApplySizeChange = objects.pushButtonApplySizeChange;
}

void MainForm_MapTabComponent::connectUIActions() 
{
	connect(pushButtonApplySizeChange, &QPushButton::clicked, this, &MainForm_MapTabComponent::onPushButtonApplySizeChangeClick);
}

void MainForm_MapTabComponent::onPushButtonApplySizeChangeClick() 
{
	int offsetLeft { spinBoxMapSizeLeft->value() };
	int offsetTop { spinBoxMapSizeTop->value() };
	int offsetRight { spinBoxMapSizeRight->value() };
	int offsetBottom { spinBoxMapSizeBottom->value() };
	if (offsetLeft < 0 || 
		offsetTop < 0 ||
		offsetRight < 0 ||
		offsetBottom < 0) {	
		//Check if there's tiles that are already assigned in the ones we will remove
		if (glComponent->isShrinkMapImpactAssignedTiles(offsetLeft,
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
		glComponent->resizeMap(offsetLeft,
							   offsetTop,
							   offsetRight,
							   offsetBottom);
	}
	catch(invalid_argument &err) {
		ErrorMessage::show(err.what());
	}
}
