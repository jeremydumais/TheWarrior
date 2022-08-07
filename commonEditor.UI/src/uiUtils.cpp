#include "uiUtils.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>


namespace commoneditor::ui {

void UIUtils::centerToScreen(QWidget* widget)
{
  if (!widget)
    return;
  auto *screen = QGuiApplication::screenAt(QCursor::pos());
  int desk_x = screen->size().width();
  int desk_y = screen->size().height();
  int x = widget->width();
  int y = widget->height();
  widget->move(desk_x / 2 - x / 2 + screen->geometry().left(), desk_y / 2 - y / 2 + screen->geometry().top());
}

} // namespace commoneditor::ui
