#include "mainForm.hpp"
#include "../../mapEditor/src/utils/uiUtils.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior - Item Editor");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainForm mainWin;
    UIUtils::CenterToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}