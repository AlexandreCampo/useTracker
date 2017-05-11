

#include "dictionary.h"

#include <map>
#include <vector>
#include <algorithm>

#include <iostream>

using namespace std;

namespace ac
{

Dictionary::Dictionary(int dimensions, std::vector<int>& referenceHues)
{
    this->dimensions = dimensions;
    
    // build a lookup table to quickly convert hue to ref color
    for (int i = 0; i <= 180*3; i++)
	huesToColor.push_back(-1);
    
    int n = referenceHues.size();
    
    // add first hue as last hue to deal with circularity
    referenceHues.push_back(referenceHues[0] + 180);
    
    // for each pair of successive hues
    for (unsigned int i = 0; i < n; i++)
    {
	int wd = (referenceHues[i+1] - referenceHues[i]) / 2;
	for (unsigned int h = referenceHues[i]; h < referenceHues[i+1]; h++)
	{
	    int hh = h % 180;
	    
	    if (h < referenceHues[i] + wd) huesToColor[hh] = i;
	    else huesToColor[hh] = (i+1) % n;
	}
    }
    // copy first 180 items 3 times so that we access the buffer below and above 0-180 limits
    for (unsigned int i = 0; i < 180; i++)
    {
	huesToColor[i+180] = huesToColor[i];
	huesToColor[i+360] = huesToColor[i];
    }
}

Dictionary::~Dictionary()
{
    
}

void Dictionary::add(int code, int id)
{
    sorted = false;
    
    Marker m (code, id);

    for (int i = 0; i < 4; i++)
    {
	markers.push_back(m);
	m.rotate();
    }
}

void Dictionary::sort()
{
    // sort markers
    std::sort(markers.begin(), markers.end(),
	      [](const Marker& a, const Marker& b)
	      {
		  return a.code < b.code;   
	      }
	);

    // make a mirror vector with only codes as ints
    codes.clear();
    for (auto m : markers)
	codes.push_back(m.code);
}

void Dictionary::clear()
{
    markers.clear();
}
    
int Dictionary::findBestMarker(const vector<int>& hues, int maxHueDist, int maxMarkerDist, int& dist)
{
    // make sure markers are sorted
    if (!sorted) sort();
    
    // build a list of candidates    
    vector<vector<int>> candidates;
    vector<int> candidatesDistances;
    vector<vector<int>> nextCandidates;
    vector<int> nextCandidatesDistances;

    vector<int> quantized;

    for (int d = 0; d < dimensions; d++)
     	quantized.push_back(huesToColor[hues[d]]);

    
    // for each pixel / dimension, evaluate the possible hues
    for (unsigned int d = 0; d < dimensions; d++)
    {
	int h = hues[d] + 180;
	
	int minc = huesToColor[h-maxHueDist];
	int maxc = huesToColor[h+maxHueDist];

	if (maxc < minc) maxc += Marker::numHues;
	
	// empty list, just push the possibilities
	if (candidates.size() == 0)
	{
	    for (int j = minc; j <= maxc; j++)
	    {
		int c = j % Marker::numHues;
		std::vector<int> candidate (1, c);
		candidates.push_back(candidate);

		int diff = abs(quantized[d] - c);
		int distance = Marker::distances[diff];		
		candidatesDistances.push_back(distance);
	    }
	}
	else
	{
	    nextCandidates.clear();
	    nextCandidatesDistances.clear();
	    		
	    for (int j = minc; j <= maxc; j++)
	    {
		int c = j % Marker::numHues;
		int diff = abs(quantized[d] - c);
		int distance = Marker::distances[diff];
		
		for (int i = 0; i < candidates.size(); i++)
		{
		    int candidateDistance = distance + candidatesDistances[i];
		    
		    if (candidateDistance <= maxMarkerDist)
		    {
			std::vector<int> candidate(candidates[i]);
			candidate.push_back(c);
			
			nextCandidates.push_back(candidate);
			nextCandidatesDistances.push_back(candidateDistance);
		    }
		}
	    }
	    candidates = nextCandidates;
	    candidatesDistances = nextCandidatesDistances;
	}
    }
    
    std::vector<int> ccodes;
    for (auto c : candidates)
    {
	int code = 0;
	int exponent = 1;
	
	for (int i = dimensions - 1; i >= 0; i--)
	{
	    code += c[i] * exponent;
	    exponent *= Marker::numHues;
	}
	ccodes.push_back(code);
    }

    
    // locate them in the dictionary and find the closest one
    int bestDistance = 999999999;
    int best = -1;
    for (auto code : ccodes)
    {
	const auto& i = std::lower_bound(codes.begin(), codes.end(), code);
	if (i != codes.end() && *i == code)
	{
	    auto m = markers.begin() + (i - codes.begin());
	    
	    int dsq = m->euclideanDistance (hues);

	    if (dsq < bestDistance)
	    {
		bestDistance = dsq;
		best = m->id;
	    }
	}
    }
//    cout << "Best = " << best << " code " << bestCode << " | "; // endl;

    // // now loop over all markers to find the definite best...
    // int tb = -1;
    // int tbc = -1;
    // int tbd = 999999999;
    // Marker bm;
    // for (const Marker& m : markers)
    // {	
    // 	int dsq = m.euclideanDistance(hues);

    // 	if (dsq < tbd)
    // 	{
    // 	    bm = m;
    // 	    tb = m.id;
    // 	    tbc = m.code;
    // 	    tbd = dsq;
    // 	}
    // }
    // cout << "Exhaustive id = " << tb << " code " << tbc << " pixels/hues ";
    // for (auto c : bm.pixels)
    // 	cout << c;
    // cout << " | ";
    // for (auto h : bm.hues)
    // 	cout << h << " ";

    // cout << " | ";
    // if (tb == best) cout << "--- OK ---";
    // else cout << "*** FAILED ***";
    // cout << endl;


    
    dist = bestDistance;
    return best;
}

}
