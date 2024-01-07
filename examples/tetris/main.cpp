#include "tetris.hpp"

int main()
{
    return Tetris{ Tetris::CreateInfo{ .windowWidth = 640, .windowHeight = 480 } }.exec();
}
