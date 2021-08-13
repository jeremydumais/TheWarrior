#include "mainForm.hpp"
#include "utils/uiUtils.hpp"
#include <GL/glut.h>

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainForm mainWin;
    UIUtils::CenterToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}