#pragma once

#include <cstdint>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Position
{
    int32_t x, y;

    Position() : x(s_invalidPosition), y(s_invalidPosition) {};
    Position(int32_t x, int32_t y) : x(x), y(y) {};

    bool isValid() { return x != s_invalidPosition && y != s_invalidPosition; }
    void setInvalid() { x = s_invalidPosition; y = s_invalidPosition; }

    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y;
    }

    glm::mat4 position3D() const
    {
        return glm::translate(
            glm::mat4x4(1.0f),
            glm::vec3(x, y, 0)
        );
    }

private:
    static constexpr int32_t s_invalidPosition = std::numeric_limits<int32_t>::min();
};
