add_library(PositionBasedDynamics
		${PBDLIB}/Common/Common.h
		
		DirectPositionBasedSolverForStiffRodsInterface.h
		MathFunctions.cpp
		MathFunctions.h
		PositionBasedDynamics.cpp
		PositionBasedDynamics.h
		PositionBasedElasticRods.cpp
		PositionBasedElasticRods.h
		PositionBasedFluids.cpp
		PositionBasedFluids.h
		PositionBasedRigidBodyDynamics.cpp
		PositionBasedRigidBodyDynamics.h
		PositionBasedGenericConstraints.h
		SPHKernels.cpp
		SPHKernels.h
		TimeIntegration.cpp
		TimeIntegration.h
		XPBD.cpp
		XPBD.h
		
		CMakeLists.txt
)

find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} ${PBDLIB})



