
// #include "SafeErosion.h"

// void SafeErosion::Apply()
// {
//     for (int j = 0; j < size; j++)
//     {
// 	// we assume the buffer is coherent with the state of the pixels...
// 	// now scan pixels and check if they are surrounded, in that case they stay in the newlist, otherwise... bye bye
// 	vector<Match> m;
// 	vector<bool> deleted;
// 	for (int i = 0; i < matches.size(); i++)
// 	{
// 	    // scan around
// 	    int score = 0;
// 	    int index = matches[i].index;

// 	    // boundary ...
// 	    if (matches[i].x == 0 || matches[i].x == width - 1 || matches[i].y == 0 || matches[i].y == height)
// 	    {
// 		deleted.push_back(true);
// 	    }
// 	    else
// 	    {
// 		score += marked.data[index - 1];
// 		score += marked.data[index + 1];
// 		score += marked.data[index - width];
// 		score += marked.data[index + width];

// 		if (score == 4 || score == 3)
// 		{
// 		    deleted.push_back(false);
// 		    matches[i].weight++;
// 		    m.push_back(matches[i]);
// 		}
// 		else if (score == 0)
// 		{
// 		    deleted.push_back(false);
// 		    m.push_back(matches[i]);
// 		}
// 		else
// 		{
// 		    deleted.push_back(true);
// 		}
// 	    }
// 	}

// 	// ok now just update the marked.data by deleting matches
// 	for (int i = 0; i < matches.size(); i++)
// 	{
// 	    if (deleted[i]) marked.data[matches[i].index] = 0;
// 	}

// 	// overwrite list of pixels
// 	matches = m;
//     }

//     // put weights on marked img
//     for (int i = 0; i < matches.size(); i++)
//     {
// 	marked.data[matches[i].index] = (unsigned char) matches[i].weight;
//     }
// }

// void SafeErosion::LoadXML (XMLElement* el)
// {
//     if (el)
//     {
//     	el->QueryIntAttribute("active", &active);
//     	el->QueryIntAttribute("size", &size);
//     }
// }

// void SafeErosion::SaveXML (XMLElement* el)
// {
//     if (el)
//     {
// 	el->SetAttribute("active", active);
// 	el->SetAttribute("size", size);
//     }
// }
