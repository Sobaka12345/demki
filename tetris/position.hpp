#pragma once

#include <cstdint>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Position
{
    int32_t x, y;

    glm::mat4 position3D() const
    {
        return glm::translate(
            glm::mat4x4(1.0f),
            glm::vec3(x, y, 0)
        );
    }
};
