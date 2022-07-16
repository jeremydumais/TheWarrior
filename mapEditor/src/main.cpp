#include "mainForm.hpp"
#include "uiUtils.hpp"
#include <GL/glut.h>

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	QGuiApplication::setDesktopFileName("thewarrior.mapeditor.desktop");
    MainForm mainWin;
    UIUtils::centerToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}