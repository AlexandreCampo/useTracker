/***************************************************************
 * Name:      App.h
 * Purpose:   Defines Application Class
 * Author:     ()
 * Created:   2014-08-16
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef APP_H
#define APP_H

#include <wx/app.h>

#include "PipelinePlugin.h"
#include <opencv2/opencv.hpp>

//vector<PipelinePlugin*> CreatePipelinePlugin (string str, cv::FileNode& fn, unsigned int threadsCount);

template<typename T>
std::vector<PipelinePlugin*> CreatePipelinePluginVector(cv::FileNode& fn, unsigned int threadsCount)
{
    std::vector<PipelinePlugin*> pfv;
    T* firstInstance = new T;

    pfv.push_back(firstInstance);

    PipelinePlugin* p = static_cast<PipelinePlugin*>(firstInstance);

    if (p->multithreaded)
    {
	for (unsigned int i = 1; i < threadsCount; i++)
	    pfv.push_back(new T);
    }

    for (auto f : pfv)
	f->LoadXML(fn);

    return pfv;
}

extern std::map<std::string, std::vector<PipelinePlugin*> (*)(cv::FileNode&, unsigned int)> NewPipelinePluginVector;

class MainFrame;

class App : public wxApp
{
public:
    MainFrame* mainFrame = nullptr;
        
    virtual bool OnInit();
};

#endif // APP_H
