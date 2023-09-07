#include "physics/Common/Common.h"
#include "physics/Utils/Timing.h"
#include "visual/MiniGL.h"
#include "visual/Widget.h"
#include "visual/MeshUtils.h"
#include "physics/Utils/FileSystem.h"
#include "physics/Simulation/Simulation.h"
#include "physics/Simulation/TimeManager.h"
#include "physics/Simulation/SimulationModel.h"
#include "physics/Simulation/DistanceFieldCollisionDetection.h"

#include "animation/animation.h"

using namespace PBD;
using namespace Utilities;

namespace {
    MeshWidget* Widget;
    DistanceFieldCollisionDetection *cd;
}
void render();
void timeStep();
void buildModel();

int main(int argc, char **argv){
    Widget = new MeshWidget();
    Widget->init(argc, argv);

    // SimulationModel *model = new SimulationModel();
	// model->init();
	// Simulation::getCurrent()->setModel(model);

    // cd = new DistanceFieldCollisionDetection();
	// cd->init();

    // buildModel();

    // MiniGL::setClientIdleFunc(timeStep);	
    MiniGL::setClientSceneFunc(render);		
    MiniGL::setViewport (40.0f, 0.1f, 500.0, Vector3r (5.0, 30.0, 70.0), Vector3r (5.0, 0.0, 0.0));
    MiniGL::mainLoop();

    // delete Simulation::getCurrent();
	delete Widget;
	// delete model;
	// delete cd;

    return 0;
}

void loadFBX();

void render(){
    Widget->render();
}

void timeStep(){
    SimulationModel *model = Simulation::getCurrent()->getModel();
    Simulation::getCurrent()->getTimeStep()->step(*model);
}

void buildModel(){
    // set time step
    TimeManager::getCurrent ()->setTimeStepSize (static_cast<Real>(0.005));

    SimulationModel *model = Simulation::getCurrent()->getModel();
	Simulation::getCurrent()->getTimeStep()->setCollisionDetection(*model, cd);

    SimulationModel::RigidBodyVector &rb = model->getRigidBodies();
	SimulationModel::ConstraintVector &constraints = model->getConstraints();

    std::string fileNameSphere = FileSystem::normalizePath(Widget->getExePath() + "/resources/models/sphere.obj");
	IndexedFaceMesh meshSphere;
	VertexData vdSphere;
	MESH::loadOBJ(fileNameSphere, vdSphere, meshSphere, Vector3r::Zero(), Matrix3r::Identity(), 2.0*Vector3r::Ones());

    std::string fileNameBox = FileSystem::normalizePath(Widget->getExePath() + "/resources/models/cube.obj");
	IndexedFaceMesh meshBox;
	VertexData vdBox;
	MESH::loadOBJ(fileNameBox, vdBox, meshBox, Vector3r::Zero(), Matrix3r::Identity(), Vector3r::Ones());
	meshBox.setFlatShading(true);

    rb.resize(2);
	unsigned int rbIndex = 0;

    // add floor
    rb[rbIndex] = new RigidBody();
	rb[rbIndex]->initBody(1.0,
		Vector3r(0.0, -0.5, 0.0),
		Quaternionr(1.0, 0.0, 0.0, 0.0),
		vdBox, meshBox, Vector3r(100.0, 1.0, 100.0));
	rb[rbIndex]->setMass(0.0);

    const std::vector<Vector3r> &vertices = rb[rbIndex]->getGeometry().getVertexDataLocal().getVertices();
	const unsigned int nVert = static_cast<unsigned int>(vertices.size());

	cd->addCollisionBox(rbIndex, CollisionDetection::CollisionObject::RigidBodyCollisionObjectType, vertices.data(), nVert, Vector3r(100.0, 1.0, 100.0));
	rbIndex++;

    // add ball
    rb[rbIndex] = new RigidBody();
    rb[rbIndex]->initBody(100.0,
        Vector3r(-0.5, 14.0, -0.5),
        Quaternionr(1.0, 0.0, 0.0, 0.0),
        vdSphere, meshSphere);

    const std::vector<Vector3r> &vertices_b = rb[rbIndex]->getGeometry().getVertexDataLocal().getVertices();
    const unsigned int nVert_b = static_cast<unsigned int>(vertices_b.size());
    cd->addCollisionSphere(rbIndex, CollisionDetection::CollisionObject::RigidBodyCollisionObjectType, vertices_b.data(), nVert_b, 2.0);
}