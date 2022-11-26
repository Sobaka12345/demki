#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "renderer/GraphicalApplication.hpp"

int main() {
    auto& app = GraphicalApplication::instance();
    app.setWindowSize(600, 600);

    return app.exec();
}
