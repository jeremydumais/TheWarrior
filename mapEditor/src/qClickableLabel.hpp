#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QtWidgets>

class QClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QClickableLabel(QWidget *parent = 0);
    ~QClickableLabel();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void onMouseReleaseEvent(QMouseEvent *event);
};

#endif // QCLICKABLELABEL_H