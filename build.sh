cd build
#rm -r .
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake ..

export VCPKG_DIR=~/projects/vcpkg/installed/x64-linux

cmake .. -DCMAKE_PREFIX_PATH=~/projects/vcpkg/installed/x64-linux/ \
    -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DBoost_ADDITIONAL_VERSIONS=1.70.0;1.71.0 \
    -DCMAKE_BUILD_TYPE=Debug


#cmake .. -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake \
#    -DBoost_ADDITIONAL_VERSIONS=1.70.0;1.71.0 \
#    -DCMAKE_BUILD_TYPE=Debug

cmake --build .
