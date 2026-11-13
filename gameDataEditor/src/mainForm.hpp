#ifndef GAMEDATAEDITOR_SRC_MAINFORM_HPP_
#define GAMEDATAEDITOR_SRC_MAINFORM_HPP_

#include <memory>
#include <optional>
#include <string>
#include "mainController.hpp"
#include "ui_mainForm.h"
#include "qTableWidgetKeyPressWatcher.h"

class MainForm : public QMainWindow {
Q_OBJECT

 public:
    explicit MainForm(QWidget *parent = nullptr, const std::string &resourcesPath = "",
                      const std::string &configFolder = "");
    ~MainForm() override = default;
    void connectUIActions();
    bool event(QEvent *event) override;
    void functionAfterShown();

 protected:
    void closeEvent(QCloseEvent *event) override;

 private:
    Ui::MainForm ui;
    bool m_functionAfterShownCalled;
    std::string m_userConfigFolder;
    std::string m_executablePath;
    std::string m_resourcesPath;
    gamedataeditor::controllers::MainController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetItemsKeyWatcher;
    static const std::string THEME_PATH;
    static const std::string RECENT_MAPS;
    void updateSectionActions();
    void initializeCategoriesTableControl();
    void initializeItemsTableControl(std::optional<thewarrior::models::ItemType> itemType);
    void action_About_Click();
    void action_LightTheme_Click();
    void action_DarkTheme_Click();
    void action_SaveItemStore_Click();
    bool saveGameData();
    void refreshCategoriesTable();
    void refreshItemsTable();
    QIcon getCategoryIcon(const std::string &categoryName) const;
    void action_ManageItemsTexture_Click();
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
    void setAppStylesheet(const std::string &style);
    void refreshWindowTitle();
    void onTableWidgetItemCategoriesSelectionChanged();
    void onTableWidgetItemsDoubleClicked(QTableWidgetItem *item);
    void onTableWidgetItemsKeyPressEvent(int key, int, int);
    void onPushButtonAddItemClick();
    void onPushButtonEditItemClick();
    void onPushButtonDeleteItemClick();
    std::optional<std::string> getSelectedItemId() const;
    std::unique_ptr<QDialog> getItemTypeForm(thewarrior::models::ItemType itemType, std::optional<std::string> itemIdToEdit);
};

#endif  // GAMEDATAEDITOR_SRC_MAINFORM_HPP_

