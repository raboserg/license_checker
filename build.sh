cd build
#rm -r build

#export VGPKG_ROOT=~/projects/vcpkg
#export VCPKG_DIR=~/projects/vcpkg
#/installed/x64-linux

#cmake .. -DCMAKE_PREFIX_PATH=~/projects/vcpkg/installed/x64-linux/ \    -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake \#    -DBoost_ADDITIONAL_VERSIONS=1.70.0;1.71.0 \    -DCMAKE_BUILD_TYPE=Debug

#cmake .. -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-linux

#cmake .. -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-linux 
#-DOPENSSL_INCLUDE_DIR=/usr/include/openssl -DOPENSSL_SSL_LIBRARY=/usr/lib/openssl/libssl.so -DOPENSSL_CRYPTO_LIBRARY=/usr/lib/openssl/libcrypto.so 

cmake .. -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x64-linux 

#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake ..
#cmake .. -DCMAKE_TOOLCHAIN_FILE=~/projects/vcpkg/scripts/buildsystems/vcpkg.cmake \
#    -DBoost_ADDITIONAL_VERSIONS=1.70.0;1.71.0 \
#    -DCMAKE_BUILD_TYPE=Debug

cmake --build .
