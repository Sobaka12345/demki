#ifndef ABSTRACT_VK_APPLICATION_HPP
#define ABSTRACT_VK_APPLICATION_HPP

#include <gapi_context.hpp>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace shell::abstract::vk {

class Application 
{
public:
    #ifndef NDEBUG
    static constexpr auto s_validationEnabled = true;
    #else
    static constexpr auto s_validationEnabled = false;
    #endif

    static const std::vector<const char*> s_validationLayers;
    static const VkDebugUtilsMessengerCreateInfoEXT s_debugMessengerCreateInfo;

    explicit Application(int& argc, char **argv) noexcept;
    virtual ~Application();

    struct VkInstanceBuilder : VkInstanceCreateInfo
    {
        VkInstance build() noexcept;
        
    protected:
        VkInstanceBuilder(Application& app) noexcept;
        friend class Application;

    private:
        Application& app;
    };
    friend struct VkInstanceBuilder;
    virtual VkInstanceBuilder& vkInstanceBuilder() noexcept;

protected:
    void init() noexcept;

private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugUtilsMessenger;
};

}

#endif // ABSTRACT_VK_APPLICATION_HPP