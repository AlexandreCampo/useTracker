
#include "marker.h"

#include <cmath>
#include <iostream>

using namespace std;

std::vector<int> Marker::distances;
std::vector<int> Marker::referenceHues;

// dimensions
int Marker::cols;
int Marker::rows;
int Marker::numPixels;
int Marker::numHues;


Marker::Marker()
{
    pixels.resize(numPixels);
    hues.resize(numPixels);
}

void Marker::rotate()
{
    vector<int> p (pixels);
    vector<int> h (hues);

    pixels = vector<int> (p.begin()+1, p.end());
    pixels.push_back(p[0]);
    hues = vector<int> (h.begin()+1, h.end());
    hues.push_back(h[0]);

    calculateCode();
}

Marker::Marker(int code, int id)
{
    pixels.resize(numPixels);
    hues.resize(numPixels);

    int t = code;
    for (int i = numPixels - 1; i >= 0; i--)
    {
	pixels[i] = t % numHues;
	t /= numHues;
	if (!referenceHues.empty())
	    hues[i] = referenceHues[ pixels[i] ];
    }

    this->code = code;
    this->id = id;
}

int Marker::getCode ()
{
    return code;
}


int Marker::getId ()
{
    return id;
}

void Marker::calculateCode ()
{
    code = 0;
    int exponent = 1;
    
    for (int i = numPixels - 1; i >= 0; i--)
    {
    	code += pixels[i] * exponent;
    	exponent *= numHues;
    }
}


// build marker from a list of detected hues
void Marker::quantize()
{
    // find closest marker colors
    for (int i = 0; i < pixels.size(); i++)
    {
	int mindist = 200;
	int refc = -1;	    
	for (int k = 0; k < pixels.size(); k++)
	{
	    int diff = abs( (hues[i] - referenceHues[k]));
	    diff = 90 - abs(diff - 90);
	    if (diff < mindist)
	    {
		refc = k;
		mindist = diff;
	    }
	}
	pixels[i] = refc;
    }
}


int Marker::selfDistance ()
{
    return distance(pixels, true);
}    

// check that neighbor pixels differ
int Marker::adjacencyDistance ()
{
    int diff = abs(pixels[0] - pixels[numPixels-1]);
    int mindist = distances[diff];
    
    for (int c = 1; c < numPixels; c++)
    {
	int diff = abs(pixels[c-1] - pixels[c]);
	int dist = distances[diff];
	
	if (dist < mindist)
	    mindist = dist;
    }
    return mindist;
}

int Marker::distance (const Marker& m, bool self)
{
    return distance(m.pixels, self);
}

int Marker::distance (const std::vector<int>& opixels, bool self)
{
    int mindist = numPixels * numHues * numHues;
    
    int start = self ? 1 : 0;

    // cout << "comparing " << numPixels << " " << numHues << " ";
    // for (auto c : opixels)
    // 	cout << c;
    // cout << " with ";
    
    // for each rotation possible
    for (int r = start; r < numPixels; r++)
    {
	// color wise compare
	int d = 0;
	for (int i = 0; i < numPixels; i++)
	{
	    int j = (r + i) % numPixels;
	    
	    int diff = abs(opixels[j] - pixels[i]);
	    d += distances[diff];

//	    cout << opixels[j];
	}

//	cout << " dist = " << d << "/" << mindist << " | ";
	if (d < mindist)
	    mindist = d;
    }
    //  cout << endl;
    return mindist;
}

int Marker::euclideanDistance (const std::vector<int>& mhues) const
{
    // color wise compare (hue)
    int d = 0;
    for (int i = 0; i < numPixels; i++)
    {
	int diff = abs(mhues[i] - hues[i]);
	diff = 90 - abs (diff - 90);
	
	d += diff * diff;
    }

    return d;
}

