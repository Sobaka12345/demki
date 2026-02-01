#include "application.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <vulkan/vulkan_core.h>

namespace shell::glfw {

std::vector<const char*> getRequiredVulkanExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
}

Application::Application(int& argc, char** argv)
    : abstract::Application<
        shell::abstract::vk::Application,
        shell::abstract::ogl::Application
    >(argc, argv)
{
    glfwInit();
}

Application::~Application()
{
    glfwTerminate();
}


void Application::init() {
    getRequiredVulkanExtensions();
}

int Application::destroy(int code) {
    return code;
}

// int Application::mainLoop()
// {
//     auto start = std::chrono::steady_clock::now();
//     while (!m_mainWindow->shouldClose())
//     {
//         auto end = std::chrono::steady_clock::now();
//         const auto dt =
//             std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start)
//                 .count();
//         if (!m_fpsCap.timePassed(dt)) {
//             std::this_thread::sleep_for(std::chrono::duration<int64_t, TimeResolution>(m_fpsCap.timeLeft()));
//             continue;
//         };
//         update(dt);
//         perform();
//         start = end;
//         glfwPollEvents();
//     }

//     context().waitIdle();

//     return 0;
// }

}    //  namespace engine
