set project_name=senttbox
conan install . --install-folder ../build/%project_name% --build=missing
cmake -B ../build/%project_name% -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=../install/%project_name%
cmake --build ../build/%project_name% --target install