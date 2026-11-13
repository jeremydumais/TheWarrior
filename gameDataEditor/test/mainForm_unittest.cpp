#include "mainForm.hpp"
#include "gameDataStorage.hpp"
#include <gtest/gtest.h>
#include <QApplication>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTemporaryDir>
#include <QTimer>
#include <filesystem>

class MainFormUnsavedChanges : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(resources.isValid());
        ASSERT_TRUE(config.isValid());
        window = std::make_unique<MainForm>(nullptr, resources.path().toStdString(),
                                            config.path().toStdString());
        window->show();
    }

    void addItem() {
        QTimer::singleShot(0, window.get(), [this] {
            auto chooser = QApplication::activeModalWidget();
            ASSERT_NE(nullptr, chooser);
            QTimer::singleShot(0, window.get(), [] {
                auto editor = QApplication::activeModalWidget();
                ASSERT_NE(nullptr, editor);
                editor->findChild<QLineEdit *>("lineEditId")->setText("ite001");
                editor->findChild<QLineEdit *>("lineEditName")->setText("Test item");
                editor->findChild<QLineEdit *>("lineEditTextureName")->setText("ItemsTile");
                editor->findChild<QPushButton *>("pushButtonOK")->click();
            });
            chooser->findChild<QPushButton *>("pushButtonAddItem")->click();
        });
        window->findChild<QAction *>("action_AddItem")->trigger();
        ASSERT_TRUE(window->isWindowModified());
        EXPECT_TRUE(window->windowTitle().contains("[*]"));
    }

    void answerClosePrompt(QMessageBox::StandardButton answer) {
        QTimer::singleShot(0, window.get(), [answer] {
            auto message = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
            ASSERT_NE(nullptr, message);
            EXPECT_EQ(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                      message->standardButtons());
            EXPECT_EQ(message->button(QMessageBox::Cancel), message->defaultButton());
            message->button(answer)->click();
        });
    }

    void selectItem() {
        auto categories = window->findChild<QTableWidget *>("tableWidgetItemCategories");
        ASSERT_EQ(1, categories->rowCount());
        categories->selectRow(0);
        auto items = window->findChild<QTableWidget *>("tableWidgetItems");
        ASSERT_EQ(1, items->rowCount());
        items->selectRow(0);
    }

    std::shared_ptr<thewarrior::models::GameData> readSavedData() {
        auto data = std::make_shared<thewarrior::models::GameData>();
        thewarrior::storage::GameDataStorage storage;
        storage.loadGameData((resources.path() + "/gamedata/gameData.gdt").toStdString(), data);
        return data;
    }

    QTemporaryDir resources;
    QTemporaryDir config;
    std::unique_ptr<MainForm> window;
};

TEST_F(MainFormUnsavedChanges, CleanWindowClosesWithoutPrompt) {
    EXPECT_FALSE(window->isWindowModified());
    EXPECT_TRUE(window->close());
}

TEST_F(MainFormUnsavedChanges, CancelledAddDoesNotMarkWindowDirty) {
    QTimer::singleShot(0, window.get(), [] {
        auto dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
        ASSERT_NE(nullptr, dialog);
        dialog->reject();
    });
    window->findChild<QAction *>("action_AddItem")->trigger();
    EXPECT_FALSE(window->isWindowModified());
    EXPECT_TRUE(window->close());
}

TEST_F(MainFormUnsavedChanges, SaveActionPersistsChangesAndClearsTitleMarker) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    window->findChild<QAction *>("action_Save")->trigger();
    EXPECT_FALSE(window->isWindowModified());
    EXPECT_NE(nullptr, readSavedData()->getItemStore().findItem("ite001"));
    EXPECT_TRUE(window->close());
}

TEST_F(MainFormUnsavedChanges, CancelKeepsDirtyWindowOpen) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    answerClosePrompt(QMessageBox::Cancel);
    EXPECT_FALSE(window->close());
    EXPECT_TRUE(window->isVisible());
    EXPECT_TRUE(window->isWindowModified());
    EXPECT_EQ(0, readSavedData()->getItemStore().getItemCount());
}

