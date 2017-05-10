#ifndef MARKER_H
#define MARKER_H

#include <vector>

struct Marker
{    
    static std::vector<int> distances;
    static std::vector<int> referenceHues; 

    // dimensions
    static int cols;
    static int rows;
    static int numPixels;
    static int numHues;
    
    // marker stored as color indices
    std::vector<int> pixels;

    // marker stored as hue values
    std::vector<int> hues;

    int code;
    int id;
    
    Marker();
    Marker(std::vector<int>& detectedHues);
    Marker(int code, int id = 0);
    int getId ();
    int getCode ();
    void calculateCode ();

    void rotate();
    void quantize();
    int selfDistance ();
    int adjacencyDistance ();
    int distance (const Marker& m, bool self = false);
    int distance (const std::vector<int>& pixels, bool self = false);
    int euclideanDistance (const std::vector<int>& hues) const;

};

#endif
