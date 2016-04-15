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

#include "Tracker.h"

#include "ImageProcessingEngine.h"
#include "Blob.h"

#include <fstream>

//#include "../dlib/optimization/max_cost_assignment.h"
//using namespace dlib;

using namespace std;
using namespace cv;


Tracker::~Tracker()
{
    CloseOutput();
}

void Tracker::Reset()
{
    SetMaxEntities(entitiesCount);
//    if (output) OpenOutput();
}

void Tracker::SetMaxEntities (unsigned int n)
{
    entities.clear();

    if (n == 0) n = 1;
    entitiesCount = n;

    // create a list of entities
    for (unsigned int i = 0; i < entitiesCount; i++)
    {
	entities.push_back(Entity(motionEstimatorLength));
    }
}

inline void Tracker::UpdateMotionEstimator (Entity& entity, const Entity& previousEntity)
{
    // static image
//    if (pipeline->parent->staticFrame) return;

    if (previousEntity.assigned)
    {
	unsigned int didx = entity.diffIdx;
	entity.dx[didx] = entity.x - previousEntity.x;
	entity.dy[didx] = entity.y - previousEntity.y;
	didx++;
	if (didx >= motionEstimatorLength) didx = 0;
	entity.diffIdx = didx;
    }
}

void Tracker::Apply()
{
    if (pipeline->parent->staticFrame) return;

    if (replay)
    {
	Replay();
    }
    else
    {
	Track();
    }
}

void Tracker::Replay()
{
    // locate data
    unsigned int currentFrame = pipeline->parent->capture->GetFrameNumber();

    // TODO DEBUG
//    cout << "Tracker : current frame=" << currentFrame << " h-start=" << historyStartFrame << " h-size=" << history.size() << endl;

    bool found = false;
    int eidx = -1; 
    
    if (currentFrame >= historyStartFrame)
    {
	if (historyEntriesIndex < historyEntries.size())
	{
	    // directly find correct frame, or search forward
	    for (unsigned int i = historyEntriesIndex; i < historyEntries.size(); i++)
	    {
		if (historyEntries[i].frameNumber == currentFrame)
		{
		    // found the entries, update current entities
		    eidx = historyEntries[i].entitiesIndex;
		    historyEntriesIndex = i;
		    found = true;	
		    break;
		}
	    }

	    // look for frame in previous entries
	    if (!found)
	    {
		for (unsigned int i = 0; i < historyEntriesIndex; i++)
		{
		    if (historyEntries[i].frameNumber == currentFrame)
		    {
			historyEntriesIndex = i;
			eidx = historyEntries[i].entitiesIndex;
			found = true;
			break;
		    }
		}
	    }
	}
    }

    if (found)
    {
	for (unsigned int e = 0; e < entitiesCount; e++)
	{
	    entities[e] = history[eidx+e];
	}
	historyEntriesIndex++;
    }
    else
	historyEntriesIndex = historyEntries.size();
}

