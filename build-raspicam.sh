!#usr/bin/env sh

# setup folder
raspicam=$(find $PWD -type d -name "raspicam-*")
echo ${raspicam} 
cd ${raspicam} 
mkdir build-raspi 
cd build-raspi && echo $PWD 

# configure cmake 
cmake ..

# compile and install
make -j$(nproc) && make install && ldconfig && cd ..
echo $PWD
echo "raspicam ready, continue next step..."
echo