TEST_F(MainFormUnsavedChanges, EditingWithoutChangesKeepsWindowClean) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    window->findChild<QAction *>("action_Save")->trigger();
    ASSERT_NO_FATAL_FAILURE(selectItem());
    QTimer::singleShot(0, window.get(), [] {
        auto editor = QApplication::activeModalWidget();
        ASSERT_NE(nullptr, editor);
        editor->findChild<QPushButton *>("pushButtonOK")->click();
    });
    window->findChild<QAction *>("action_EditItem")->trigger();
    EXPECT_FALSE(window->isWindowModified());
}

TEST_F(MainFormUnsavedChanges, EditedItemMarksWindowDirty) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    window->findChild<QAction *>("action_Save")->trigger();
    ASSERT_NO_FATAL_FAILURE(selectItem());
    QTimer::singleShot(0, window.get(), [] {
        auto editor = QApplication::activeModalWidget();
        ASSERT_NE(nullptr, editor);
        editor->findChild<QLineEdit *>("lineEditName")->setText("Changed item");
        editor->findChild<QPushButton *>("pushButtonOK")->click();
    });
    window->findChild<QAction *>("action_EditItem")->trigger();
    EXPECT_TRUE(window->isWindowModified());
    window->findChild<QAction *>("action_Save")->trigger();
    EXPECT_FALSE(window->isWindowModified());
    const auto data = readSavedData();
    ASSERT_NE(nullptr, data->getItemStore().findItem("ite001"));
    EXPECT_EQ("Changed item", data->getItemStore().findItem("ite001")->getName());
}

TEST_F(MainFormUnsavedChanges, DeletedItemMarksWindowDirty) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    window->findChild<QAction *>("action_Save")->trigger();
    ASSERT_NO_FATAL_FAILURE(selectItem());
    QTimer::singleShot(0, window.get(), [] {
        auto prompt = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
        ASSERT_NE(nullptr, prompt);
        prompt->button(QMessageBox::Yes)->click();
    });
    window->findChild<QAction *>("action_DeleteItem")->trigger();
    EXPECT_TRUE(window->isWindowModified());
    window->findChild<QAction *>("action_Save")->trigger();
    EXPECT_FALSE(window->isWindowModified());
    EXPECT_EQ(0, readSavedData()->getItemStore().getItemCount());
}

TEST_F(MainFormUnsavedChanges, DiscardClosesWithoutSaving) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    answerClosePrompt(QMessageBox::Discard);
    EXPECT_TRUE(window->close());
    EXPECT_EQ(0, readSavedData()->getItemStore().getItemCount());
}

TEST_F(MainFormUnsavedChanges, SaveOnClosePersistsChangesAndCloses) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    answerClosePrompt(QMessageBox::Save);
    EXPECT_TRUE(window->close());
    EXPECT_FALSE(window->isWindowModified());
    EXPECT_NE(nullptr, readSavedData()->getItemStore().findItem("ite001"));
}

TEST_F(MainFormUnsavedChanges, FailedSaveOnCloseKeepsDirtyWindowOpen) {
    ASSERT_NO_FATAL_FAILURE(addItem());
    const auto path = std::filesystem::path(resources.path().toStdString()) / "gamedata/gameData.gdt";
    ASSERT_TRUE(std::filesystem::remove(path));
    ASSERT_TRUE(std::filesystem::create_directory(path));
    QTimer::singleShot(0, window.get(), [this] {
        auto prompt = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
        ASSERT_NE(nullptr, prompt);
        QTimer::singleShot(0, window.get(), [] {
            auto error = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
            ASSERT_NE(nullptr, error);
            EXPECT_EQ(QMessageBox::Critical, error->icon());
            error->accept();
        });
        prompt->button(QMessageBox::Save)->click();
    });
    EXPECT_FALSE(window->close());
    EXPECT_TRUE(window->isVisible());
    EXPECT_TRUE(window->isWindowModified());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    return RUN_ALL_TESTS();
}
