
#ifndef ABOUTBOXFORM_H
#define ABOUTBOXFORM_H

#include "ui_aboutBoxForm.h"

class AboutBoxForm : public QDialog
{
Q_OBJECT

public:
    explicit AboutBoxForm(QWidget *parent = 0);
    ~AboutBoxForm() = default;
private:
    Ui::aboutBoxFormClass ui;
private slots:
    void pushButtonImageAttributions_Click();
};

#endif // ABOUTBOXFORM_H
