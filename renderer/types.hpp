#pragma once

#include <cstdint>

struct Scissors
{
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
};

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};
