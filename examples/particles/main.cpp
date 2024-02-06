#include "particles_application.hpp"

int main(int argc, char** argv)
{
    return ParticlesApplication{ ParticlesApplication::CreateInfo::readFromCmd(argc, argv) }.exec();
}
