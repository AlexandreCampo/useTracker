
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <map>
#include "marker.h"

class Dictionary
{
public :
    
    int dimensions;
    
    std::vector<int> huesToColor;

    bool sorted = false;
    std::vector<Marker> markers;
    std::vector<int> codes;    

    Dictionary(int dimensions, std::vector<int>& referenceHues);
    ~Dictionary();

    void sort();
    void add(int code, int id);
    void clear();    
    int findBestMarker(const std::vector<int>& p, int maxHueDist, int maxMarkerDist, int& dist);
};


#endif
