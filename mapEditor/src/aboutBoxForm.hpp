
#ifndef MAPEDITOR_SRC_ABOUTBOXFORM_HPP_
#define MAPEDITOR_SRC_ABOUTBOXFORM_HPP_

#include "ui_aboutBoxForm.h"

class AboutBoxForm : public QDialog {
Q_OBJECT

 public:
    explicit AboutBoxForm(QWidget *parent = nullptr);
    ~AboutBoxForm() override = default;

 private:
    Ui::aboutBoxFormClass ui;

 private slots:
        void pushButtonImageAttributions_Click();
};

#endif  // MAPEDITOR_SRC_ABOUTBOXFORM_HPP_
