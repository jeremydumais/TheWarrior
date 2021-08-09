#include "uiUtils.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

void UIUtils::CenterToScreen(QWidget* widget) 
{
  if (!widget)
    return;
  QDesktopWidget* m = QApplication::desktop();
  QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
  int desk_x = desk_rect.width();
  int desk_y = desk_rect.height();
  int x = widget->width();
  int y = widget->height();
  widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}
