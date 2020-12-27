#ifndef MAINFORM_H
#define MAINFORM_H

#include "gameMap.hpp"
#include "selectionMode.hpp"
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
	SelectionMode selectionMode;
	std::shared_ptr<GameMap> map;
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
						  const std::string &internalError) const;
	void setAppStylesheet(const std::string &style);
	void connectUIActions();
	void action_About_Click();
	void action_LightTheme_Click();
	void action_DarkTheme_Click();
	void action_SelectClick();
	void action_MoveMapClick();
	void action_ApplyTextureClick();
	void action_ApplyObjectClick();
	void mapPaint(QPaintEvent *e);
	void resizeEvent(QResizeEvent *);
	void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> selectedTileIndexes);
    void onTileMouseMoveEvent(bool mousePressed, int tileIndex);
	void onPushButtonAddTextureClick();
	void onPushButtonEditTextureClick();
	void onPushButtonDeleteTextureClick();
	std::vector<std::string> getAlreadyUsedTextureNames() const;
	boost::optional<const Texture &> getSelectedTextureInTextureList();
	void refreshTextureList();
	void onPushButtonSelectedTextureClearClick();
	void onPushButtonSelectedObjectClearClick();
	void onLabelImageTextureMouseReleaseEvent(QMouseEvent *event);
	int getTextureIndexFromPosition(const QPoint &pos, const Texture &texture);
	void onLineEditTexIndexTextChange(const QString &text);
	QPixmap getTextureTileImageFromTexture(int tileIndex, const Texture &texture) const;
};

#endif // MAINFORM_H

