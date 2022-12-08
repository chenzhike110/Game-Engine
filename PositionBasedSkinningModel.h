#ifndef __POSITIONBASEDSKINNING_H__
#define __POSITIONBASEDSKINNING_H__
#endif

#include "Simulation/ParticleData.h"
#include "Simulation/SimulationModel.h"
#include <vector>

namespace PBD
{
    class PositionBasedSkinningModel : public SimulationModel
    {
        public:
            PositionBasedSkinningModel();
            virtual ~PositionBasedSkinningModel();

            ParticleData m_boneParticles;
            Vector3r m_stiffness;
            std::vector<Matrix4d> m_joints; 

        public:
            void updateBones(std::vector<Matrix4d> joints);
            void addVertice(const unsigned int nPoints, Vector3r *points);
            void addPointEdgeDistanceConstraint(const unsigned int pA, const unsigned int pB, const unsigned int pG);

            void setBendingAndTwistingStiffness(const Vector3r &val) { m_stiffness = val; }
            Vector3r &getBendingAndTwistingStiffness() { return m_stiffness; }
    };
}