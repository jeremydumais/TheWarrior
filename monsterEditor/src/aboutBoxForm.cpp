#include "aboutBoxForm.hpp"
#include <fmt/format.h>
#include "imageAttributionsForm.hpp"

AboutBoxForm::AboutBoxForm(QWidget *parent)
    : QDialog(parent),
    ui(Ui::aboutBoxFormClass()) {
    ui.setupUi(this);
    connect(ui.pushButtonImageAttributions, &QPushButton::clicked, this, &AboutBoxForm::pushButtonImageAttributions_Click);
    setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
    ui.labelVersion->setText(fmt::format("Version {0}", PROJECT_VERSION).c_str());
}

void AboutBoxForm::pushButtonImageAttributions_Click() {
    ImageAttributionsForm formImageAttributions(this);
    formImageAttributions.exec();
}
