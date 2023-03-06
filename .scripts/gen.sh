conan install . --install-folder ../build/msge --build=missing
cmake -B ../build/msge -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=../install/msge
cmake --build ../build/msge --target install
