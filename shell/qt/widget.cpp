#include "widget.hpp"

#include <assert.hpp>

#include <QWidget>

namespace shell::qt {

namespace __private {

BaseQWidget::BaseQWidget(QWidget* parent)
    : QWidget(parent)
{}

BaseQWidget::~BaseQWidget() {}

bool BaseQWidget::event(QEvent* e)
{
    return QWidget::event(e);
}

bool BaseQWidget::eventFilter(QObject* watched, QEvent* e)
{
    return QWidget::eventFilter(watched, e);
}

}

}    //  namespace shell::qt
