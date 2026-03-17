/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#include "Stopwatch.h"
#include "ImageProcessingEngine.h"

#include <algorithm>

using namespace std;
using namespace cv;

Stopwatch::Stopwatch () : PipelinePlugin()
{
    CloseOutput();
}

void Stopwatch::LoadData (string filename)
{
    std::ifstream file(filename);
    std::string str;

    // eat up first line
    for (int i = 0; i < 3; i++)
	file >> str;

    events.clear();

    // read first line separately
    int id = 0;
    Event e;

    if (!file.eof())
    {
	e.id = id;
	file >> e.time;
	file >> e.name;    
	file >> str;
	if (str == "start") e.type = false;
	else e.type = true;

	do
	{
	    events.push_back(e);
	    id++;
	    
	    e.id = id;
	    file >> e.time;
	    file >> e.name;    
	    file >> str;
	    if (str == "start") e.type = false;
	    else e.type = true;
	    
	} while (!file.eof()); // if eof is reached while parsing data, do not record the event
    }
}

void Stopwatch::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	shortcuts.clear();
	shortcutsStartEndFlag.clear();

	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];

	FileNode fn2 = fn["Shortcuts"];
	if (!fn2.empty())
	{
	    FileNodeIterator it = fn2.begin(), it_end = fn2.end();
	    for (; it != it_end; ++it)
	    {
		FileNode fn3 = (*it); // ugly hack to go around duplicate key bug

		Shortcut s;
		s.key = (int) fn3["Key"];
		s.name = (string) fn3["Name"];
		string type = (string) fn3["Type"];
		if (type == "single")
		    s.type = false;
		else
		    s.type = true;

		shortcuts.push_back(s);
		shortcutsStartEndFlag.push_back(false);
	    }
	}
    }
}

void Stopwatch::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;

    // save shortcuts if any defined
    if (!shortcuts.empty())
    {
	fs << "Shortcuts" << "{";

	int i = 0;
	for (auto s : shortcuts)
	{
	    fs << string("Shortcut_") + std::to_string(i) << "{"; // ugly hack to go around duplicate key bug 

	    fs << "Key" << (int) s.key;
	    fs << "Name" << s.name;

	    if (s.type == false)
		fs << "Type" << "single";
	    else
		fs << "Type" << "start/end";

	    fs << "}";

	    i++;
	}

	fs << "}";
    }
}

void Stopwatch::AddShortcut(unsigned char key, std::string name, int type)
{
    shortcuts.push_back(Shortcut(key, name, type));
    shortcutsStartEndFlag.push_back(false); // ready to receive start events
}

void Stopwatch::DeleteShortcut(int index)
{
    auto it = shortcuts.begin();
    it += index;
    shortcuts.erase(it);
    
    auto it2 = shortcutsStartEndFlag.begin();
    it2 += index;
    shortcutsStartEndFlag.erase(it2);
}

int Stopwatch::RecordEvent (unsigned char k)
{
    int id = -1;

    // search user defined events    
    int s = -1;

    for (unsigned int i = 0; i < shortcuts.size(); i++)
	if (shortcuts[i].key == k)
	    s = i;

    if (s >= 0)
    {
	// if event is start / end, record accordingly, otherwise stay in start type
	int type = false;
	if (shortcuts[s].type)
	{
	    type = shortcutsStartEndFlag[s];
	    shortcutsStartEndFlag[s] = !shortcutsStartEndFlag[s];
	}

	id = events.size();
	events.push_back(Event(id, shortcuts[s].name, pipeline->parent->capture->GetTime(), type));
    }
    return id;
}

void Stopwatch::DeleteEvent (int id)
{
    for (auto e = events.begin(); e != events.end(); ++e)
    {	
	if (e->id == id)
	{
	    events.erase(e);
	    break;
	}
    }
}

Stopwatch::Event& Stopwatch::GetLastEvent()
{
    return events[events.size()-1];
}

void Stopwatch::CloseOutput()
{
    if (outputStream.is_open())
    {
	// make a local copy before sorting
	std::vector<Event> ev = events;

	// sort recored events
	sort (ev.begin(), ev.end());

	// write events in the file
	for (auto e : ev)
	{
	    if (e.type == false)
		outputStream << e.time << "\t" << e.name << "\t" << "start" << std::endl;
	    else
		outputStream << e.time << "\t" << e.name << "\t" << "end" << std::endl;
	}

	// close the file
	outputStream.close();
    }
}

void Stopwatch::OpenOutput()
{
    // open normal output
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), std::ios::out);
	if (outputStream.is_open())
	{
	    outputStream << "time \t event \t type" << std::endl;
	}
    }
}
