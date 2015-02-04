#ifndef SAFEEROSION_H
#define SAFEEROSION_H

#include "PipelinePlugin.h"


class SafeErosion : public PipelinePlugin
{
public:

    int size = 1;

    void Reset(){};
    void Apply(){};
    /* void Apply(); */
    /* void LoadXML (XMLElement* el); */
    /* void SaveXML (XMLElement* el); */
};

#endif
