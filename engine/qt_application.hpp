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

    virtual int run() override;
    virtual shell::qt::Window& window() override;

private:
    std::unique_ptr<shell::qt::Window> m_mainWindow;
};

}    //  namespace engine
