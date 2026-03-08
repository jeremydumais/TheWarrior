#include <QtWidgets/qmessagebox.h>
#include <string>
#include "warningMessage.hpp"

namespace commoneditor::ui {

void WarningMessage::show(const std::string &message,
        const std::string &internalWarning) {
    QMessageBox msgBox;
    msgBox.setText(message.c_str());
    if (!internalWarning.empty()) {
        msgBox.setInformativeText(internalWarning.c_str());
    }
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Warning");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

}  // namespace commoneditor::ui
