#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include "constants.hpp"
#include "mainForm.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::UIUtils;
namespace po = boost::program_options;

void printAppHeader(const std::string &orgName,
        const std::string &appName);

int main(int argc, char *argv[]) {
    po::options_description desc("Usage : GameDataEditor [options]");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "display the version");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        printAppHeader(ORGANIZATIONNAME, APPLICATIONNAME);
        std::cout << '\n' << desc << "\n";
        return 0;
    }

    if (vm.count("version")) {
        printAppHeader(ORGANIZATIONNAME, APPLICATIONNAME);
        std::cout << "Version: " << PROJECT_VERSION << "\n";
        return 0;
    }
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
    QCoreApplication::setApplicationName(APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QGuiApplication::setDesktopFileName("thewarrior.gamedataeditor.desktop");
    MainForm mainWin(nullptr);
    UIUtils::centerToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}

void printAppHeader(const std::string &orgName,
        const std::string &appName) {
    std::cout << orgName << '\n';
    std::cout << appName << '\n';
}
