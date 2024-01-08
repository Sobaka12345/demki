#include "tetris.hpp"

int main(int argc, char** argv)
{
    return Tetris{ Tetris::CreateInfo::readFromCmd(argc, argv) }.exec();
}
