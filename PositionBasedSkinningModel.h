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

        protected:
            ParticleData m_surfaceParticles;
            Vector3r m_stiffness;

        public:
            void addVertice(const unsigned int nPoints, Vector3r *points);
            void addPointEdgeDistanceConstraint(const unsigned int pA, const unsigned int pB, const unsigned int pG);

            void setBendingAndTwistingStiffness(const Vector3r &val) { m_stiffness = val; }
            Vector3r &getBendingAndTwistingStiffness() { return m_stiffness; }
    };
}