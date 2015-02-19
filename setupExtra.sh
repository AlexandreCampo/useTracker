#!/usr/bin/env bash

# this script aims at configuring a computer to run USE Tracker
# this specific part installs ffmpeg + opencv from sources

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

rootpath=`pwd`

# make sure the following dependencies have been installed 
echo The setup program of the USE Tracker is about to update your system
echo The following packages will be installed : make g++ automake freeglut3-dev libboost-program-options-dev.
echo Please enter your password to proceed
sudo apt-add-repository ppa:jon-severinsson/ffmpeg
sudo apt-get -qq update
sudo apt-get -qq upgrade
sudo apt-get -qq remove ffmpeg x264 libx264-dev

echo
echo
echo "Installing Dependencies"
sudo apt-get -qq install make g++ automake freeglut3-dev libboost-program-options-dev libopencv-dev build-essential checkinstall cmake pkg-config yasm libjpeg-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libxine-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev python-dev python-numpy libtbb-dev libqt4-dev libgtk2.0-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 libx264-dev v4l-utils ffmpeg || error "$LINENO: Aborted, could not install the required dependencies."

# download and compile latest stable opencv
echo
echo
echo "Downloading OpenCV" $version
version="$(wget -q -O - http://sourceforge.net/projects/opencvlibrary/files/opencv-unix | egrep -m1 -o '\"[0-9](\.[0-9]+)+' | cut -c2-)"
wget -O opencv-$version.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/$version/opencv-"$version".zip/download

echo
echo
echo "Building OpenCV" $version
unzip opencv-$version.zip
cd opencv-$version
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=OFF -D WITH_QT=ON -D WITH_OPENGL=ON -D BUILD_opencv_gpu=ON -D BUILD_opencv_ocl=ON ..
make -j2

echo
echo
echo "Installing OpenCV" $version
sudo make install 
sudo sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
echo "OpenCV" $version "ready to be used"

# back to root dir
cd $rootpath

exit 1






