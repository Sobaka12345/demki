#pragma once

#include "abstract_application.hpp"

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

private:
    using QGuiApplication::exec;
};

}
