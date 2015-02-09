
#include "CaptureDefault.h"

using namespace std;
using namespace cv;

CaptureDefault::CaptureDefault() : Capture()
{
    frame = Mat::zeros(600, 800, CV_8UC3);

    int baseline = 0;
    char str[] = "USE Tracker";
    Size textSize = getTextSize(str, FONT_HERSHEY_TRIPLEX, 2.0, 3, &baseline);
    Point pos (frame.cols / 2 - textSize.width / 2, frame.rows / 2 + textSize.height / 2);
    putText(frame, str, pos, FONT_HERSHEY_TRIPLEX, 2.0, cvScalar(64,64,64), 3, CV_AA);

    width = frame.cols;
    height = frame.rows;
    fps = 1;

    type = IMAGE;
}

bool CaptureDefault::GetNextFrame ()
{
    return false;
}

wxLongLong CaptureDefault::GetNextFrameSystemTime()
{
    return 0;
}

bool CaptureDefault::GetFrame (double time)
{
    return false;
}

long CaptureDefault::GetFrameNumber ()
{
    return 0;
}

long CaptureDefault::GetFrameCount ()
{
    return 1;
}

double CaptureDefault::GetTime()
{
    return 0;
}