void Tracker::Track()
{
    vector<Blob>& blobs = pipeline->parent->blobs;
    int frameNumber = pipeline->parent->capture->GetFrameNumber();

    // remember previous positions
    previousEntities = entities;
    for (unsigned int i = 0; i < entities.size(); i++) entities[i].assigned = false;

    // Estimate motion of entities based on previous observations
    for (unsigned int e = 0; e < entities.size(); e++)
    {
	// if entity is not in visible zone, don't move, just wait for reassignment
	if (entities[e].zone != ZONE_VISIBLE && !previousEntities[e].assigned) continue;

	// if entity not detected for a long time
	if (frameNumber - entities[e].lastFrameDetected >= motionEstimatorTimeout * pipeline->parent->capture->fps) continue;

	// extrapolate motion of  entities
	float dx = 0.0, dy = 0.0;
	for (unsigned int i = 0; i < motionEstimatorLength; i++)
	{
	    dx += entities[e].dx[i];
	    dy += entities[e].dy[i];
	}
	dx /= motionEstimatorLength;
	dy /= motionEstimatorLength;

	entities[e].x += dx;
	entities[e].y += dy;

	// prevent entity from going outside the picture
	if (entities[e].x < 0) entities[e].x = 0;
	else if (entities[e].x >= pipeline->width) entities[e].x = pipeline->width - 1;
	if (entities[e].y < 0) entities[e].y = 0;
	else if (entities[e].y >= pipeline->height) entities[e].y = pipeline->height - 1;

	// check if entity is in visible zone
	unsigned int idx = entities[e].x + entities[e].y * pipeline->width;
	entities[e].zone = pipeline->zoneMap.data[idx];

	// motion estimator
	unsigned int didx = entities[e].diffIdx;
	entities[e].dx[didx] = dx * extrapolationDecay;
	entities[e].dy[didx] = dy * extrapolationDecay;
	didx++;
	if (didx >= motionEstimatorLength) didx = 0;
	entities[e].diffIdx = didx;
    }

    // now for blobs/entities assignment, I will compute distances and rank couples to have best correspondances
    vector<Interdistance> interdistances;
    for (unsigned int b = 0; b < blobs.size(); b++)
    {
	if (blobs[b].available)
	{
	    for (unsigned int e = 0; e < entities.size(); e++)
	    {
		int distsq;

		int diffX = entities[e].x - blobs[b].x;
		int diffY = entities[e].y - blobs[b].y;
		distsq = diffX * diffX + diffY * diffY;

		interdistances.push_back (Interdistance(b, e, distsq));
	    }
	}
    }
    std::sort(interdistances.begin(), interdistances.end());

    // do the assignment work
    for (unsigned int d = 0; d < interdistances.size(); d++)
    {
	// get an interdistance, if blob and entities are free, establish the correspondance
	unsigned int b = interdistances[d].blobIdx;
	unsigned int e = interdistances[d].entityIdx;

	if (entities[e].assigned) continue;
	if (!blobs[b].available) continue;
	if (entities[e].toforget) continue;

	// now check if the blob is not too far
	// this test is not applied on first assignment
	if (entities[e].lastFrameDetected >= 0)
	{
	    float maxMotion = maxMotionPerSecond * (frameNumber - entities[e].lastFrameDetected) / pipeline->parent->capture->fps;
	    if (interdistances[d].distsq > maxMotion * maxMotion) continue;
	}

	bool createVirtualEntity = false;

	// if the entity is a virtual one, assigned long enough, promote it to a real one
	if (e >= entitiesCount && (frameNumber - entities[e].lastFrameNotDetected) > virtualEntitiesLifetime * pipeline->parent->capture->fps)
	{
	    // find the corresponding real entity
	    entities[e].toforget = true;
	    int linkedEntity = entities[e].linkedEntity;
	    entities[linkedEntity] = entities[e];

	    // not assigned, the virtual e will be erased
	    e = linkedEntity;
	}

	// if the entity was lost/frozen for a long time, wait before assigning and use a virtual entity instead (long code !!!)
	else if (!previousEntities[e].assigned)
	{
	    if (virtualEntitiesZone && entities[e].zone != ZONE_VISIBLE) createVirtualEntity = true;
	    else if (virtualEntitiesDistsq >= 1 && interdistances[d].distsq > virtualEntitiesDistsq) createVirtualEntity = true;
	    else if (frameNumber - entities[e].lastFrameDetected > virtualEntitiesDelay * pipeline->parent->capture->fps) createVirtualEntity = true;
	}

	if (createVirtualEntity && useVirtualEntities)
	{
	    entities[e].toforget = true;
	    entities.push_back(Entity(motionEstimatorLength));
	    entities.back().linkedEntity = e;
	    entities.back().lastFrameNotDetected = frameNumber - pipeline->parent->timestep * pipeline->parent->capture->fps;
	    e = entities.size() - 1;
	}

	// ok all tests passed, make it a detected entity
	entities[e].x = blobs[b].x;
	entities[e].y = blobs[b].y;
	entities[e].size = blobs[b].size;
	entities[e].assigned = true;
	entities[e].zone = blobs[b].zone;
//	    entities[e].zone = ZONE_VISIBLE;
	blobs[b].available = false;
	blobs[b].assignment = e;
	entities[e].lastFrameDetected = frameNumber;

	if (e < previousEntities.size())
	    UpdateMotionEstimator (entities[e], previousEntities[e]);
    }

    // erase undetected virtual entities
    for (unsigned int e = entities.size() - 1; e >= entitiesCount; e--)
    {
	if (!entities[e].assigned)
	{
	    entities[ entities[e].linkedEntity ].toforget = false;
	    for (unsigned int i = e; i < entities.size() - 1; i++) entities[i] = entities[i+1];
	    entities.resize(entities.size() - 1);
	}
    }

    // and reset counters if not detected
    for (unsigned int e = 0; e < entities.size(); e++)
    {
	if (!entities[e].assigned) entities[e].lastFrameNotDetected = frameNumber;
    }

    // if (textStream.is_open()) outputText (textStream, entities, frameNumber, video->GetFrameTime());


    // save to history
//    cout << "Tracker : " << "saved data from frame " << pipeline->parent->capture->GetFrameNumber() << endl;

    if (history.empty()) historyStartFrame = frameNumber;

//    cout << "Tracked frame " << pipeline->parent->capture->GetFrameNumber() << " hindex" << history.size() / entitiesCount << " hstart = " << historyStartFrame << endl;

    historyEntriesIndex = historyEntries.size();
    historyEntries.push_back(HistoryEntry (frameNumber, history.size()));
    for (unsigned int e = 0; e < entitiesCount; e++)
    {
    	history.push_back(entities[e]);
    }
}

