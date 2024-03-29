#ifndef MONSTEREDITOR_SRC_MAINFORM_HPP_
#define MONSTEREDITOR_SRC_MAINFORM_HPP_

#include <string>
#include "mainController.hpp"
#include "ui_mainForm.h"
#include "qTableWidgetKeyPressWatcher.h"

class MainForm : public QMainWindow {
Q_OBJECT

 public:
    explicit MainForm(QWidget *parent = nullptr,
            const std::string &currentFilePath = "");
    ~MainForm() override = default;
    void connectUIActions();
    bool event(QEvent *event) override;
    void functionAfterShown();

 private:
    Ui::MainForm ui;
    bool m_functionAfterShownCalled;
    std::string m_userConfigFolder;
    std::string m_executablePath;
    std::string m_resourcesPath;
    std::string m_currentFilePath;
    monstereditor::controllers::MainController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetMonstersKeyWatcher;
    static const std::string THEME_PATH;
    static const std::string RECENT_DB;
    void initializeMonstersTableControl();
    void action_About_Click();
    void action_LightTheme_Click();
    void action_DarkTheme_Click();
    void action_OpenMonsterStore_Click();
    void action_OpenRecentMonstersDB_Click();
    void action_SaveMonsterStore_Click();
    void action_SaveAsMonsterStore_Click();
    void refreshRecentMapsMenu();
    void addNewRecentMonstersDB(const std::string &filePath);
    void refreshMonstersTable();
    void action_ManageTextures_Click();
    const std::string &getExecutablePath();
    const std::string &getResourcesPath();
    void setAppStylesheet(const std::string &style);
    void openMonsterStore(const std::string &filePath);
    void saveMonsterStore(const std::string &filePath);
    void refreshWindowTitle();
    void onPushButtonAddMonsterClick();
    void onPushButtonEditMonsterClick();
    void onPushButtonDeleteMonsterClick();
    void onTableWidgetMonstersDoubleClicked(QTableWidgetItem *item);
    void onTableWidgetMonstersKeyPressEvent(int key, int, int);
    std::optional<std::string> getSelectedItemId() const;
};

#endif  // MONSTEREDITOR_SRC_MAINFORM_HPP_

