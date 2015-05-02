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

#include "GetBlobsAngles.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Blob.h"
#include "ImageProcessingEngine.h"
#include <vector>

using namespace cv;
using namespace std;

GetBlobsAngles::GetBlobsAngles () : PipelinePlugin()
{
}

void GetBlobsAngles::Apply()
{
    vector<Blob>& blobs = pipeline->parent->blobs;
    vector<StatData> stats;
    stats.resize(blobs.size());

    // scan the whole labels, and calculate blobs data
    for (int y = 0; y < pipeline->height; y++)
    {
	int* row = pipeline->labels.ptr<int>(y);

	for (int x = 0; x < pipeline->width; x++)
	{
	    if (row[x] >= 0)
	    {
		unsigned int idx = blobs[ row[x] ].assignment;

		int dx = x - blobs[idx].x;
		int dy = y - blobs[idx].y;

		stats[idx].cov += dx * dy;
		stats[idx].varx += dx * dx;
		stats[idx].vary += dy * dy;

		int dist = dx * dx + dy * dy;

		if (dist > stats[idx].maxdist)
		    stats[idx].maxdist = dist;

		// Point pos;
		// // pos.x = x;
		// // pos.y = y;
		// pos.x = blobs[idx].x;
		// pos.y = blobs[idx].y;
		// line(pipeline->parent->hud, pos, pos+Point(1,1), cvScalar(200, 100, 200), 2);
//		cout << "CoM " << x << " " << y << " = " << pos.x << " " << pos.y << endl;
	    }
	}
    }

    // now scan the blobs and calculate their angles
    for (unsigned int i = 0; i < blobs.size(); i++)
    {
	if (blobs[i].available)
	{
	    if (stats[i].varx > stats[i].vary)
	    {
		blobs[i].angle = atan2 ((float)stats[i].cov / (float)stats[i].varx, 1);
	    }
	    else
	    {
		blobs[i].angle = atan2 (1, (float)stats[i].cov / (float)stats[i].vary);
	    }
	    blobs[i].length = sqrt((float)stats[i].maxdist) * 2.0;
//	    cout << "Blob " << i << " angle " << blobs[i].angle << " length " << blobs[i].length  << " params " << stats[i].cov << " " << stats[i].varx << " " << stats[i].vary << endl;
	}
    }
//    cout << "--------" << endl;
}

void GetBlobsAngles::OutputHud (Mat& hud)
{
    char str[32];
    Point pos;
    Point pos2;
    vector<Blob>& blobs = pipeline->parent->blobs;
    for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
    {
	if (b->available)
	{
	    sprintf (str, "%d", b->size);
	    int sqlen = sqrt(b->size) / 2;
	    pos.x = b->x;
	    pos.y = b->y;
	    pos2.x = b->x + cos(b->angle) * sqlen;
	    pos2.y = b->y + sin(b->angle) * sqlen;

//	    cout << "Blob " << " angle " << b->angle << endl;

	    rectangle(hud, pos-Point(sqlen,sqlen), pos+Point(sqlen,sqlen), cvScalar(127, 127, 127,255), CV_FILLED);
	    line(hud, pos, pos2, cvScalar(255, 0, 127,255), 1);
	    putText(hud, str, pos+Point(4,4), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str, pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);
	}
    }
}


void GetBlobsAngles::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
    }
}

void GetBlobsAngles::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
}
