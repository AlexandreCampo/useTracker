/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#include "RemoteControl.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <iostream>

#include "ImageProcessingEngine.h"
#include "Blob.h"

using namespace cv;
using namespace std;

RemoteControl::RemoteControl () : PipelinePlugin()
{
    btStatus = 0;
}

RemoteControl::~RemoteControl()
{
    if (btStatus > 0)
	close (btSocket);
}

void RemoteControl::Reset()
{
//    multithreaded = true;
//    SetSize(size);

    btAddress = string("00:07:80:80:19:6C");

    // allocate a socket
    btSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    struct sockaddr_rc addr = { 0 };
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(btAddress.c_str(), &addr.rc_bdaddr );

    // connect to server
    btStatus = connect(btSocket, (struct sockaddr *)&addr, sizeof(addr));

    // send a message
    if( btStatus == 0 )
    {
	string str ("[=0,0]");
        btStatus = write(btSocket, str.c_str(), str.length());
    }

    if (btStatus < 0) std::cout << "Looks like bt connection failed..." << std::endl;
}

void RemoteControl::Apply()
{
    /// Apply the dilation operation
    // erode (pipeline->marked, result, structuringElement);
    // result.copyTo(pipeline->marked);

//    cout << "In apply method " << btStatus << endl;

    // if we are connected
    if (btStatus > 0)
    {
	// locate the robot in the picture
	int x, y;
	float angle;
	bool perceived = false;
	vector<Blob>& blobs = pipeline->parent->blobs;
	for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
	{
	    if (b->available && (b->tagId == 33))
	    {
		perceived = true;
		x = b->x;
		y = b->y;
		angle = b->angle;
	    }
	}

	int fr = 0, fl = 0;

	if (perceived)
	{	
	    // try to drive it from one corner to the other. Switch when close enough (d < 100)
	    int cx, cy;
	    if (corner == 0)
	    {
		cx = 200; cy = 200;
	    }
	    else if (corner == 1)
	    {
		cx = 1720; cy = 200;
	    }
	    else if (corner == 2)
	    {
		cx = 1720; cy = 880;
	    }
	    else
	    {
		cx = 200; cy = 880;
	    }
	    
	    // get distance to current corner
	    int dx = cx - x;
	    int dy = cy - y;
	    float d = sqrt (dx*dx + dy*dy);
	    
	    
	    if (d < 100) 
	    {
		corner++;
		if (corner >= 4) corner = 0;
	    }
	    else
	    {
		// get relative angle to corner
		float absAngle = atan2 (dy, dx);
		
		// go forward speed 30;
		float speed = 50;
		float relativeAngle = absAngle - angle;
		
		if (relativeAngle > M_PI) relativeAngle -= 2.0 * M_PI;
		if (relativeAngle <= -M_PI) relativeAngle += 2.0 * M_PI;
	
		speed *= cos (relativeAngle);

		if (fabs(speed) < 15)
		{
		    if (speed < 0) speed = -15;
		    else speed = 15;
		}

		// bend trajectory towards the goal
		if (speed > 0)
		{
		    if (relativeAngle < 15.0 * M_PI / 180.0) {fr -= 10; fl += 10;}
		    else if (relativeAngle > 15.0 * M_PI / 180.0) {fr += 10; fl -= 10;}
		}
		else
		{
		    if (relativeAngle > 15.0 * M_PI / 180.0) {fr -= 10; fl += 10;}
		    else if (relativeAngle < 15.0 * M_PI / 180.0) {fr += 10; fl -= 10;}
		}
		
		fr += speed;
		fl += speed;
		
		cout << "Current force " << fr << " " << fl << " | speed " << speed << " angle " << angle * 180.0 / M_PI << " relangle " << relativeAngle * 180.0 / M_PI << endl;
		
	    }
	}

	// drive towards the corner
	char str[32];
	sprintf (str, "[=%d,%d]\n\0", fr, fl);
	btStatus = write(btSocket, str, strlen(str));
    }
}

void RemoteControl::OutputHud (Mat& hud)
{
    vector<Blob>& blobs = pipeline->parent->blobs;
    for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
    {
	if (b->available && (b->tagId == 33))
	{
	    Point pos,pos2;
	    int sqlen = 30;
	    pos.x = b->x;
	    pos.y = b->y;
	    pos2.x = b->x + cos(b->angle) * sqlen;
	    pos2.y = b->y + sin(b->angle) * sqlen;

	    line(hud, pos, pos2, cvScalar(255, 0, 127,255), 1);

	    string str (".RC");
	    putText(hud, str.c_str(), pos+Point(4,4), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str.c_str(), pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);

	    int cx, cy;
	    if (corner == 0)
	    {
		cx = 200; cy = 200;
	    }
	    else if (corner == 1)
	    {
		cx = 1720; cy = 200;
	    }
	    else if (corner == 2)
	    {
		cx = 1720; cy = 880;
	    }
	    else
	    {
		cx = 200; cy = 880;
	    }

	    pos.x = cx;
	    pos.y = cy;

	    str = string (".Goal");
	    putText(hud, str.c_str(), pos+Point(4,4), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str.c_str(), pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);

	}
    }
}

void RemoteControl::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	btAddress = (string)fn["btAddress"];
    }
}

void RemoteControl::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "btAddress" << btAddress;
}

