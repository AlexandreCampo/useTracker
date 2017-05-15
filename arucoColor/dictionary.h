#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <map>
#include "marker.h"

namespace ac
{
    
    struct Dictionary
    {
	std::vector<int> huesToColor;
	
	bool updated = false;
	std::vector<Marker> markers;
	std::vector<int> codes;    

	std::vector<std::pair<int,int>> originalCodes
	
	Dictionary();
	~Dictionary();
	
	void sort();
	void add(int code, int id);
	std::vector<std::pair<int,int>> getCodes();
	void clear();    
	int findBestMarker(const std::vector<int>& p, int maxHueDist, int maxMarkerDist, int& dist);
	void refreshHues();
    };

}
#endif
