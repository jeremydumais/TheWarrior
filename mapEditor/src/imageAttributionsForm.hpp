
#ifndef IMAGEATTRIBUTIONSFORM_H
#define IMAGEATTRIBUTIONSFORM_H

#include "ui_imageAttributionsForm.h"

class ImageAttributionsForm : public QDialog
{
Q_OBJECT

public:
	explicit ImageAttributionsForm(QWidget *parent = 0);
	~ImageAttributionsForm() = default;
private:
	Ui::imageAttributionsFormClass ui;
};

#endif // IMAGEATTRIBUTIONSFORM_H
