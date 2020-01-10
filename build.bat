rem set VCPKG_DIR="D:/library/vcpkg/installed/x64-windows"
set TOOLCHAIN_FILE="D:/library/vcpkg/scripts/buildsystems/vcpkg.cmake"

rem rmdir /S /Q build
rem mkdir build

cd build
rem cmake .. -DCMAKE_PREFIX_PATH=D:/library/vcpkg/installed/x64-windows -DCMAKE_TOOLCHAIN_FILE=D:/library/vcpkg/scripts/buildsystems/vcpkg.cmake -DBoost_ADDITIONAL_VERSIONS=1.70.0;1.71.0 -DCMAKE_BUILD_TYPE=Debug
rem cmake .. -A x64 -DCMAKE_TOOLCHAIN_FILE=D:/library/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
cmake .. -A x64 -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-windows -DRUNTIME="MD" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
rem cmake .. -A x64 -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-windows -DRUNTIME="MD" ..


cmake --build .

rem cmake -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..
rem cmake -DCMAKE_BUILD_TYPE=Debug -A x64 -DCMAKE_TOOLCHAIN_FILE=d:/library/vcpkg/scripts/buildsystems/vcpkg.cmake ..