void Tracker::OutputHud (Mat& hud)
{
    char str[8];
    Point pos;

    // draw a trail if history available
    // first, draw past positions
    vector<Point> last;

    for (unsigned int i = 0; i < entitiesCount; i++)
    {
	last.push_back(Point(-1,-1));
    }

    // draw past trail if possible (data available)
    if (historyEntriesIndex < historyEntries.size())
    {
	for (unsigned int h = historyEntriesIndex - trailLength; h <= historyEntriesIndex; h++)
	{
	    if (h >= 0 && h < historyEntries.size())
	    {
		int ei = historyEntries[h].entitiesIndex;
		for (unsigned int e = 0; e < entitiesCount; e++)
		{
		    pos.x = history[ei+e].x;
		    pos.y = history[ei+e].y;

		    if (last[e].x >= 0)
		    {
			line(hud, pos, last[e], cvScalar(32,32,32,255), 2, CV_AA);
		    }
		    last[e] = pos;
		}
//		cout << "past trail at h=" << h <<endl;
	    }
	}
    }

    // then, try to draw future positions if in replay mode
    if (replay)
    {
	for (unsigned int h = historyEntriesIndex; h < historyEntriesIndex + trailLength; h++)
	{
	    if (h >= 0 && h < historyEntries.size())
	    {
		int ei = historyEntries[h].entitiesIndex;
		for (unsigned int e = 0; e < entitiesCount; e++)
		{
		    pos.x = history[ei+e].x;
		    pos.y = history[ei+e].y;

		    if (last[e].x >= 0)
		    {
			line(hud, pos, last[e], cvScalar(255,255,255,255), 2, CV_AA);
		    }
		    last[e] = pos;
		}
//		cout << "future trail at h=" << h <<endl;
	    }
	}
    }

    // plot id number, if possible
//    if (!replay || historyEntriesIndex < historyEntries.size())
//    {
//	unsigned int ei = historyEntries[historyEntriesIndex].entitiesIndex;
	for (unsigned int e = 0; e < entitiesCount; e++)
	{
	    sprintf (str, "%d", e);
	    pos.x = entities[e].x;
	    pos.y = entities[e].y;
	    putText(hud, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str, pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);
	}
//    }
}

void Tracker::OutputStep ()
{
    if (outputStream.is_open() && !replay)
    {
	for (unsigned int e = 0; e < entitiesCount; e++)
	{
	    outputStream
		<< pipeline->parent->capture->GetTime() << "\t"
		<< pipeline->parent->capture->GetFrameNumber() << "\t"
		<< entities[e].lastFrameDetected << "\t"
		<< entities[e].lastFrameNotDetected << "\t"
		<< e << "\t"
		<< entities[e].assigned << "\t"
		<< entities[e].zone << "\t"
		<< entities[e].size << "\t"
		<< entities[e].x << "\t"
		<< entities[e].y
		<< std::endl;
	}
    }
}


