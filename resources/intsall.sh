#! /bin/sh
# export LD_LIBRARY_PATH=/opt/simulator/lib
# export BOOST_ROOT 
sudo ln -s libP7.so libP7.so.2

chmod +x itvpnagent
sudo cp libP7.so /usr/local/lib
sudo cp libssl.so.1.1 /usr/local/lib
sudo cp libcrypto.so.1.1 /usr/local/lib
sudo cp libboost_system.so.1.72.0 /usr/local/lib
sudo cp libboost_filesystem.so.1.72.0 /usr/local/lib
sudo cp libACE.so.6.5.6 /usr/local/lib
#sudo cp libP7.so.2 /usr/local/lib
sudo cp libboost_random.so.1.72.0 /usr/local/lib
sudo cp libboost_thread.so.1.72.0 /usr/local/lib
sudo cp libboost_chrono.so.1.72.0 /usr/local/lib
sudo cp libboost_atomic.so.1.72.0 /usr/local/lib
sudo cp libboost_date_time.so.1.72.0 /usr/local/lib
sudo cp libboost_regex.so.1.72.0 /usr/local/lib
sudo cp libssl.so.3 /usr/local/lib
sudo cp libcrypto.so.3 /usr/local/lib
sudo cp libz.so.1 /usr/local/lib
cd /usr/local/lib
sudo ldconfig -n

# ldconfig -v
# ldconfig -v | grep -i ssl
# strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep GLIBCXX


./config --openssldir=/home/user/projects/library/ITVPN_DEV/build -Wl-rpath=/home/user/install/GCC/gcc-5.4.0/build/lib64 no-shared no-idea no-mdc2 no-rc5 zlib enable-tlsext
./config --prefix=/home/user/projects/library/ITVPN_DEV/build --openssldir=/home/user/projects/library/ITVPN_DEV/build '-Wl,-rpath,$(LD_LIBRARY_PATH)'

