#pragma once

#include "abstract_application.hpp"

#include <qt/window/window.hpp>

#include <QApplication>

namespace engine {

class QtApplication
    : public QGuiApplication
    , public AbstractApplication
{
public:
    QtApplication(int& argc, char** argv);
    ~QtApplication();

    virtual int exec() override;
    virtual shell::qt::Window& window() override;

private:
    using QGuiApplication::exec;

private:
    std::unique_ptr<shell::qt::Window> m_mainWindow;
};

}    //  namespace engine
