#!/usr/bin/env bash

# this script aims at configuring a computer to run USE Tracker
# this specific part installs ffmpeg + opencv from sources

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# make sure the following dependencies have been installed 
gksu -m "The setup program of the USE Tracker is about to update your system and install the following packages : make g++ automake freeglut3-dev libboost-program-options-dev" "sudo apt-add-repository ppa:jon-severinsson/ffmpeg; apt-get update; apt-get upgrade -y; apt-get --yes install make g++ automake freeglut3-dev libboost-program-options-dev" || error "$LINENO: Could not install the required dependencies."

exit (-1);

gksu -m "The setup program of the USE Tracker is about to install the following required dependencies : make g++ automake freeglut3-dev libboost-program-options-dev" "apt-get --yes install make g++ automake freeglut3-dev libboost-program-options-dev" || error "$LINENO: Could not install the required dependencies."


# install dependencies
sudo apt-add-repository ppa:jon-severinsson/ffmpeg
sudo apt-get update 
sudo apt-get upgrade -y
version="$(wget -q -O - http://sourceforge.net/projects/opencvlibrary/files/opencv-unix | egrep -m1 -o '\"[0-9](\.[0-9]+)+' | cut -c2-)"
echo "Installing OpenCV" $version
mkdir OpenCV
cd OpenCV
echo "Removing any pre-installed ffmpeg and x264"
sudo apt-get -qq remove ffmpeg x264 libx264-dev
echo "Installing Dependenices"
sudo apt-get -qq install libopencv-dev build-essential checkinstall cmake pkg-config yasm libjpeg-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libxine-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev python-dev python-numpy libtbb-dev libqt4-dev libgtk2.0-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils ffmpeg
echo "Downloading OpenCV" $version
wget -O OpenCV-$version.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/$version/opencv-"$version".zip/download
echo "Installing OpenCV" $version
unzip OpenCV-$version.zip
cd opencv-$version
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D BUILD_opencv_gpu=OFF -D BUILD_opencv_ocl=OFF ..
make -j2
sudo checkinstall
sudo sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
echo "OpenCV" $version "ready to be used"


# add ffmpeg ppa


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



