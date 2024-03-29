#!/usr/bin/env bash

# if your local copy is modified and you want to ensure it is the same as the master, you can erase all the changes by running
# git reset --hard origin/master

# find location of the script, which should be root of useTracker
# from http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
 
cd $DIR
 
echo Updating > update_status.txt
 
(
echo "# Pulling update from github repository" ; git pull &> update_status.txt
error=$(cat update_status.txt | grep Aborting | wc -l)

wxversion=3.0

# make sure you all necessary packages installed
#if [ $error -eq 0 ]; then
#zenity --info --text "About to update required package dependencies"
#(echo "# Updating packages..."; pkexec apt-get --yes install build-essential cmake premake4 libwxbase$wxversion-dev libopencv-dev libboost-program-options-dev libboost-filesystem-dev libwxgtk$wxversion-dev libwxgtk-media$wxversion-dev freeglut3-dev libva-dev libbz2-dev libx264-dev libbluetooth-dev
#) |
#zenity --progress \
#  --title="Updating package dependencies for USE Tracker" \
#  --progress\
#  --auto-close\
#  --pulsate
#fi

if [ $error -eq 0 ]; then
echo "# Cleaning sources"; make clean &> update_status.txt
echo "# Building arucoColor"; cd arucoColor; premake4 gmake &> update_status.txt; make config=release clean &> update_status.txt; make config=release &> update_status.txt; cd ..
echo "# Building from updated sources"; make release &> update_status.txt
fi
) |
zenity --progress \
  --title="Updating USE Tracker" \
  --progress\
  --auto-close\
  --pulsate
 
error=$(cat update_status.txt | grep Aborting | wc -l)
if [ $error != 0 ] ; then
        zenity --error \
          --text="Could not pull files from GitHub.com\nUpdate canceled."
exit -1
fi
 
error=$(ls ./bin/Release/useTracker | wc -l)
if [ $error -eq 0 ]; then
        zenity --error \
          --text="Could not compile USE Tracker.\nUpdate canceled."
exit -1
fi
 
#zenity --info --text "About to install USE Tracker to system directory"
#(echo "# Installing"; gksudo -S -m "Installing USE Tracker to system directories" "cp ./bin/Release/useTracker /usr/bin/"
#) |
zenity --info --text "About to install USE Tracker to system directory"
(echo "# Installing"; pkexec install $(pwd)/bin/Release/useTracker /usr/bin/; pkexec install $(pwd)/images/* /usr/share/useTracker/images/
) |
zenity --progress \
  --title="Updating USE Tracker" \
  --progress\
  --auto-close\
  --pulsate
 
m1=$(md5sum ./bin/Release/useTracker | cut -f 1 -d ' ')
m2=$(md5sum /usr/bin/useTracker | cut -f 1 -d ' ')
if [ $m1 != $m2 ]; then
        zenity --error \
          --text="Could not install USE Tracker.\nPlease close all instances of USE Tracker or try to restart.\nUpdate canceled."
exit -1
fi
 
zenity --info --text="Update successful."
