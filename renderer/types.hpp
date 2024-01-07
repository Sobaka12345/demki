#pragma once

#include <cstdint>

enum class GAPI
{
    OpenGL,
    Vulkan
};

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

enum class Multisampling
{
    MSA_1X = 1,
    MSA_2X = 2,
    MSA_4X = 4,
    MSA_8X = 8,
    MSA_16X = 16,
    MSA_32X = 32,
    MSA_64X = 64,
};
