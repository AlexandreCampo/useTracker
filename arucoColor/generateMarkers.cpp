
// generate markers
// compile with  g++ -g generateMarkers.cpp marker.cpp -o generateMarkers -std=c++11


#include "marker.h"

#include <cstdlib>
#include <random>
#include <iostream>

using namespace std;

#define MAXFAILS 50000

int MAXMARKERS = 500;
int MINTAU = 3;
int MINADJACENCY = 0;
int MINDICTSIZE = 1;
int MINAVGDIST = 1;

// number of hues that can be recognized
int C = 9;

// total number of pixels of a marker
int N = 4;

int main(int argc, char** argv)
{
    Marker::numHues = C;
    Marker::numPixels = N;

    // for (int i = 0; i < )
    // Marker
        
    MINTAU = atoi(argv[1]);
    MINADJACENCY = atoi(argv[2]);
    MINDICTSIZE = atoi(argv[3]);
    MAXMARKERS = atoi(argv[4]);
    MINAVGDIST = atof(argv[5]);
    
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uniform(0, pow(C,N)); // guaranteed unbiased

    // fill the distances table    
    for (int i = 0; i < C; i++)
    {
	if (i <= C / 2)
	    Marker::distances.push_back(i);
	else
	    Marker::distances.push_back(C - i);	
    }

    int tau = 20;
    int n = 0;
    int fail = 0;
    
    vector<Marker> dictionary;
    vector<int> taus;
    while (n < MAXMARKERS)
    {
	// generate a new marker
	Marker candidate (uniform(rng));

	// same adjacent colors
	if (candidate.adjacencyDistance() < MINADJACENCY)
	{
	    fail++;
	}

	// self dist too small ?
	else if (candidate.selfDistance() < tau )
	{
	    fail++;
	}

        // self dist and adjacent colors ok
	else
	{
	    // calculate distance to dictionnary
	    int mindist = N * C;
	    for (auto m : dictionary)
	    {
		int d = m.distance(candidate);
		if (d < mindist)
		    mindist = d;
	    }
	
	    // add to dictionary ? 
	    if (mindist >= tau)
	    {
		dictionary.push_back(candidate);
		taus.push_back(mindist);
		n++;
		
		fail = 0;
	    }
	    else
	    {
		fail++;
	    }
	}

	// decrease tau
	if (fail >= MAXFAILS)
	{
	    fail = 0;
	    tau--;
	    cout << "Searching... switched to tau " << tau << endl;
	}

	// stop searching
	if (tau < MINTAU)
	    break;

    }

    if (dictionary.size() > MINDICTSIZE)
    {
	// calculate average interdistance
	float d = 0;
	float c = 0;
	for (auto m1 : dictionary)
	{
	    int mindist = N * C;
	    for (auto m2 : dictionary)
	    {
		int d = m1.distance(m2);
		if (d > 0 && d < mindist)
		    mindist = d;
	    }
	    d += mindist;
	    c += 1.0;
	}
	float avgDist = d / c;

	if (avgDist >= MINAVGDIST)
	{
	
	    cout << "Dictionary of size " << dictionary.size() << " avgDist = " << avgDist << endl;
	    for (int i = 0; i < dictionary.size(); i++)
	    {
		cout << dictionary[i].getCode() << "/" << taus[i] << " ";
	    }
	    cout << endl;
	    
	    for (int i = 0; i < dictionary.size(); i++)
	    {
		cout << dictionary[i].getCode() << " ";
	    }
	    cout << endl;
	    cout << endl;
	}
    }
}
