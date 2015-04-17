
#include "ExtractBlobs.h"

#include "Blob.h"
#include "ImageProcessingEngine.h"

#include <vector>
//#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

ExtractBlobs::~ExtractBlobs()
{
    CloseOutput();
}

void ExtractBlobs::Reset()
{
//    if (output) OpenOutput();
}

void ExtractBlobs::Apply()
{
    // reset labels if necessary
    if (recordLabels) 
	pipeline->labels.setTo (-1);

    // remove matches with mask
//    pipeline->marked &= pipeline->zoneMap;

    vector<Blob>& blobs = pipeline->parent->blobs;
    blobs.clear();

    // create empty segments
    vector<Segment> seg1;
    vector<Segment> seg2;

    // preallocate to max size
    seg1.resize(pipeline->width/2);
    seg2.resize(pipeline->width/2);

    vector<Segment>* segPtr = &seg1;
    vector<Segment>* psegPtr = &seg2;

    Mat& marked = pipeline->marked;

    unsigned int index = 0;
    unsigned int sidx = 0;
    unsigned int psidx = 0;
    for (int y = 0; y < pipeline->height; y++)
    {
	// scan the line
        auto& seg = *segPtr;
	unsigned int size = 0;
	sidx = 0;
	for (int x = 0; x < pipeline->width; x++)
	{
	    if (size > 0)
	    {
		if (marked.data[index] == 0)
		{
		    seg[sidx].max = x;
		    seg[sidx].size = size;
		    seg[sidx].sumX = seg[sidx].min * size + ((size * (size - 1)) / 2);
		    seg[sidx].sumY = size * y;
		    sidx++;
		    size = 0;
		}
		else
		{
		    size++;
		}
	    }
	    else
	    {
		if (marked.data[index] > 0)
		{
		    seg[sidx].min = x;
		    size++;
		}
	    }
	    index++;
	}
	// record last segment if touching border
	if (size > 0)
	{
	    seg[sidx].max = pipeline->width;
	    seg[sidx].size = size;
	    seg[sidx].sumX = seg[sidx].min * size + ((size * (size - 1)) / 2);
	    seg[sidx].sumY = size * y;
	    sidx++;
	}

//	cout << "Line " << y << ", segments : ";

	// now check all segments wrt to past line to update blobs
	unsigned int minpsi = 0;
	for (unsigned int si = 0; si < sidx; si++)
	{
	    Segment& s = (*segPtr)[si];
	    bool matched = false;

//	    cout << s.min << ":" << s.max << " ";

	    for (unsigned int psi = minpsi; psi < psidx; psi++)
	    {
		Segment& ps = (*psegPtr)[psi];

		if (s.max > ps.min)
		{
		    if (s.min < ps.max)
		    {
			// found a matching segment
			if (!matched)
			{
//			    cout << " over " << ps.min << ":" << ps.max << ":" <<  blobs[ps.blobIdx].size << ":" << blobs[ps.blobIdx].available << ":" << ps.blobIdx << " | ";

			    // first match, merge segment to existing blob
			    s.blobIdx = ps.blobIdx;
			    blobs[s.blobIdx].size += s.size;
			    blobs[s.blobIdx].x += s.sumX;
			    blobs[s.blobIdx].y += s.sumY;
			    matched = true;
			}
			else
			{
			    // more than one match, merge two blobs, unless already the same...
			    if (s.blobIdx != ps.blobIdx)
			    {
//				cout << "over+ " << ps.min << ":" << ps.max << ":" << blobs[ps.blobIdx].size << ":" << blobs[ps.blobIdx].available  << ":" << ps.blobIdx << " becomes " << blobs[s.blobIdx].size << ":" << blobs[s.blobIdx].available << ":" << s.blobIdx << " update ";

				blobs[s.blobIdx].size += blobs[ps.blobIdx].size;
				blobs[s.blobIdx].x += blobs[ps.blobIdx].x;
				blobs[s.blobIdx].y += blobs[ps.blobIdx].y;
				blobs[ps.blobIdx].available = false;

				// update blobIdx for all segments
				unsigned int oldIdx = ps.blobIdx;
				for (unsigned int ssi = 0; ssi < sidx; ssi++)
				    if ((*segPtr)[ssi].blobIdx == oldIdx)
				    {
//					cout << "s" << ssi << " ";
					(*segPtr)[ssi].blobIdx = s.blobIdx;
				    }

				for (unsigned int pssi = minpsi; pssi < psidx; pssi++)
				    if ((*psegPtr)[pssi].blobIdx == oldIdx)
				    {
					(*psegPtr)[pssi].blobIdx = s.blobIdx;
//					cout << "p" << pssi << " ";
				    }

				// also update blobIdx for all blobs
				for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
				    if (b->assignment == oldIdx)
					b->assignment = s.blobIdx;
			    }
			}
		    }
		    else
		    {
			// next segments will also be behind of this one at least
			minpsi = psi + 1;
		    }
		}
		else
		{
		    // other segments are ahead by construction...
		    break;
		}
	    }
	    // did not find a corresponding segment ?
	    if (!matched)
	    {
		// create a blob
		Blob b (0, 0, 0);
		b.size = s.size;
		b.x = s.sumX;
		b.y = s.sumY;
		b.assignment = blobs.size();
		s.blobIdx = b.assignment;
		blobs.push_back(b);

//		cout << "not matched, creates b" << s.blobIdx << " | ";
	    }
	    // if recording labels, write down the numbers...
	    if (recordLabels)
	    {
		int* row = pipeline->labels.ptr<int>(y);
		for (unsigned int x = s.min; x < s.max; x++)
		{
		    row[x] = (int)s.blobIdx;
		}
	    }
	}
//	cout << endl;

	// last step: swap scanlines
	vector<Segment>* tmp = psegPtr;
	psegPtr = segPtr;
	segPtr = tmp;
	psidx = sidx;
    }

    // finally update blobs positions and validate their size
//    int debug = 0;
    for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
    {
	if (b->available)
	{
	    if ((b->size >= minSize) && (b->size <= maxSize || maxSize == 0))
	    {
		b->x /= b->size;
		b->y /= b->size;
		b->zone = pipeline->zoneMap.at<unsigned char>(b->y, b->x);
	    }
	    else
	    {
		b->available = false;
	    }
	}
//	cout << "Blob " << debug << " " << b->x << " " << b->y << " " << b->size << " " << b->available << " " << b->assignment << endl;
//	debug++;
    }

    //  cout << "Found a total of " << blobs.size() << " blobs of which " << debug << " are valid" << endl;
}


