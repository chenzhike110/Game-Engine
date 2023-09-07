#ifndef ANIMATION_H
#define ANIMATION_H

#include <physics/Simulation/ParticleData.h>

using namespace PBD;

namespace ANIM
{
    class Animation
    {
        Animation();
        void loadBVH(const std::string &pFile);
        void loadFBX(const std::string &pFile);
    };
}

#endif