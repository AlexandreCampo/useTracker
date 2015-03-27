#!/usr/bin/env bash

# this script aims at configuring a computer to run the FaMouS simulator

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# make sure the following dependencies have been installed 
echo The setup program of the USE Tracker is about to update your system
echo
echo
sudo echo Updating, please wait...
sudo apt-get -qq update
sudo apt-get -qq upgrade

# find latest version of libwxgtk
# wxversion=$(apt-cache search libwxgtk | egrep -o '[0-9](\.[0-9]+)+' | sort | tail -1)
# wxgtkpackages=$(apt-cache search libwxgtk | grep dev | grep $wxversion)
wxversion=3.0

echo
echo
echo "Installing Dependencies"
sudo apt-get --yes install build-essential cmake libwxbase$wxversion-dev libopencv-dev libboost-program-options-dev libwxgtk$wxversion-dev libwxgtk-media$wxversion-dev freeglut3-dev libva-dev libbz2-dev libx264-dev || error "$LINENO: Aborted, could not install the required dependencies."

# download and compile latest stable opencv
echo
echo
echo "Downloading ArUco" $version
version="$(wget -q -O - http://sourceforge.net/projects/aruco/files/ | egrep -m1 -o '\"[0-9](\.[0-9]+)+' | cut -c2-)"
wget -O aruco-$version.tgz http://sourceforge.net/projects/aruco/files/$version/aruco-"$version".tgz/download

echo
echo
echo "Building ArUco" $version
tar zxvf aruco-$version.tgz
cd aruco-$version
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release .
make

echo
echo
echo "Installing ArUco" $version
sudo make install 
sudo ldconfig
echo "ArUco" $version "ready to be used"
cd ..

echo
echo
echo "Building USE Tracker"
make clean
make release

# install images
sudo mkdir -p /usr/share/useTracker
sudo cp -a images /usr/share/useTracker/

# install program
sudo cp ./bin/Release/useTracker /usr/bin/

