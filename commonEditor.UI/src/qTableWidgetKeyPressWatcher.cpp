#include "qTableWidgetKeyPressWatcher.h"

bool QTableWidgetKeyPressWatcher::eventFilter(QObject * receiver, QEvent * event) 
{
  auto table = qobject_cast<QTableWidget*>(receiver);
  if (table && event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent*>(event);
      emit keyPressed(keyEvent->key(), table->currentRow(), table->currentColumn());
  }
  return false;
}

void QTableWidgetKeyPressWatcher::installOn(QTableWidget * widget)
{
  widget->installEventFilter(this);
}