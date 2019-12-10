rem setx /m OVPN3_HOME_DIR "D:/library/openvpn3"
rmdir /S /Q build
mkdir build
cd build
rem cmake -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..
cmake -DCMAKE_BUILD_TYPE=Debug -A x64 -DCMAKE_TOOLCHAIN_FILE=d:/library/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build ..