#pragma once

#include "../window/window.hpp"

#include <isurface.hpp>

#include <QWidget>

namespace shell::qt {

class Widget
    : public QWidget
    , public renderer::ISurface
{
public:
    Widget(QWidget* parent);

public:
    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual uint32_t width() const override;
    virtual uint32_t height() const override;

    virtual std::pair<int, int> framebufferSize() const override;
    virtual bool available() const override;
    virtual void registerCursorPosCallback(
        std::function<void(double, double)> callback) const override;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const override;

    virtual void accept(renderer::RenderInfoVisitor& visitor) const override;

    virtual renderer::IGraphicsContext& graphicsContext() override;

private:
    virtual Window* window() = 0;
    virtual const Window* window() const = 0;

protected:
    QWidget* m_windowContainer;
};

}    //  namespace shell::qt
