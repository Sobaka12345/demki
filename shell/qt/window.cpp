#include "window.hpp"

#include <assert.hpp>

#include <QWindow>
#include <QWindowStateChangeEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <qcoreapplication.h>
#include <qnamespace.h>

namespace shell::qt {

namespace __private {

BaseQWindow::BaseQWindow(int width, int height, std::string name, QWindow* parent)
    : QWindow(parent)
{
    setWidth(width);
    setHeight(height);
    setTitle(QString::fromStdString(name));

    connect(this, &QWindow::visibleChanged, 
        this, &BaseQWindow::aboutToOpen, Qt::SingleShotConnection); 

    installEventFilter(this);
}

BaseQWindow::~BaseQWindow() {}

bool BaseQWindow::event(QEvent* e)
{
    if (e->type() == QEvent::UpdateRequest)
    {
        emit render();
        requestUpdate();
        return true;
    }
    else if (e->type() == QEvent::Resize)
    {
        auto event = static_cast<QResizeEvent*>(e);
        emit sizeChanged(event->size().width(), event->size().height());
    }
    else if (e->type() == QEvent::WindowStateChange)
    {
        auto event = static_cast<QWindowStateChangeEvent*>(e);
        if (auto iconified = windowState() == Qt::WindowMinimized;
            iconified || event->oldState() == Qt::WindowMinimized)
            {
                emit iconifiedChanged(iconified);
            }
    }
    else if (e->type() == QEvent::KeyPress)
    {
        //  not implemented
    }
    else if (e->type() == QEvent::MouseMove)
    {
        // TO DO: create own qt events
        auto event = static_cast<QMouseEvent*>(e);
        auto pos = event->pos().toPointF();
        emit mousePosChanged(pos.x(), pos.y());
    }
    else if (e->type() == QEvent::Close)
    {
        emit aboutToClose();
    }

    return QWindow::event(e);
}

bool BaseQWindow::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::Resize && watched == this)
    {
        auto event = static_cast<QResizeEvent*>(e);
        sizeChanged(event->size().width(), event->size().height());
    }

    return QWindow::eventFilter(watched, e);
}

void BaseQWindow::exposeEvent(QExposeEvent*)
{
    requestUpdate();
}

void BaseQWindow::waitForEvents()
{
    QGuiApplication::processEvents();
}

}

}    //  namespace shell::qt
