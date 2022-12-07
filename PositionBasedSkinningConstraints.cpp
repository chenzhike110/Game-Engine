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
    const unsigned i1 = m_bodies[0];
	const unsigned i2 = m_bodies[1];

    Vector3r &x1 = pd.getPosition(i1);
	Vector3r &x2 = pd.getPosition(i2);

    const Real invMass1 = pd.getInvMass(i1);
	const Real invMass2 = pd.getInvMass(i2);

    x1 += -0.5*invMass1*m_stiffness*((x1-x2).norm()-m_restLength)*(x1-x2).normalized();
    x2 += 0.5*invMass2*m_stiffness*((x1-x2).norm()-m_restLength)*(x1-x2).normalized();

    return true;

}
