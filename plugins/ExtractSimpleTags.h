#ifndef EXTRACT_SIMPLE_TAGS_H
#define EXTRACT_SIMPLE_TAGS_H

#include "PipelinePlugin.h"


class ExtractSimpleTags : public PipelinePlugin
{
public:

//    int minSize = 10;

    void Reset(){};
    void Apply();

//    void GetBlob (Mat& marked, Mat& img, int x, int y, int idx, int w, int h, Blob& blob);
};


#endif
