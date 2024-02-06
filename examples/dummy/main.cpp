#include "dummy.hpp"

int main(int argc, char** argv)
{
    return Dummy{ Dummy::CreateInfo::readFromCmd(argc, argv) }.exec();
}
