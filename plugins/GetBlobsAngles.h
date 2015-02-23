#ifndef GETBLOBSANGLES_H
#define GETBLOBSANGLES_H

#include "PipelinePlugin.h"


class GetBlobsAngles : public PipelinePlugin
{
public:

    struct StatData
    {
	StatData()
	    {
		cov = 0;
		varx = 0;
		vary = 0;
		maxdist = 0;
	    }

	int cov;
	int varx;
	int vary;
	int maxdist;
    };

    GetBlobsAngles();

    void Apply();
    void OutputHud (cv::Mat& hud);

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
