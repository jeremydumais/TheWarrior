#include "errorMessage.hpp"
#include <QtWidgets/qmessagebox.h>

void ErrorMessage::show(const std::string &message,
					    const std::string &internalError) 
{
    QMessageBox msgBox;
	msgBox.setText(message.c_str());
	if (internalError.length() > 0) {
		msgBox.setInformativeText(internalError.c_str());
	}
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Error");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}
