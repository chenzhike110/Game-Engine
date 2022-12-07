#include "PositionBasedSkinningConstraints.h"

using namespace PBD;

bool SkinningPointEdgeDistanceConstraint::initConstraint(PositionBasedSkinningModel &model, const unsigned int particle1, const unsigned int particle2)
{
    m_bodies[0] = particle1;
	m_bodies[1] = particle2;

    ParticleData &pd = model.getParticles();

    Vector3r &x1 = pd.getPosition(particle1);
    Vector3r &x2 = pd.getPosition(particle2);

    m_restLength = (x1 - x2).norm();
    return true;
}

bool SkinningPointEdgeDistanceConstraint::solvePositionConstraint(SimulationModel &model, const unsigned int iter)
{
    PositionBasedSkinningModel &simModel = static_cast<PositionBasedSkinningModel&>(model);

    ParticleData &pd = model.getParticles();
    
}
