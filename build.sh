ROOT=$(pwd)
echo ${ROOT}

cd extern
git clone https://github.com/InteractiveComputerGraphics/PositionBasedDynamics.git
cd PositionBasedDynamics
mkdir build_cmake
cd build_cmake
cmake -DPBD_LIBS_ONLY=On -DBUILD_SHARED_LIBS=On -DUSE_PYTHON_BINDINGS=Off -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=${ROOT}/libs/PositionBasedDynamics -DUSE_DOUBLE_PRECISION=ON ..
make -j4
make install

cd ${ROOT}/extern
git clone https://github.com/InteractiveComputerGraphics/GenericParameters.git
cd GenericParameters
mkdir build_cmake
cd build_cmake
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=${ROOT}/libs/GenericParameters -DGENERICPARAMETERS_NO_TESTS:BOOL=1 ..
make -j4
make install