void Tracker::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), std::ios::out);
	if (outputStream.is_open())
	{
	    outputStream << "time \t frame \t lastFrameDetected \t lastFrameNotDetected \t entity \t assigned \t zone \t size \t x \t y " << std::endl;
	}
    }
}

void Tracker::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void Tracker::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];

	minInterdistance = (float)fn["MinInterDistance"];
	maxMotionPerSecond = (float)fn["MaxMotionPerSecond"];
	extrapolationDecay = (float)fn["ExtrapolationDecay"];
	motionEstimatorLength = (int)fn["ExtrapolationHistorySize"];
	motionEstimatorTimeout = (float)fn["ExtrapolationTimeout"];

	entitiesCount = (int)fn["EntitiesCount"];

	trailLength = (int)fn["HistoryTrailLength"];

	FileNode fn2 = fn["VirtualEntities"];

	if (!fn2.empty())
	{
	    useVirtualEntities = (int)fn2["Active"];
	    virtualEntitiesDelay = (float)fn2["DelayToCreation"];
	    virtualEntitiesZone = (int)fn2["Zone"];
	    float v = (float)fn2["Distance"];
	    virtualEntitiesDistsq = v * v;
	    virtualEntitiesLifetime = (float)fn2["PromotionTime"];
	}
   }
}

void Tracker::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;

    fs << "EntitiesCount" << (int)entitiesCount;
    fs << "MinInterdistance" << minInterdistance;
    fs << "MaxMotionPerSecond" << maxMotionPerSecond;
    fs << "ExtrapolationDecay" << extrapolationDecay;
    fs << "ExtrapolationHistorySize" << (int)motionEstimatorLength;
    fs << "ExtrapolationTimeout" << motionEstimatorTimeout;

    fs << "HistoryTrailLength" << (int)trailLength;

    // we don't save that if we don't use it...
    if (useVirtualEntities)
    {
	fs << "VirtualEntities" << "{";

	fs << "Active" << useVirtualEntities;
	fs << "DelayToCreation" << virtualEntitiesDelay;
	fs << "Zone" << virtualEntitiesZone;
	fs << "PromotionTime" << virtualEntitiesLifetime;
	fs << "Distance" << sqrt(virtualEntitiesDistsq);

	fs << "}";
    }
}

void Tracker::SetReplay(bool enable)
{
    replay = enable;

    if (!replay) ClearHistory();
}

void Tracker::ClearHistory()
{
//    cout << "===============================================Clearing history=======================================================================" << endl;

    historyStartFrame = pipeline->parent->capture->GetFrameNumber();
//    cout << "H start = " << historyStartFrame << endl;
    history.clear();
    historyEntries.clear();
    historyEntriesIndex = 0;
}

void Tracker::LoadHistory(string filename)
{
    std::ifstream file(filename);
    std::string str;

    // eat up first line
    for (int i = 0; i < 10; i++)
	file >> str;

    history.clear();

    // read first line separately
    int count = 0;
    unsigned int frameNumber;
    Entity e;
    int readEntities = 0;

    if (!file.eof())
    {
	file >> str >> historyStartFrame >> e.lastFrameDetected >> e.lastFrameNotDetected >> str
	     >> e.assigned >> e.zone >> e.size >> e.x >> e.y;

	historyEntries.push_back(HistoryEntry(historyStartFrame, 0));

	do
	{
	    history.push_back(e);
	    readEntities++;
	    
	    file >> str >> frameNumber >> e.lastFrameDetected >> e.lastFrameNotDetected >> str
		 >> e.assigned >> e.zone >> e.size >> e.x >> e.y;
	    
	    if (count == 0)
		if (frameNumber != historyStartFrame)
		    count = history.size();
	    
	    if (count > 0 && (readEntities % count == 0))
	    {
		historyEntries.push_back(HistoryEntry(frameNumber, history.size()-1));

	    }
	    
	} while (!file.eof()); // if eof is reached while parsing data, do not record the entity
    }

    SetMaxEntities (count);
}
