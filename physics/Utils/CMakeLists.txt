add_library(Utils
		FileSystem.h
		Hashmap.h
		IndexedFaceMesh.cpp
		IndexedFaceMesh.h
		IndexedTetMesh.cpp
		IndexedTetMesh.h
		Logger.h
		OBJLoader.h
		PLYLoader.h
		SceneLoader.cpp
		SceneLoader.h
		StringTools.h
		SystemInfo.h
		TetGenLoader.cpp
		TetGenLoader.h
		Timing.h
		Version.h
		VolumeIntegration.cpp
		VolumeIntegration.h

		CMakeLists.txt
)

############################################################
# GenericParameters
############################################################
include_directories(${GenericParameters_INCLUDE_DIR} ${PBDLIB} ${PROJECT_SOURCE_DIR})
if(TARGET Ext_GenericParameters)
	add_dependencies(Utils Ext_GenericParameters)
endif()

find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )




