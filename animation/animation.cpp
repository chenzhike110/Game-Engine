#include "animation/animation.h"
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "animation/BVHLoader.h"

#include "visual/MeshUtils.h"
#include "Utils/FileSystem.h"
// #include "Utils/RigidBody.h"
#include "physics/Simulation/SimulationModel.h"
#include "physics/Simulation/Simulation.h"
#include "physics/Utils/IndexedFaceMesh.h"

namespace ANIM
{

Animation::Animation()
{
    std::string m_exePath = Utilities::FileSystem::getProgramPath();
    std::string fileNameBox = Utilities::FileSystem::normalizePath(m_exePath + "/resources/models/cylinder_unit_box.obj");
	MESH::loadOBJ(fileNameBox, vdBox, meshBox, Vector3r::Zero(), Matrix3r::Identity(), Vector3r::Ones());
}

void Animation::loadBVH(const std::string &pFile)
{   
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(pFile, aiProcess_ValidateDataStructure);
    // std::cout << scene->mAnimations[0]->mNumChannels << std::endl;
    // build physical model from aiScene
    PBD::SimulationModel *model = PBD::Simulation::getCurrent()->getModel();
    SimulationModel::RigidBodyVector &rb = model->getRigidBodies();

    // update root node
    _rootName = scene->mRootNode->mName.C_Str();
    _jointName.push_back(_rootName);
    auto rootAnim = scene->mAnimations[0]->mChannels[0];
    for (int f=0; f<rootAnim->mNumPositionKeys; f++)
    {
        aiVector3D pos = rootAnim->mPositionKeys[f].mValue;
        _rootPos.push_back(Eigen::Vector3d({pos.x, pos.y, pos.z}));
    }
    _tPos[_rootName] = _rootPos[0];

    for (unsigned int i=1; i<scene->mAnimations[0]->mNumChannels; ++i)
    {
        auto nodeAnim = scene->mAnimations[0]->mChannels[i];
        _jointName.push_back(nodeAnim->mNodeName.C_Str());
        std::cout << "Get Joint: " << nodeAnim->mNodeName.C_Str() << " " << nodeAnim->mNumPositionKeys;
        aiVector3D pos = nodeAnim->mPositionKeys[0].mValue;
        std::cout << " " << pos.x << " " << pos.y << " " << pos.z << std::endl;
        const aiNode* node = scene->mRootNode->FindNode(nodeAnim->mNodeName.C_Str());

        if (node->mParent != nullptr)
        {
            Eigen::Vector3d offset({pos.x, pos.y, pos.z});
            Eigen::Vector3d norm({1.0, offset.norm(), 1.0});
            Eigen::Quaterniond rotation = Eigen::Quaterniond::FromTwoVectors(norm, offset);
            _tPos[node->mName.C_Str()] = _tPos[node->mParent->mName.C_Str()] + offset;
            std::cout << "Add Link from " << node->mName.C_Str() << " to " << node->mParent->mName.C_Str()
                << " " << offset.norm() << std::endl;
            rb.resize(rb.size()+1);
            rb[rb.size()-1] = new PBD::RigidBody();
            rb[rb.size()-1]->initBody(
                1.0,
                (_tPos[node->mName.C_Str()]+_tPos[node->mParent->mName.C_Str()])/2.0*0.01,
                // Quaternionr(1.0, 0.0, 0.0, 0.0),
                rotation,
                vdBox, meshBox, norm*0.01
            );
            
        }
        

    }
}

void Animation::loadFBX(const std::string &pFile)
{

}

}