#ifndef MAINFORM_H
#define MAINFORM_H

#include "gameMap.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include "mainController.hpp"
#include <memory>
#include <vector>
#include "ui_mainForm.h"

class MainForm : public QMainWindow
{
Q_OBJECT

public:
	explicit MainForm(QWidget *parent = 0);
	~MainForm();
	bool event(QEvent *event) override;
    void functionAfterShown();
private:
	Ui::MainForm ui;
	MainController controller;
	std::string userConfigFolder;
	SelectionMode selectionMode;
	std::string currentFilePath;
	MapTile *currentMapTile;
	std::string lastSelectedTextureName;
	std::string lastSelectedObjectName;
	int lastSelectedTextureIndex;
	int lastSelectedObjectIndex;
	bool functionAfterShownCalled;
	std::string executablePath;
	std::string resourcesPath;
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
	void showErrorMessage(const std::string &message,
						  const std::string &internalError = "") const;
	void setAppStylesheet(const std::string &style);
	void connectUIActions();
	void action_Open_Click();
	void action_OpenRecentMap_Click();
	void action_Save_Click();
	void action_SaveAs_Click();
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
	void action_DisplayGrid_Click();
	void action_SelectClick();
	void action_MoveMapClick();
	void action_ApplyTextureClick();
	void action_ApplyObjectClick();
	void action_EnableCanStepClick();
	void action_DisableCanStepClick();
	void openMap(const std::string &filePath);
	void saveMap(const std::string &filePath);
	void refreshWindowTitle();
	void refreshRecentMapsMenu();
	void addNewRecentMap(const std::string &filePath);
	void mapPaint(QPaintEvent *e);
	void resizeEvent(QResizeEvent *);
	void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> selectedTileIndexes);
    //void onTileMouseMoveEvent(bool mousePressed, int tileIndex);
	void onPushButtonApplySizeChangeClick();
	void onPushButtonAddTextureClick();
	void onPushButtonEditTextureClick();
	void onPushButtonDeleteTextureClick();
	boost::optional<const Texture &> getSelectedTextureInTextureList();
	void refreshTextureList();
	void displaySelectedTextureImage();
	void onPushButtonSelectedTextureClearClick();
	void onPushButtonSelectedObjectClearClick();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
	void onLineEditTexNameTextChanged(const QString &text);
	void onSpinBoxTexIndexValueChanged(int value);
	void onLineEditObjTexNameTextChanged(const QString &text);
	void onSpinBoxObjTexIndexValueChanged(int value);
	void onCheckBoxTileCanSteppedOnChanged(int state);
    void onComboBoxTextureCurrentIndexChanged();
	QPixmap getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const;
	Point to_Point(QPoint point);
};

#endif // MAINFORM_H

