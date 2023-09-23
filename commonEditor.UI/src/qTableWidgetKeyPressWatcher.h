#pragma once

#include <QKeyEvent>
#include <QtCore/QObject>
#include <QtWidgets/QTableWidget>

class QTableWidgetKeyPressWatcher : public QObject {
public:
  using QObject::QObject;
  Q_SIGNAL void keyPressed(int key, int row, int column);
  void installOn(QTableWidget * widget);
private:
  Q_OBJECT
  bool eventFilter(QObject * receiver, QEvent * event) override;
};