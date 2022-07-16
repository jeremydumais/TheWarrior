#include "mainForm.hpp"
#include "uiUtils.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior - Monster Editor");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainForm mainWin;
    UIUtils::centerToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}