#pragma once

#include "abstract_application.hpp"

namespace engine {

class GraphicalApplication : public AbstractApplication
{
public:
    GraphicalApplication(int& argc, char** argv);
    virtual ~GraphicalApplication();

    virtual int exec() override;

private:
    int mainLoop();
};

}    //  namespace engine
