#!/usr/bin/env bash

# this script aims at configuring a computer to run USE Tracker

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# find out if sudo is available, otherwise use su
SUDOAVAILABLE=$(which sudo | wc -l)

# find latest version of libwxgtk
# wxversion=$(apt-cache search libwxgtk | egrep -o '[0-9](\.[0-9]+)+' | sort | tail -1)
# wxgtkpackages=$(apt-cache search libwxgtk | grep dev | grep $wxversion)
wxversion=3.0

# make sure the following dependencies have been installed 
echo The setup program of the USE Tracker is about to update your system
echo
echo

if (( "$SUDOAVAILABLE" >= "1" ))
then 

sudo echo Updating, please wait...
sudo apt-get -q -y update
sudo apt-get -q -y upgrade
echo Installing Dependencies
sudo apt-get --yes install build-essential cmake libwxbase$wxversion-dev libopencv-dev libboost-program-options-dev libboost-filesystem-dev libwxgtk$wxversion-dev libwxgtk-media$wxversion-dev freeglut3-dev libva-dev libbz2-dev libx264-dev libbluetooth-dev || error '$LINENO: Aborted, could not install the required dependencies.'

else

su -c "echo Updating, please wait...;\
apt-get -q -y update;\
apt-get -q -y upgrade; \
echo Installing Dependencies; \
apt-get --yes install build-essential cmake libwxbase$wxversion-dev libopencv-dev libboost-program-options-dev libboost-filesystem-dev libwxgtk$wxversion-dev libwxgtk-media$wxversion-dev freeglut3-dev libva-dev libbz2-dev libx264-dev libbluetooth-dev || error '$LINENO: Aborted, could not install the required dependencies.' "

fi


# download and compile latest stable opencv
echo
echo
echo "Downloading ArUco" $version
version="$(wget -q -O - http://sourceforge.net/projects/aruco/files/ | egrep -m1 -o '\"[0-9](\.[0-9]+)+' | cut -c2-)"
version="1.3.0"
#wget -O aruco-$version.tgz http://sourceforge.net/projects/aruco/files/$version/aruco-"$version".tgz/download

wget -O aruco-$version.tgz https://downloads.sourceforge.net/project/aruco/OldVersions/aruco-1.3.0.tgz


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

if (( "$SUDOAVAILABLE" >= "1" ))
then 
sudo make install
sudo ldconfig
else
su -c "make install; ldconfig"
fi

echo "ArUco" $version "ready to be used"
cd ..

echo
echo
echo "Building USE Tracker"
make clean
make release

# install images & program
# $SU installmkdir -p /usr/share/useTracker
if (( "$SUDOAVAILABLE" >= "1" ))
then 
sudo mkdir -p /usr/share/useTracker/images
sudo install images/* /usr/share/useTracker/images/
sudo install ./bin/Release/useTracker /usr/bin/
else
su -c "mkdir -p /usr/share/useTracker/images; install images/* /usr/share/useTracker/images/; install ./bin/Release/useTracker /usr/bin/"
fi
