#ifndef QCLOSABLEDOCKWIDGET_H
#define QCLOSABLEDOCKWIDGET_H

#include <QtWidgets>
#include <qevent.h>

class QClosableDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit QClosableDockWidget(QWidget *parent = nullptr);
    ~QClosableDockWidget() override;
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void onCloseEvent(QCloseEvent *event);
};

#endif // QCLOSABLEDOCKWIDGET_H
