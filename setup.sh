#!/usr/bin/env bash

# this script aims at configuring a computer to run the FaMouS simulator

function error
{
    echo "${PROGNAME}: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# make sure the following dependencies have been installed 
echo The setup program of the USE Tracker is about to update your system
sudo apt-get -qq update
sudo apt-get -qq upgrade

echo
echo
echo "Installing Dependencies"
#sudo apt-get -qq install make g++ automake freeglut3-dev libboost-program-options-dev libopencv-dev build-essential pkg-config libjpeg-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libgtk2.0-dev x264 libx264-dev v4l-utils ffmpeg || error "$LINENO: Aborted, could not install the required dependencies."

echo
echo
echo "Building USE Tracker
make release
