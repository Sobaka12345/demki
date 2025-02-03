#pragma once

#include <iwindow.hpp>

#include <QWindow>

namespace shell::qt {

class Window
    : public QWindow
    , virtual public IWindow
{
    Q_OBJECT

public:
    Window(int width, int height, std::string name, QWindow* parent = nullptr);
    ~Window();

    virtual bool event(QEvent* e) override;

    virtual bool eventFilter(QObject* watched, QEvent* event) override;
    virtual void exposeEvent(QExposeEvent*) override;

    virtual bool available() const override;

    virtual std::string name() const override;
    virtual void close() override;
    virtual std::pair<int, int> framebufferSize() const override;
    virtual uint32_t width() const override;
    virtual uint32_t height() const override;
    virtual bool iconified() const override;
    virtual void registerCursorPosCallback(
        std::function<void(double, double)> callback) const override;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const override;
    virtual void registerWindowIconifiedCallback(std::function<void(bool)> callback) const override;
    virtual void registerOnKeyPressedCallback(
        std::function<void(int, int, int, int)> callback) const override;

signals:
    void render();
    void aboutToClose();

private:
    mutable std::vector<std::function<void(double xPos, double yPos)>> m_cursorPosCallback;
    mutable std::vector<std::function<void(int, int)>> m_framebufferResizeCallbacks;
    mutable std::vector<std::function<void(bool)>> m_windowIconifiedCallbacks;
    mutable std::vector<std::function<void(int key, int scancode, int action, int mods)>>
        m_onKeyPressedCallback;
};

}    //  namespace shell::qt
