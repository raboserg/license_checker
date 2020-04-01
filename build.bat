chcp 65001
rem set VCPKG_DIR="D:/library/vcpkg/installed/x64-windows"
rem set TOOLCHAIN_FILE="D:/library/vcpkg/scripts/buildsystems/vcpkg.cmake"
rem rmdir /S /Q build
rem mkdir build

cd build
rem cmake .. -A x64 -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x64-windows -DRUNTIME="MT" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
rem cmake .. -DCMAKE_CXX_FLAGS:STRING="-m32" -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x86-windows -DRUNTIME="MT" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
rem cmake .. -A x86 -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x86-windows -DRUNTIME="MT" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
cmake .. -A x64 -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-windows -DRUNTIME="MT" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
rem --config Release
rem cmake .. -A x86 -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x86-windows -DRUNTIME="MT" -DCMAKE_TOOLCHAIN_FILE=D:\library\vcpkg\scripts\buildsystems\vcpkg.cmake ..
cmake --build .