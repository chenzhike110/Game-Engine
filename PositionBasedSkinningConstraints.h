#ifndef _POSITIONBASEDSKINNINGCONSTRAINTS_H_
#define _POSITIONBASEDSKINNINGCONSTRAINTS_H_
#endif

#include "Simulation/Constraints.h"
#include "PositionBasedSkinningModel.h"

namespace PBD
{
    class SimulationModel;

    class SkinningPointEdgeDistanceConstraint : public Constraint
    {
        public:
            static int TYPE_ID;
            Real m_restLength;

            SkinningPointEdgeDistanceConstraint() : Constraint(2) {}
            virtual int &getTypeId() const { return TYPE_ID; }

            bool initConstraint(PositionBasedSkinningModel &model, const unsigned int particle1, const unsigned int particle2);
            virtual bool solvePositionConstraint(SimulationModel &model, const unsigned int iter);
    };
}