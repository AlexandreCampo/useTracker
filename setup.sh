#!/usr/bin/env bash

# this script aims at configuring a computer to run the FaMouS simulator

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# make sure the following dependencies have been installed 
gksu -m "The setup program of the FaMouS simulator is about to install the following required dependencies : make g++ dos2unix automake libtool libglu1-mesa-dev libgl1-mesa-dev freeglut3-dev libgsl0-dev libtinyxml-dev libboost-program-options-dev" "apt-get --yes install make g++ dos2unix automake libtool libglu1-mesa-dev libgl1-mesa-dev freeglut3-dev libgsl0-dev libtinyxml-dev libboost-program-options-dev" || error "$LINENO: Could not install the required dependencies."


# install dependencies
wget  http://sourceforge.net/projects/premake/files/Premake/4.3/premake-4.3-linux.tar.gz/download || error "$LINENO: Could not download premake4 from the web."
mv download pm.tgz
tar zxvf pm.tgz 
rm pm.tgz

cd bullet
wget http://bullet.googlecode.com/files/bullet-2.78.zip  || error "$LINENO: Could not download bullet from the web."
unzip -u bullet-2.78.zip
cp btRigidBody.h bullet-2.78/src/BulletDynamics/Dynamics/btRigidBody.h
cd bullet-2.78  || error "$LINENO: bullet library is not where expected."
dos2unix autogen.sh
chmod u+x autogen.sh
./autogen.sh
./configure --disable-demos || error "$LINENO: Configuration of bullet failed."
make clean
make  || error "$LINENO: Compilation of bullet failed."
gksu -m "The setup program of the FaMouS simulator is about to install the modified bullet 2.78 library on your system" "make install" || error "$LINENO: Could not install modified bullet 2.78."
gksu ldconfig || error "$LINENO: Failed to update dynamic linker informations."
cd ../../

# from here on, the most difficult is done. The simulator should compile seamlessly if previous things are properly set up.
./premake4 clean
./premake4 gmake
make clean
make

cd experiments/discrimination
../../premake4 clean
../../premake4 gmake
make clean
make



