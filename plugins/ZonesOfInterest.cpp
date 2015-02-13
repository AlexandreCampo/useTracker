
#include "ZonesOfInterest.h"


using namespace cv;

ZonesOfInterest::ZonesOfInterest() : PipelinePlugin()
{
    multithreaded = true;
}

void ZonesOfInterest::Reset()
{
}

void ZonesOfInterest::Apply()
{
    pipeline->marked &= pipeline->zoneMap;
}

void ZonesOfInterest::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
    }
}

void ZonesOfInterest::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
}

