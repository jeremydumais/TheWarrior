#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include "mainForm.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::UIUtils;
namespace po = boost::program_options;

void printAppHeader(const std::string &orgName,
        const std::string &appName);

int main(int argc, char *argv[]) {
    const auto ORGANIZATIONNAME = "Jed# Software";
    const auto APPLICATIONNAME = "The Warrior - Monster Editor";
    po::options_description desc("Usage : MonsterEditor [options]");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "display the version")
        ("input-file", po::value<std::string>(), "input file");

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
    std::string inputFile = "";
    if (vm.count("input-file")) {
       inputFile = vm["input-file"].as<std::string>();
    }
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Jed# Software");
    QCoreApplication::setApplicationName("The Warrior - Monster Editor");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QGuiApplication::setDesktopFileName("thewarrior.monstereditor.desktop");
    MainForm mainWin(nullptr, inputFile);
    UIUtils::centerToScreen(&mainWin);
    mainWin.showMaximized();
    return QApplication::exec();
}

void printAppHeader(const std::string &orgName,
        const std::string &appName) {
    std::cout << orgName << '\n';
    std::cout << appName << '\n';
}