void ExtractBlobs::OutputHud (Mat& hud)
{
    cout << "new output iteration " << endl;

    char str[32];
    Point pos;
    vector<Blob>& blobs = pipeline->parent->blobs;
    for (auto b : blobs)
    {
	if (b.available)
	{
	    sprintf (str, "%d", b.size);
	    pos.x = b.x;
	    pos.y = b.y;
	    int sqlen = sqrt(b.size) / 2;
	    cout << "Display bob " << " " << b.x << " " << b.y << " " << b.size << endl;
	    rectangle(hud, pos-Point(sqlen,sqlen), pos+Point(sqlen,sqlen), cvScalar(127, 127, 127, 255), CV_FILLED);
	    putText(hud, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0, 255), 2, CV_AA);
	    putText(hud, str, pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200, 255), 2, CV_AA);
	}
    }
}

void ExtractBlobs::OutputStep()
{
    // in any case, also output blob list with characs...
    if (outputStream.is_open())
    {
	vector<Blob>& blobs = pipeline->parent->blobs;
	for (auto b : blobs)
	{
	    if (b.available)
	    {
		outputStream 
		    << pipeline->parent->capture->GetTime() << "\t" 
		    << pipeline->parent->capture->GetFrameNumber() << "\t" 
		    << b.x << "\t" 
		    << b.y << "\t" 
		    << b.angle << "\t" 
		    << b.size << "\t" 
		    << b.zone 
		    << std::endl;
	    }
	}
    }
}

void ExtractBlobs::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void ExtractBlobs::OpenOutput()
{
    // open normal output
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), std::ios::out);
	if (outputStream.is_open())
	{
	    outputStream << "time \t frame \t blob_x \t blob_y \t blob_angle \t blob_size \t blob_zone" << std::endl;
	}
    }
}

void ExtractBlobs::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	minSize = (int)fn["MinSize"];
	maxSize = (int)fn["MaxSize"];
	recordLabels = (int)fn["RecordLabels"];
	outputFilename = (string)fn["OutputFilename"];
   }
}

void ExtractBlobs::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "MinSize" << (int)minSize;
    fs << "MaxSize" << (int)maxSize;
    fs << "OutputFilename" << outputFilename;
    fs << "RecordLabels" << recordLabels;
}
