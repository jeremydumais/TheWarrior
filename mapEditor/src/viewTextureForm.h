
#ifndef VIEWTEXTUREFORM_H
#define VIEWTEXTUREFORM_H

#include "ui_viewTextureForm.h"

class ViewTextureForm : public QDialog
{
Q_OBJECT

public:
	explicit ViewTextureForm(QWidget *parent = 0);
	~ViewTextureForm() = default;
private:
	Ui::viewTextureFormClass ui;
private slots:
};

#endif // VIEWTEXTUREFORM_H
