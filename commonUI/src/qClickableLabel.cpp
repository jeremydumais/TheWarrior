#include "qClickableLabel.hpp"

QClickableLabel::QClickableLabel(QWidget *parent)
    : QLabel(parent)
{
}
QClickableLabel::~QClickableLabel()
{
}

void QClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    emit onMouseReleaseEvent(event);
}