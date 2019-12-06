rem setx /m OVPN3_HOME_DIR "D:/library/openvpn3"
rmdir /S /Q build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..