#include "cubic.hpp"

int main(int argc, char** argv)
{
    return Cubic{ Cubic::CreateInfo{ .windowWidth = 1920, .windowHeight = 1080 } }.exec();
}
