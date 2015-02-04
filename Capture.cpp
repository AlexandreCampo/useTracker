
#include "Capture.h"

Capture::Capture()
{
//    this->blocking = blocking;

#ifdef WIN32
    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency( &nFrequency );
    frequency = (double)nFrequency.QuadPart;
#endif //WIN32

    initTime = GetSystemTime();
}

double Capture::GetTime()
{
    time = GetSystemTime() - initTime;
//    return GetSystemTime() - initTime;
    return time;
}

double Capture::GetSystemTime()
{
    double dTime = 0.0;

#ifdef WIN32
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter( &nCounter );
    dTime = ( (double)nCounter.QuadPart ) / m_dFrequency;
#else
    struct timespec now;
    clock_gettime( CLOCK_REALTIME, &now );
    dTime = ( (double)now.tv_sec ) + ( (double)now.tv_nsec ) / 1000000000.0;
#endif //WIN32

    return dTime;
}


void Capture::SetStartTime (float time)
{
    startTime = time;
}

void Capture::SetDuration(float time)
{
    duration = time;
}

void Capture::SetTimestep(float t)
{
    timestep = t;
}

