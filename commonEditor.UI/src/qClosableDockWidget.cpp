#include "qClosableDockWidget.hpp"
#include <qdockwidget.h>

QClosableDockWidget::QClosableDockWidget(QWidget *parent)
    : QDockWidget(parent) {
}

QClosableDockWidget::~QClosableDockWidget() {
}

void QClosableDockWidget::closeEvent(QCloseEvent *event) {
    emit onCloseEvent(event);
}
