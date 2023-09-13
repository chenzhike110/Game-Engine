#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>
#include <Eigen/Dense>
#include <physics/Simulation/ParticleData.h>
#include <physics/Utils/IndexedFaceMesh.h>

using namespace PBD;

namespace ANIM
{

class Animation
{
public:
    Animation();
    void loadBVH(const std::string &pFile);
    void loadFBX(const std::string &pFile);

private:

    std::vector<Eigen::Vector3d> _rootPos;
    std::map<std::string, Eigen::Vector3i> _constraint;
    std::map<std::string, std::string> _jointParent;
    std::map<std::string, std::vector<Eigen::Vector3d>> _jointAngle;
    std::map<std::string, Eigen::Vector3d> _tPos;
    std::map<std::string, int> _rbIndex;
    std::string _rootName;
    // Assimp::BVHLoader* _bvhloader;
    Utilities::IndexedFaceMesh meshBox;
	PBD::VertexData vdBox;
    // SimulationModel::RigidBodyVector m_rigidBodies;
};


}

#endif