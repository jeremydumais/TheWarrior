#include "mainForm.hpp"
#include <GL/glut.h>

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainForm mainWin;
    mainWin.show();
    return QApplication::exec();
}