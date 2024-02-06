#include "cubic.hpp"

int main(int argc, char** argv)
{
    return Cubic{ Cubic::CreateInfo::readFromCmd(argc, argv) }.exec();
}
