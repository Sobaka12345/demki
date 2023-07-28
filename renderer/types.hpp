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

enum class Multisampling
{
    MSA_1X,
    MSA_2X,
    MSA_4X,
    MSA_8X,
    MSA_16X,
    MSA_32X,
    MSA_64X,
};

enum class SampleShading
{
    SS_0_PERCENT,
    SS_20_PERCENT,
    SS_40_PERCENT,
    SS_60_PERCENT,
    SS_80_PERCENT,
    SS_100_PERCENT,
};
