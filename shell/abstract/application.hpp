#pragma once

#include <gapi_context.hpp>

#include <ratio>

namespace shell::abstract {

template <typename ...ApplicationsT>
struct Application : public ApplicationsT...
{
    using TimeResolution = std::nano;

    Application(int argc, char** argv)
        : ApplicationsT(argc, argv)...
    {}

    virtual ~Application() {};

    virtual void init() {}
    virtual int destroy(int exitCode) { return exitCode; }

    [[nodiscard]] int run() {
        (ApplicationsT::init(), ...);
        init();
        return destroy(mainLoop());
    }

private:
    virtual int mainLoop() = 0;
};

}    //  namespace shell::abstract
