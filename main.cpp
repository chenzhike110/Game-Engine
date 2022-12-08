#include "Common/Common.h"
#include "Demos/Visualization/MiniGL.h"
#include "Demos/Visualization/Selection.h"
#include "Simulation/TimeManager.h"
#include <Eigen/Dense>
#include "Simulation/SimulationModel.h"
#include "Simulation/TimeStepController.h"
#include <iostream>
#include "Demos/Visualization/Visualization.h"
#include "Utils/Logger.h"
#include "Utils/Timing.h"
#include "Utils/FileSystem.h"
#include "Demos/Common/DemoBase.h"
#include "Demos/Common/TweakBarParameters.h"
#include "Simulation/Simulation.h"
#include "Utils/OBJLoader.h"
#include "PositionBasedSkinningModel.h"

// Enable memory leak detection
#if defined(_DEBUG) && !defined(EIGEN_ALIGN)
	#define new DEBUG_NEW 
#endif

using namespace PBD;
using namespace Eigen;
using namespace std;
using namespace Utilities;

void timeStep();
void buildModel();
void createCharacter();
void render();
void reset();
void TW_CALL setBendingMethod(const void *value, void *clientData);
void TW_CALL getBendingMethod(void *value, void *clientData);
void TW_CALL setSimulationMethod(const void *value, void *clientData);
void TW_CALL getSimulationMethod(void *value, void *clientData);

const Real width = 10.0;
const Real height = 10.0;
short simulationMethod = 2;
short bendingMethod = 2;
Real bendingStiffness = 0.01;
DemoBase *base;

std::vector<Vector3r> renderFaces;

// main 
int main( int argc, char **argv )
{
	REPORT_MEMORY_LEAKS

	base = new DemoBase();
	base->init(argc, argv, "Skinning");

	SimulationModel *model = new SimulationModel();
	model->init();
	Simulation::getCurrent()->setModel(model);

	buildModel();

	base->createParameterGUI();

	// OpenGL
	MiniGL::setClientIdleFunc (timeStep);		
	MiniGL::addKeyFunc('r', reset);
	MiniGL::setClientSceneFunc(render);			
	MiniGL::setViewport (40.0f, 0.1f, 800.0f, Vector3r (0.0, 100.0, 300.0), Vector3r (0.0, 100.0, 0.0));

	TwType enumType2 = TwDefineEnum("SimulationMethodType", NULL, 0);
	TwAddVarCB(MiniGL::getTweakBar(), "SimulationMethod", enumType2, setSimulationMethod, getSimulationMethod, &simulationMethod, 
		" label='Simulation method' enum='0 {None}, 1 {Distance constraints}, 2 {FEM based PBD}, 3 {Strain based dynamics}, 4 {XPBD distance constraints}' group=Simulation");
	TwType enumType3 = TwDefineEnum("BendingMethodType", NULL, 0);
	TwAddVarCB(MiniGL::getTweakBar(), "BendingMethod", enumType3, setBendingMethod, getBendingMethod, &bendingMethod, 
		" label='Bending method' enum='0 {None}, 1 {Dihedral angle}, 2 {Isometric bending}, 3 {XPBD isometric bending}' group=Bending");
    MiniGL::mainLoop();	

	base->cleanup();

	Utilities::Timing::printAverageTimes();
	Utilities::Timing::printTimeSums();

	delete Simulation::getCurrent();
	delete base;
	delete model;

	return 0;
}

void reset()
{
	Utilities::Timing::printAverageTimes();
	Utilities::Timing::reset();

	Simulation::getCurrent()->reset();
	base->getSelectedParticles().clear();
}


void timeStep ()
{
	const Real pauseAt = base->getValue<Real>(DemoBase::PAUSE_AT);
	if ((pauseAt > 0.0) && (pauseAt < TimeManager::getCurrent()->getTime()))
		base->setValue(DemoBase::PAUSE, true);

	if (base->getValue<bool>(DemoBase::PAUSE))
		return;

	// Simulation code
	SimulationModel *model = Simulation::getCurrent()->getModel();
	const unsigned int numSteps = base->getValue<unsigned int>(DemoBase::NUM_STEPS_PER_RENDER);
	for (unsigned int i = 0; i < numSteps; i++)
	{
		START_TIMING("SimStep");
		Simulation::getCurrent()->getTimeStep()->step(*model);
		STOP_TIMING_AVG;
	}
}

