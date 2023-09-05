ROOT=${pwd}
echo ROOT

cd extern
git clone https://github.com/InteractiveComputerGraphics/PositionBasedDynamics.git
cd PositionBasedDynamics
mkdir build_cmake
cmake -DPBD_LIBS_ONLY=On -DUSE_PYTHON_BINDINGS=Off -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=${ROOT}/libs/PositionBasedDynamics -DUSE_DOUBLE_PRECISION=ON ..
make -j4
make install
cd ${pwd}