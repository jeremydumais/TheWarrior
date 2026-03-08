#include <QtWidgets/qmessagebox.h>
#include <string>
#include "errorMessage.hpp"

namespace commoneditor::ui {

void ErrorMessage::show(const std::string &message,
        const std::string &internalError) {
    QMessageBox msgBox;
    msgBox.setText(message.c_str());
    if (!internalError.empty()) {
        msgBox.setInformativeText(internalError.c_str());
    }
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Error");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

}  // namespace commoneditor::ui
