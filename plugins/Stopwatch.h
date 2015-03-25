#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>

class Stopwatch : public PipelinePlugin
{
public:

    class Shortcut
    {
    public:
	unsigned char key;
	std::string name;
	int type = -1;

	Shortcut(){};

	Shortcut(unsigned char key, std::string name, int type)
	{
	    this->key = key;
	    this->name = name;
	    this->type = type;
	}
    };

    class Event
    {
    public:
	std::string name;
	double time;
	bool type; // false = start / true = end
	int id = -1;

	Event(){};

	Event(int id, std::string name, double time, bool type = false)
	{
	    this->id = id;
	    this->name = name;
	    this->time = time;
	    this->type = type;
	}

	bool operator < (const Event& e) const
	{
	    return (time < e.time);
	}
    };

    std::string outputFilename;
    std::fstream outputStream;

    std::vector<Shortcut> shortcuts;
    std::vector<bool> shortcutsStartEndFlag;
    std::vector<Event> events;

    Stopwatch();

    void AddShortcut(unsigned char key, std::string name, int type);
    void DeleteShortcut(int index);
    int RecordEvent (unsigned char k);
    void DeleteEvent (int id);
    Event& GetLastEvent();

    void Reset(){};
    void Apply(){};
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void LoadData (std::string filename);

    void CloseOutput();
    void OpenOutput();
};

#endif
