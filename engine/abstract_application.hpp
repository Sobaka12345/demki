#pragma once

#include <iresources.hpp>
#include <irender_target.hpp>
#include <igraphics_context.hpp>

#include <memory>
#include <ratio>

namespace shell {
class IWindow;
}

namespace engine {

enum class GAPI
{
    OpenGL,
    Vulkan
};

class AbstractApplication
{
protected:
    using TimeResolution = std::nano;

public:
    struct CreateInfo
    {
        std::string windowName = "demki";
        int windowWidth = 640;
        int windowHeight = 480;
        GAPI gapi = GAPI::Vulkan;

        static CreateInfo readFromCmd(int argc, char** argv);
    };

public:
    AbstractApplication();
    virtual ~AbstractApplication();

    virtual shell::IWindow& window() = 0;


    renderer::IGraphicsContext& context();

    virtual int run() = 0;

protected:
    void init(shell::IWindow* window);

    virtual void update(int64_t dt) = 0;
    virtual void perform() = 0;

protected:
    std::unique_ptr<shell::IResources> m_resources;
};

}    //  namespace engine