bool load_OFF(string _filename, std::vector<Vector3r> &verts, std::vector<Vector3r> &inds)
{
    // parse the file
    std::ifstream ifs ( _filename );
    if ( !ifs ) {
        std::cerr << "file not found\n";
        return false;
    }
    else {
        unsigned int   nV, nF, dummy;
        unsigned int   i, idx;
        float          x, y, z;
        std::string    magic;
        // header: OFF #Vertice, #Faces, #Edges
        ifs >> magic;
        if ( magic != "OFF" ) {
            std::cerr << "No OFF file\n";
            return false;
        }
        ifs >> nV >> nF >> dummy;
        // verts.resize(nV);
        // inds.resize(nF);
        // read vertices
        for ( i=0; i < nV && !ifs.eof(); ++i ) {
            ifs >> x >> y >> z;
            verts.push_back(Vector3r(x, y, z));
        }
        // faces
        for ( i=0; i<nF; ++i ) {
            ifs >> nV;

            if ( nV == 3 ) {
                ifs >> x >> y >> z;
                inds.push_back(Vector3r(x, y, z));
            }
            else {
                std::cerr << "Only triangular faces are supported\n";
            }
        }
        std::cout
        << "read "
        << _filename << ": "
        << verts.size() << " vertices, "
        << ( inds.size() /3 ) << " triangles" << std::endl;
    }

    ifs.close();
    return true;
}

void buildModel ()
{
	TimeManager::getCurrent()->setTimeStepSize(0.002f);
    PositionBasedSkinningModel *model = (PositionBasedSkinningModel*) Simulation::getCurrent()->getModel();
	model->setBendingAndTwistingStiffness(Vector3r(0.5, 0.5, 0.5));

	// sim.setDamping(0.001f);
    createCharacter();
}

void render ()
{
	base->render();
    // Draw sim model
	PositionBasedSkinningModel *model = (PositionBasedSkinningModel*) Simulation::getCurrent()->getModel();
	ParticleData &pd = model->getParticles();
    float pointColor[4] = { 0.1f, 0.2f, 0.6f, 1 };
    float triangleColor[4] = { 1.0f, 1.0f, 1.0f, 0.7f };
    float edgeColor[4] = { 0.0f, 0.6f, 0.2f, 1 };
    for (unsigned int i=0; i<pd.size(); i++) {
        MiniGL::drawSphere(pd.getPosition(i), 0.1f, pointColor);
    }
    // LOG_INFO << "123";
    for (unsigned int i=0; i<renderFaces.size(); i++) {
        Vector3r a = pd.getPosition(renderFaces[i][0]);
        Vector3r b = pd.getPosition(renderFaces[i][1]);
        Vector3r c = pd.getPosition(renderFaces[i][2]);
        Vector3r norm = (b-a).cross(c-b);
        MiniGL::drawTriangle(a, b, c, norm, triangleColor);
        Vector3r center = (a + b + c) / 3.0;
        MiniGL::drawVector(center, center + norm.normalized()*5, 2.0f, edgeColor);
    }
    MiniGL::drawTime(TimeManager::getCurrent()->getTime());
}


/** Create a particle model mesh 
*/
void createCharacter()
{
	PositionBasedSkinningModel *model = (PositionBasedSkinningModel*) Simulation::getCurrent()->getModel();
    ParticleData &particles = model->getParticles();
    string filename = "/home/czk119/ws/PositionBasedDynamics/Demos/Skinning/data/Kaya.off";
    SimulationModel::ConstraintVector &constraints = model->getConstraints();

    // load off
    std::vector<Vector3r> vertices;
    load_OFF(filename, vertices, renderFaces);
    // model->addFaces(inds);
    for (int i=0; i<vertices.size(); i++) {
        particles.addVertex(vertices[i]);
    }
}

void TW_CALL setBendingMethod(const void *value, void *clientData)
{
	const short val = *(const short *)(value);
	*((short*)clientData) = val;
	reset();
}

void TW_CALL getBendingMethod(void *value, void *clientData)
{
	*(short *)(value) = *((short*)clientData);
}

void TW_CALL setSimulationMethod(const void *value, void *clientData)
{
	const short val = *(const short *)(value);
	*((short*)clientData) = val;
	reset();
}

void TW_CALL getSimulationMethod(void *value, void *clientData)
{
	*(short *)(value) = *((short*)clientData);
}
