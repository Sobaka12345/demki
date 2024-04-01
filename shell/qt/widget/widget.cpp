#include "widget.hpp"

namespace shell::qt {

Widget::Widget(QWidget* parent)
    : QWidget(parent)
{}

bool Widget::prepare(renderer::OperationContext& context)
{
    return window()->prepare(context);
}

void Widget::present(renderer::OperationContext& context)
{
    return window()->present(context);
}

uint32_t Widget::width() const
{
    return window()->width();
}

uint32_t Widget::height() const
{
    return window()->height();
}

std::pair<int, int> Widget::framebufferSize() const
{
    return window()->framebufferSize();
}

bool Widget::available() const
{
    return true;
}

void Widget::registerCursorPosCallback(std::function<void(double, double)> callback) const
{
    return window()->registerCursorPosCallback(callback);
}

void Widget::registerFramebufferResizeCallback(std::function<void(int, int)> callback) const
{
    return window()->registerFramebufferResizeCallback(callback);
}

void Widget::accept(renderer::RenderInfoVisitor& visitor) const
{
    return window()->accept(visitor);
}

renderer::IGraphicsContext& Widget::graphicsContext()
{
    return window()->graphicsContext();
}

}    //  namespace shell::qt
