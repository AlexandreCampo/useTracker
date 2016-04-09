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

#ifndef TRACKER_H
#define TRACKER_H


#include "PipelinePlugin.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>

// extern Parameters parameters;

struct Tracker : public PipelinePlugin
{
    // helper public classes
    struct Entity
    {
	int x, y;
	int size;
	std::vector<float> dx, dy;
	int diffIdx;
	bool assigned;
	bool toforget;
	int zone;
	int lastFrameDetected;
	int lastFrameNotDetected;
	int linkedEntity; // used in case this is a virtual entity

	Entity (int motionEstimatorLength = 0)
	    {
		x = 0;
		y = 0;
		size = 0;
		assigned = false;
		zone = 1;//ZONE_VISIBLE;
		lastFrameDetected = -1000;
		lastFrameNotDetected = 0;
		linkedEntity = -1;
		toforget = false;
		diffIdx = 0;
//		ResizeHistory(motionEstimatorLength);
		dx.resize(motionEstimatorLength);
		dy.resize(motionEstimatorLength);
	    }
	void ResizeHistory(int motionEstimatorLength)
	    {
		dx.clear();
		dy.clear();
		for (int i = 0; i < motionEstimatorLength; i++)
		{
		    dx.push_back(0);
		    dy.push_back(0);
		}
	    }
    };

    class Interdistance
    {
    public:
	int blobIdx, entityIdx;
	int distsq;

	Interdistance (int b, int e, int d)
	{
	    blobIdx = b;
	    entityIdx = e;
	    distsq = d;
	}
	bool operator<(const Interdistance &dp) const { return distsq < dp.distsq; }
    };

    struct HistoryEntry
    {
	HistoryEntry(unsigned int f, unsigned int i)
	    {
		frameNumber = f;
		entitiesIndex = i;
	    }
	unsigned int frameNumber;
	unsigned int entitiesIndex;
    };

    // class members
    unsigned int entitiesCount = 1;
    std::vector<Entity> entities;
    std::vector<Entity> previousEntities;
    std::fstream outputStream;
    std::string outputFilename;

    float minInterdistance = 5.0;
    float maxMotionPerSecond = 800;
    float extrapolationDecay = 0.8;
    unsigned int motionEstimatorLength = 10;
    float motionEstimatorTimeout = 1.0;

    bool useVirtualEntities = false;
    float virtualEntitiesLifetime = 1.0;
    float virtualEntitiesDelay = 0.3;
    bool virtualEntitiesZone = false;
    float virtualEntitiesDistsq = 0.0;

    // history & replay
    bool replay = false;
    std::vector<Entity> history;
    std::vector<HistoryEntry> historyEntries;
    unsigned int historyEntriesIndex = 0;
    unsigned int historyStartFrame = 0;
    unsigned int trailLength = 10;

    // methods
    ~Tracker();

    void SetMaxEntities(unsigned int n);
    inline void UpdateMotionEstimator (Entity& entity, const Entity& previousEntity);

    void Reset();
    void Apply();
    void Track();
    void Replay();

    void SetReplay(bool enable);
    void LoadHistory(std::string file);
    void ClearHistory();

    void OutputStep();
    void OpenOutput();
    void CloseOutput();
    void OutputHud (cv::Mat& hud);

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};


#endif
