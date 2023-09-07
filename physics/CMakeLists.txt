find_package(Eigen3 REQUIRED)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -idirafter${EIGEN3_INCLUDE_DIR}")
endif()

set(EXT_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(EXT_CMAKE_BUILD_TYPE "Release")
endif()

if ((DEFINED Discregrid_INCLUDE_DIR) AND (DEFINED Discregrid_DEBUG_LIB) AND (DEFINED Discregrid_LIB))
	message(STATUS "Using ${Discregrid_INCLUDE_DIR}")
	set(Discregrid_LIBRARIES optimized ${Discregrid_LIB} debug ${Discregrid_DEBUG_LIB})
else()
	ExternalProject_Add(
	   Ext_Discregrid
	   PREFIX "${CMAKE_BINARY_DIR}/extern/Discregrid"
	   GIT_REPOSITORY https://github.com/InteractiveComputerGraphics/Discregrid.git
	   GIT_TAG "4c27e1cc88be828c6ac5b8a05759ac7e01cf79e9"
	   INSTALL_DIR ${ExternalInstallDir}/Discregrid
	   CMAKE_ARGS -DCMAKE_BUILD_TYPE:STRING=${EXT_CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}/Discregrid
	   -DBUILD_CMD_EXECUTABLE:BOOL=0 -DEIGEN3_INCLUDE_DIR:PATH=${EIGEN3_INCLUDE_DIR} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
	) 
	ExternalProject_Get_Property(Ext_Discregrid INSTALL_DIR)
	set(Discregrid_INCLUDE_DIR ${INSTALL_DIR}/include)
	set(Discregrid_LIBRARIES optimized ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Discregrid${CMAKE_STATIC_LIBRARY_SUFFIX} 
							 debug ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Discregrid_d${CMAKE_STATIC_LIBRARY_SUFFIX})
	unset(INSTALL_DIR)
	message(STATUS "Building ${Discregrid_INCLUDE_DIR}")
endif()

if (DEFINED GenericParameters_INCLUDE_DIR)
	message(STATUS "Using ${GenericParameters_INCLUDE_DIR}")
else()
	## GenericParameters
	ExternalProject_Add(
	   Ext_GenericParameters
	   PREFIX "${ExternalInstallDir}/GenericParameters"
	   GIT_REPOSITORY https://github.com/InteractiveComputerGraphics/GenericParameters.git
	   GIT_TAG "a4e2744eea526270cfe38b826440d09f66473316"
	   INSTALL_DIR ${ExternalInstallDir}/GenericParameters
	   CMAKE_ARGS -DCMAKE_BUILD_TYPE=${EXT_CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}/GenericParameters -DGENERICPARAMETERS_NO_TESTS:BOOL=1		
	) 
	ExternalProject_Get_Property(Ext_GenericParameters INSTALL_DIR)
	set(GenericParameters_INCLUDE_DIR ${INSTALL_DIR}/include)
	unset(INSTALL_DIR)
endif()

add_subdirectory(PositionBasedDynamics)
add_subdirectory(Simulation)
add_subdirectory(Utils)