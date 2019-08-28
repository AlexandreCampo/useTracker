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

#include "Tracker.h"

#include "ImageProcessingEngine.h"
#include "Blob.h"

#include <math.h>       /* exp */

#include <fstream>

#include "Munkres.h"

// TODO REMOVE DEBUG
#include <iostream>
#include <cstdlib>
#include <ctime>


//#include "../dlib/optimization/max_cost_assignment.h"
//using namespace dlib;

using namespace std;
using namespace cv;

//~ Matrix<double> matrix2(5, 5);


Tracker::~Tracker()
{
    CloseOutput();
}

void Tracker::Reset()
{
    SetMaxEntities(entitiesCount);
//    if (output) OpenOutput();
}

void Tracker::SetMaxEntities (unsigned int n)
{
    entities.clear();

    if (n == 0) n = 1;
    entitiesCount = n;

    // create a list of entities
    for (unsigned int i = 0; i < entitiesCount; i++)
    {
        entities.push_back(Entity(motionEstimatorLength));
    }
}

inline void Tracker::UpdateMotionEstimator (Entity& entity, const Entity& previousEntity)
{
    // static image
//    if (pipeline->parent->staticFrame) return;

    //~ if (previousEntity.assigned)
    //~ {
    
	//~ cout<<"new_release"<<endl;
    int frameNumber = pipeline->parent->capture->GetFrameNumber();

    
    float number_of_frames = frameNumber - lastframeNumber;
	unsigned int didx = entity.diffIdx;
	
	if (entity.toforget == true)
	{	
		if (didx == 0)
		{
			entity.dx[didx] = entity.dx[motionEstimatorLength-1];
			entity.dy[didx] = entity.dx[motionEstimatorLength-1];		
		}
		else
		{
			entity.dx[didx] = entity.dx[didx-1];
			entity.dy[didx] = entity.dx[didx-1];					
		}
	}
	else
	{
		entity.dx[didx] = (entity.x - previousEntity.x)/number_of_frames;
		entity.dy[didx] = (entity.y - previousEntity.y)/number_of_frames;
	}
	
	didx++;
	if (didx >= motionEstimatorLength) didx = 0;
	entity.diffIdx = didx;
    //~ }
}

void Tracker::Apply()
{
    if (pipeline->parent->staticFrame) return;

    if (replay)
    {
        Replay();
    }
    else
    {
        Track();
    }
}

void Tracker::Replay()
{
    // locate data
    unsigned int currentFrame = pipeline->parent->capture->GetFrameNumber();

    // TODO DEBUG
//    cout << "Tracker : current frame=" << currentFrame << " h-start=" << historyStartFrame << " h-size=" << history.size() << endl;

    bool found = false;
    int eidx = -1; 
    
    if (currentFrame >= historyStartFrame)
    {
        if (historyEntriesIndex < historyEntries.size())
        {
            // directly find correct frame, or search forward
            for (unsigned int i = historyEntriesIndex; i < historyEntries.size(); i++)
            {
                if (historyEntries[i].frameNumber == currentFrame)
                {
                    // found the entries, update current entities
                    eidx = historyEntries[i].entitiesIndex;
                    historyEntriesIndex = i;
                    found = true;	
                    break;
                }
            }

            // look for frame in previous entries
            if (!found)
            {
                for (unsigned int i = 0; i < historyEntriesIndex; i++)
                {
                    if (historyEntries[i].frameNumber == currentFrame)
                    {
                        historyEntriesIndex = i;
                        eidx = historyEntries[i].entitiesIndex;
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    if (found)
    {
        for (unsigned int e = 0; e < entitiesCount; e++)
        {
            entities[e] = history[eidx+e];
        }
        historyEntriesIndex++;
    }
    else
        historyEntriesIndex = historyEntries.size();
}

void Tracker::Track()
{
    vector<Blob>& blobs = pipeline->parent->blobs;
    int frameNumber = pipeline->parent->capture->GetFrameNumber();

    // remember previous positions
    previousEntities = entities;
    for (unsigned int i = 0; i < entities.size(); i++) entities[i].assigned = false;
    
    

	//Count the number of blobs available	
	int blobs_available_size = 0;
	
    for (unsigned int b = 0; b < blobs.size(); b++)
    {
        if (blobs[b].available)
        {
			blobs_available_size += 1;
            }
        }
	
	//~ cout<<"blobs_available_size: "<<blobs_available_size<<endl;
	//If no blob is available, reuse the previous entities
    
    if (blobs_available_size == 0)
    {
		for (unsigned int e = 0; e < entities.size(); e++)  
		{	entities[e].x = previousEntities[e].x;
			entities[e].y = previousEntities[e].y;
			entities[e].assigned = true;
			entities[e].size = previousEntities[e].size;
			entities[e].zone = previousEntities[e].zone;    
		}
	}
    
    
    else
    {
		// Estimate motion of entities based on previous observations
		for (unsigned int e = 0; e < entities.size(); e++)
		{
			// if entity is not in visible zone, don't move, just wait for reassignment
			if (entities[e].zone != ZONE_VISIBLE && !previousEntities[e].assigned) continue;

			// if entity not detected for a long time
			if (frameNumber - entities[e].lastFrameDetected >= motionEstimatorTimeout * pipeline->parent->capture->fps) continue;

			
			// set decay factor vector
			std::vector<float> decayfactor(motionEstimatorLength);
			float sum_of_elems = 0;
			
			for (float it = 0; it < decayfactor.size(); it++)
			{
				decayfactor[it]=exp(-it*extrapolationDecay);
				sum_of_elems += decayfactor[it];
			}
			
			for (float it = 0; it < decayfactor.size(); it++)
			{
				decayfactor[it]/=sum_of_elems;
			}		
			

			// extrapolate motion of  entities		
			
			//multiply the steps by the decay factor and add the sum to the previous coordinates of the entity
			unsigned int didx = entities[e].diffIdx;	
			int pastindex = didx-1;		
			//~ if (e==5)
			//~ {
			//~ cout<<" "<<endl;
			//~ cout<<"entity: "<<e<<endl;
			//~ cout<<"didx: "<<pastindex<<endl;
			//~ cout<<" "<<endl;
			//~ }			
			float dx = 0.0, dy = 0.0;
			for ( int i = 0; i < motionEstimatorLength; i++)
			{	
				unsigned int idx_decay;

				if (i-pastindex <= 0)
				{
					idx_decay = abs(i-pastindex);
				}
				else
				{
					idx_decay = pastindex - i + motionEstimatorLength;
				} 			

				dx += entities[e].dx[i] * decayfactor[idx_decay];
				dy += entities[e].dy[i] * decayfactor[idx_decay];
				//~ if (e==5)
				//~ {
				//~ cout<<"dist_e: "<<i<<"  ,dx:"<<entities[e].dx[i]<<endl;                    
				//~ cout<<"dist_e: "<<i<<"  ,dy:"<<entities[e].dy[i]<<endl;
				//~ cout<<"dist_e_decay: "<<i<<"  ,dx:"<<entities[e].dx[i]* decayfactor[idx_decay]<<endl;                    
				//~ cout<<"dist_e_decay: "<<i<<"  ,dy:"<<entities[e].dy[i]* decayfactor[idx_decay]<<endl;
				//~ cout<<"decay_factor: "<<i<<"  ,decay_factor:"<<decayfactor[idx_decay]<<endl;                    
				//~ }        
			}
			
		   float number_of_frames = frameNumber - lastframeNumber;
			
			entities[e].x += dx*number_of_frames;
			entities[e].y += dy*number_of_frames;

			//~ if (e==5)
			//~ {
			//~ cout<<"dx_after: "<<dx<<endl;
			//~ cout<<"dy_after: "<<dy<<endl;
			//~ cout<<"number_of_frames: "<<number_of_frames<<endl;
			//~ cout<<"x_e_after: "<<entities[e].x<<endl;
			//~ cout<<"y_e_after: "<<entities[e].y<<endl;
			//~ }


			// prevent entity from going outside the picture
			if (entities[e].x < 0) entities[e].x = 0;
			else if (entities[e].x >= pipeline->width) entities[e].x = pipeline->width - 1;
			if (entities[e].y < 0) entities[e].y = 0;
			else if (entities[e].y >= pipeline->height) entities[e].y = pipeline->height - 1;

			// check if entity is in visible zone
			unsigned int idx = entities[e].x + entities[e].y * pipeline->width;
			entities[e].zone = pipeline->zoneMap.data[idx];

		}

		// now for blobs/entities assignment, I will compute distances and rank couples to have best correspondances
	   
	   
	   
		//Count the number of blobs available	
		int blobs_available_size = 0;
		
		for (unsigned int b = 0; b < blobs.size(); b++)
		{
			if (blobs[b].available)
			{
				blobs_available_size += 1;
				}
			}

		//Count the number of entities that have been assigned the frame before
		unsigned int preventities_assigned_size = 0;
		for (unsigned int e = 0; e < entities.size(); e++)
		{
			if (previousEntities[e].assigned)
			{
				preventities_assigned_size += 1;
				}
			}

		
		//Set the row size of the matrix (entities assigned) but if no entity is assigned then all the entities are included in the matrix
		int matrix_size = preventities_assigned_size;  
		if (preventities_assigned_size==0)
		{	
			matrix_size = entities.size();
		}


		
		// Create the cost matrix (matrix of interdistances between entities that have been assigned and blobs) 
		// and the vector of interdistances between all entities and blobs
		vector<Interdistance> interdistances;	
		int ib = 0;
		vector<int> b_ib_correspondance(blobs_available_size);  
		vector<int> e_ie_correspondance(matrix_size);   
		Matrix<double> matrix_interdistances(matrix_size, blobs_available_size); 	
		 
		if (preventities_assigned_size!=0) 
		{		
			for (unsigned int b = 0; b < blobs.size(); b++)
			{
				if (blobs[b].available)
				{
					int ie = 0;
					for (unsigned int e = 0; e < entities.size(); e++)
					{	
						double distsq;
						double diffX = entities[e].x - blobs[b].x;
						double diffY = entities[e].y - blobs[b].y;
						distsq = diffX * diffX + diffY * diffY;
						interdistances.push_back (Interdistance(b, e, distsq));
						
						if (previousEntities[e].assigned)
						{           
							matrix_interdistances(ie,ib) = distsq;               
							b_ib_correspondance[ib]=b;
							e_ie_correspondance[ie]=e;
						ie += 1;
						}
					}
				ib += 1;
				}
			}
		}
		
		else
		{
			for (unsigned int b = 0; b < blobs.size(); b++)
			{
				if (blobs[b].available)
				{	
					int ie = 0;
					for (unsigned int e = 0; e < entities.size(); e++)
					{	          

						double distsq;
						double diffX = entities[e].x - blobs[b].x;
						double diffY = entities[e].y - blobs[b].y;
						distsq = diffX * diffX + diffY * diffY;

						interdistances.push_back (Interdistance(b, e, distsq));   
						matrix_interdistances(ie,ib) = distsq;               
						b_ib_correspondance[ib]=b;
						e_ie_correspondance[ie]=e;
						
					ie += 1;

					}
				ib += 1;
				}
			}		
		}	
			
		std::sort(interdistances.begin(), interdistances.end());

		
		/// USING HUNGARIAN
		
		//set the numbers of rows (assigned entities) and th
		int nrows = matrix_size;//preventities_assigned_size;
		int ncols = blobs_available_size;
		
		// Display solved matrix.
		//~ for ( int row = 0 ; row < nrows ; row++ ) {
			//~ for ( int col = 0 ; col < ncols ; col++ ) {
				//~ std::cout.width(2);
				//~ std::cout << matrix_interdistances(row,col) << ",";
			//~ }
			//~ std::cout << std::endl;
		//~ }
		
		//Record cost matrix
		Matrix<double> matrix_cost(matrix_size, blobs_available_size);	
		matrix_cost = matrix_interdistances;

		// Apply Munkres algorithm to matrix.
		Munkres<double> m;
		m.solve(matrix_interdistances);

		// Display solved matrix.
		//~ for ( int row = 0 ; row < nrows ; row++ ) {
			//~ for ( int col = 0 ; col < ncols ; col++ ) {
				//~ std::cout.width(2);
				//~ std::cout << matrix_interdistances(row,col) << ",";
			//~ }
			//~ std::cout << std::endl;
		//~ }



		/// END OF USING HUNGARIAN	
		
		
		
		// Assign entities 
		

		bool dohungarian;
		dohungarian = true;

		for ( int row = 0 ; row < nrows ; row++ ) 
		{
			for ( int col = 0 ; col < ncols ; col++ ) 
			{
				
				unsigned int ib = col;
				unsigned int ie = row;			
				
				if ( matrix_interdistances(ie,ib) == 0 && matrix_cost(ie,ib)>maxdistsq)
				{
					dohungarian = false;
				}
			}
		}

		//~ std::cout<< "dohungarian: " << dohungarian << std::endl;
		
		//If entities have been assigned then assign the best matched blob (by the Munkres algorithm if enough number of blobs)


		if (dohungarian == true)
		{


			for ( int row = 0 ; row < nrows ; row++ ) {
				for ( int col = 0 ; col < ncols ; col++ ) {
					
					unsigned int ib = col;
					unsigned int ie = row;			
					
					if ( matrix_interdistances(ie,ib) == 0 && matrix_cost(ie,ib)<maxdistsq)
					{
						//~ std::cout << e << "-->" << ib << endl;
						int b = b_ib_correspondance[ib];				
						int e = e_ie_correspondance[ie];
						//~ std::cout << e << endl;
						//~ std::cout << entities[b].x << "," << entities[b].y<< endl;
						//~ std::cout << ie << "  entities: " << e << endl;				
						entities[e].x = blobs[b].x;
						entities[e].y = blobs[b].y;
						entities[e].size = blobs[b].size;
						entities[e].assigned = true;
						entities[e].zone = blobs[b].zone;
						blobs[b].available = false;
						blobs[b].assignment = e;
						entities[e].lastFrameDetected = frameNumber;
						entities[e].toforget = false;
						//~ UpdateMotionEstimator (entities[e], previousEntities[e]);				
						//~ std::cout << entities[b].x << "," << entities[ib].y<< endl;
					}
				}
				//~ std::cout << std::endl;
			}	
		}
		
		
		//For the first frames
		if (historyEntriesIndex < settingfirstframes)
		{	
			//if blobs are available assign them to the closest entity (starting with the smallest distance...)
			for (unsigned int d = 0; d < interdistances.size(); d++)
			{	

			unsigned int b = interdistances[d].blobIdx;
			unsigned int e = interdistances[d].entityIdx;


			if (!entities[e].assigned && blobs[b].available)
			{	
				//~ std::cout<< 'a' << "e:" << e << std::endl;
				entities[e].x = blobs[b].x;
				entities[e].y = blobs[b].y;
				entities[e].assigned = true;
				entities[e].size = blobs[b].size;
				entities[e].zone = blobs[b].zone;
				blobs[b].available = false;
				blobs[b].assignment = e;
				entities[e].toforget = false;			
				//~ UpdateMotionEstimator (entities[e], previousEntities[e]);
			}
			
			}
			
			
			//if entities are not assigned, assign them to the closest entity
			//if entities are STILL not assigned (at least two consecutive frame), assign them to the entity it has been assigned the frame before
			for (int d = interdistances.size()-1; d > -1; d-=1) 
			{	

			unsigned int b = interdistances[d].blobIdx;
			unsigned int e = interdistances[d].entityIdx;
			unsigned int e_attached;
			unsigned int prev_e_attached;
					
			//~ std::cout<< "e:" << e << std::endl;
			if (!entities[e].assigned && entities[e].lastFrameDetected > 1)
			{	
				//~ std::cout<< 'b' << "e:" << e << std::endl;
				if (previousEntities[e].assigned)
				{
					//~ std::cout<< 'b' << "e:" << e << std::endl;
					entities[e].x = blobs[b].x;
					entities[e].y = blobs[b].y;
					entities[e].assigned = false;
					entities[e].size = blobs[b].size;
					entities[e].zone = blobs[b].zone;
					e_attached = blobs[b].assignment;
					prev_e_attached=entities[e].linkedEntity;
					entities[e].linkedEntity=e_attached;
					entities[e].toforget = true;
					entities[e_attached].toforget = true;
					entities[prev_e_attached].toforget = false;
					//~ std::cout<< "e:" << e << std::endl;
					//~ std::cout<< "e_attached:" << e_attached << std::endl;	
					//~ UpdateMotionEstimator (entities[e], previousEntities[e]);
				 }
				 else
				 {
					unsigned int e_to_attach = entities[e].linkedEntity;
					
					//~ std::cout<< "attached to" << "e:" << entities[e].linkedEntity << std::endl;
					entities[e].x = entities[e_to_attach].x;
					entities[e].y = entities[e_to_attach].y;
					entities[e].assigned = false;
					entities[e].size = entities[e_to_attach].size;
					entities[e].zone = entities[e_to_attach].zone;
					entities[e].toforget = false;
					//~ UpdateMotionEstimator (entities[e], previousEntities[e]);			
				 }
						
					
				//~ std::cout<< "attached to" << "e:" << entities[e].linkedEntity << std::endl;		
			}	
			}
		
		}
		
		
		//For the following frames
		else
		{
			//if blobs are available assign them to the closest entity (starting with the smallest distance...)(if the assignment is not too far)
			for (unsigned int d = 0; d < interdistances.size(); d++)
			{	
			unsigned int b = interdistances[d].blobIdx;
			unsigned int e = interdistances[d].entityIdx;

			if (!entities[e].assigned && blobs[b].available && interdistances[d].distsq<maxdistsq)
			{	
				//~ std::cout<< 'a' << "e:" << e << std::endl;
				entities[e].x = blobs[b].x;
				entities[e].y = blobs[b].y;
				entities[e].assigned = true;
				entities[e].size = blobs[b].size;
				entities[e].zone = blobs[b].zone;
				blobs[b].available = false;
				blobs[b].assignment = e;
				entities[e].toforget = false;
				//~ UpdateMotionEstimator (entities[e], previousEntities[e]);
			}	
			}

			

			//if entities are not assigned, assign them to the closest entity (if the assignment is not too far)
			//if entities are STILL not assigned (at least two consecutive frame), assign them to the entity it has been assigned the frame before (if the assignment is not too far)
			for (int d = interdistances.size()-1; d > -1; d-=1)  
			{	
			unsigned int b = interdistances[d].blobIdx;
			unsigned int e = interdistances[d].entityIdx;
			unsigned int e_attached;
			unsigned int prev_e_attached;
			
			//~ std::cout<< "frame:" << historyEntriesIndex << "  ,  " << maxdistsq << std::endl;
			if (!entities[e].assigned)// && interdistances[d].distsq<maxdistsq)
			{	
				if (previousEntities[e].assigned && !blobs[b].available)
				{
					//~ std::cout<< 'bc' << "e:" << e << std::endl;
					entities[e].x = blobs[b].x;
					entities[e].y = blobs[b].y;
					entities[e].assigned = false;
					entities[e].size = blobs[b].size;
					entities[e].zone = blobs[b].zone;
					e_attached = blobs[b].assignment;
					prev_e_attached=entities[e].linkedEntity;
					entities[e].linkedEntity=e_attached;
					entities[e].toforget = true;
					entities[e_attached].toforget = true;
					entities[prev_e_attached].toforget = false;
					//~ std::cout<< "e:" << e << std::endl;
					//~ std::cout<< "e_attachedc:" << e_attached << std::endl;	
					//~ UpdateMotionEstimator (entities[e], previousEntities[e]);
				 }
				 else
				 {
					unsigned int e_to_attach = entities[e].linkedEntity;
					
					//~ std::cout<< "attached to" << "e:" << entities[e].linkedEntity << std::endl;
					entities[e].x = entities[e_to_attach].x;
					entities[e].y = entities[e_to_attach].y;
					entities[e].assigned = false;
					entities[e].size = entities[e_to_attach].size;
					entities[e].zone = entities[e_to_attach].zone;
					entities[e].toforget = false;
					//~ UpdateMotionEstimator (entities[e], previousEntities[e]);			
				 }			
				//~ std::cout<< "attached to" << "e:" << entities[e].linkedEntity << std::endl;		
			}	
			}
		}


		for (unsigned int e = 0; e < entities.size(); e++)
		{	
			//~ std::cout<< "e:" << e << std::endl;
			UpdateMotionEstimator (entities[e], previousEntities[e]);
		}
	}	
    
    //~ // do the assignment work
    //~ for (unsigned int d = 0; d < interdistances.size(); d++)
    //~ {
        //~ // get an interdistance, if blob and entities are free, establish the correspondance
        //~ unsigned int b = interdistances[d].blobIdx;
        //~ unsigned int e = interdistances[d].entityIdx;

        //~ if (entities[e].assigned) continue;
        //~ if (!blobs[b].available) continue;
        //~ if (entities[e].toforget) continue;

        //~ // now check if the blob is not too far
        //~ // this test is not applied on first assignment
        //~ if (entities[e].lastFrameDetected >= 0)
        //~ {
            //~ float maxMotion = maxMotionPerSecond * (frameNumber - entities[e].lastFrameDetected) / pipeline->parent->capture->fps;
            //~ if (interdistances[d].distsq > maxMotion * maxMotion) continue;
        //~ }

        //~ bool createVirtualEntity = false;

        //~ // if the entity is a virtual one, assigned long enough, promote it to a real one
        //~ if (e >= entitiesCount && (frameNumber - entities[e].lastFrameNotDetected) > virtualEntitiesLifetime * pipeline->parent->capture->fps)
        //~ {
            //~ // find the corresponding real entity
            //~ entities[e].toforget = true;
            //~ int linkedEntity = entities[e].linkedEntity;
            //~ entities[linkedEntity] = entities[e];

            //~ // not assigned, the virtual e will be erased
            //~ e = linkedEntity;
        //~ }

        //~ // if the entity was lost/frozen for a long time, wait before assigning and use a virtual entity instead (long code !!!)
        //~ else if (!previousEntities[e].assigned)
        //~ {
            //~ if (virtualEntitiesZone && entities[e].zone != ZONE_VISIBLE) createVirtualEntity = true;
            //~ else if (virtualEntitiesDistsq >= 1 && interdistances[d].distsq > virtualEntitiesDistsq) createVirtualEntity = true;
            //~ else if (frameNumber - entities[e].lastFrameDetected > virtualEntitiesDelay * pipeline->parent->capture->fps) createVirtualEntity = true;
        //~ }

        //~ if (createVirtualEntity && useVirtualEntities)
        //~ {
            //~ entities[e].toforget = true;
            //~ entities.push_back(Entity(motionEstimatorLength));
            //~ entities.back().linkedEntity = e;
            //~ entities.back().lastFrameNotDetected = frameNumber - pipeline->parent->timestep * pipeline->parent->capture->fps;
            //~ e = entities.size() - 1;
        //~ }

        //~ // ok all tests passed, make it a detected entity
        //~ entities[e].x = blobs[b].x;
        //~ entities[e].y = blobs[b].y;
        //~ entities[e].size = blobs[b].size;
        //~ entities[e].assigned = true;
        //~ entities[e].zone = blobs[b].zone;
//~ //	    entities[e].zone = ZONE_VISIBLE;
        //~ blobs[b].available = false;
        //~ blobs[b].assignment = e;
        //~ entities[e].lastFrameDetected = frameNumber;

        //~ if (e < previousEntities.size())
            //~ UpdateMotionEstimator (entities[e], previousEntities[e]);
    //~ }

    // erase undetected virtual entities
    //~ for (unsigned int e = entities.size() - 1; e >= entitiesCount; e--)
    //~ {
        //~ if (!entities[e].assigned)
        //~ {
            //~ entities[ entities[e].linkedEntity ].toforget = false;
            //~ for (unsigned int i = e; i < entities.size() - 1; i++) entities[i] = entities[i+1];
            //~ entities.resize(entities.size() - 1);
        //~ }
    //~ }

    // and reset counters if not detected
    for (unsigned int e = 0; e < entities.size(); e++)
    {
        if (!entities[e].assigned) entities[e].lastFrameNotDetected = frameNumber;
    }

    // if (textStream.is_open()) outputText (textStream, entities, frameNumber, video->GetFrameTime());


    // save to history
//    cout << "Tracker : " << "saved data from frame " << pipeline->parent->capture->GetFrameNumber() << endl;

    if (history.empty()) historyStartFrame = frameNumber;

//    cout << "Tracked frame " << pipeline->parent->capture->GetFrameNumber() << " hindex" << history.size() / entitiesCount << " hstart = " << historyStartFrame << endl;

    historyEntriesIndex = historyEntries.size();
    historyEntries.push_back(HistoryEntry (frameNumber, history.size()));
    for (unsigned int e = 0; e < entitiesCount; e++)
    {
    	history.push_back(entities[e]);
    }
    
	lastframeNumber = frameNumber;
}

void Tracker::OutputHud (Mat& hud)
{
    char str[8];
    Point pos;

    // draw a trail if history available
    // first, draw past positions
    vector<Point> last;

    for (unsigned int i = 0; i < entitiesCount; i++)
    {
        last.push_back(Point(-1,-1));
    }

    // draw past trail if possible (data available)
    if (historyEntriesIndex < historyEntries.size())
    {
        for (unsigned int h = historyEntriesIndex - trailLength; h <= historyEntriesIndex; h++)
        {
            if (h >= 0 && h < historyEntries.size())
            {
                int ei = historyEntries[h].entitiesIndex;
                for (unsigned int e = 0; e < entitiesCount; e++)
                {
                    pos.x = history[ei+e].x;
                    pos.y = history[ei+e].y;

                    if (last[e].x >= 0)
                    {
                        line(hud, pos, last[e], cvScalar(32,32,32,255), 2, CV_AA);
                    }
                    last[e] = pos;
                }
//		cout << "past trail at h=" << h <<endl;
            }
        }
    }

    // then, try to draw future positions if in replay mode
    if (replay)
    {
        for (unsigned int h = historyEntriesIndex; h < historyEntriesIndex + trailLength; h++)
        {
            if (h >= 0 && h < historyEntries.size())
            {
                int ei = historyEntries[h].entitiesIndex;
                for (unsigned int e = 0; e < entitiesCount; e++)
                {
                    pos.x = history[ei+e].x;
                    pos.y = history[ei+e].y;

                    if (last[e].x >= 0)
                    {
                        line(hud, pos, last[e], cvScalar(255,255,255,255), 2, CV_AA);
                    }
                    last[e] = pos;
                }
//		cout << "future trail at h=" << h <<endl;
            }
        }
    }

    // plot id number, if possible
//    if (!replay || historyEntriesIndex < historyEntries.size())
//    {
//	unsigned int ei = historyEntries[historyEntriesIndex].entitiesIndex;
	for (unsigned int e = 0; e < entitiesCount; e++)
	{
	    sprintf (str, "%d", e);
	    pos.x = entities[e].x;
	    pos.y = entities[e].y;
	    putText(hud, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str, pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);
	}
//    }
}

void Tracker::OutputStep ()
{
    if (outputStream.is_open() && !replay)
    {
        for (unsigned int e = 0; e < entitiesCount; e++)
        {
            outputStream
                << pipeline->parent->capture->GetTime() << "\t"
                << pipeline->parent->capture->GetFrameNumber() << "\t"
                << entities[e].lastFrameDetected << "\t"
                << entities[e].lastFrameNotDetected << "\t"
                << e << "\t"
                << entities[e].assigned << "\t"
                << entities[e].zone << "\t"
                << entities[e].size << "\t"
                << entities[e].x << "\t"
                << entities[e].y
                << std::endl;
        }
    }
}


void Tracker::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
        outputStream.open(outputFilename.c_str(), std::ios::out);
        if (outputStream.is_open())
        {
            outputStream << "time \t frame \t lastFrameDetected \t lastFrameNotDetected \t entity \t assigned \t zone \t size \t x \t y " << std::endl;
        }
    }
}

void Tracker::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void Tracker::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
        active = (int)fn["Active"];
        output = (int)fn["Output"];
        outputFilename = (string)fn["OutputFilename"];

        minInterdistance = (float)fn["MinInterDistance"];
        maxMotionPerSecond = (float)fn["MaxMotionPerSecond"];
        extrapolationDecay = (float)fn["ExtrapolationDecay"];
        motionEstimatorLength = (int)fn["ExtrapolationHistorySize"];
        motionEstimatorTimeout = (float)fn["ExtrapolationTimeout"];

        entitiesCount = (int)fn["EntitiesCount"];

        trailLength = (int)fn["HistoryTrailLength"];

        FileNode fn2 = fn["VirtualEntities"];

        if (!fn2.empty())
        {
            useVirtualEntities = (int)fn2["Active"];
            virtualEntitiesDelay = (float)fn2["DelayToCreation"];
            virtualEntitiesZone = (int)fn2["Zone"];
            float v = (float)fn2["Distance"];
            virtualEntitiesDistsq = v * v;
            virtualEntitiesLifetime = (float)fn2["PromotionTime"];
        }
    }
}

void Tracker::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;

    fs << "EntitiesCount" << (int)entitiesCount;
    fs << "MinInterdistance" << minInterdistance;
    fs << "MaxMotionPerSecond" << maxMotionPerSecond;
    fs << "ExtrapolationDecay" << extrapolationDecay;
    fs << "ExtrapolationHistorySize" << (int)motionEstimatorLength;
    fs << "ExtrapolationTimeout" << motionEstimatorTimeout;

    fs << "HistoryTrailLength" << (int)trailLength;

    // we don't save that if we don't use it...
    if (useVirtualEntities)
    {
        fs << "VirtualEntities" << "{";

        fs << "Active" << useVirtualEntities;
        fs << "DelayToCreation" << virtualEntitiesDelay;
        fs << "Zone" << virtualEntitiesZone;
        fs << "PromotionTime" << virtualEntitiesLifetime;
        fs << "Distance" << sqrt(virtualEntitiesDistsq);

        fs << "}";
    }
}

void Tracker::SetReplay(bool enable)
{
    replay = enable;

    if (!replay) ClearHistory();
}

void Tracker::ClearHistory()
{
//    cout << "===============================================Clearing history=======================================================================" << endl;

    historyStartFrame = pipeline->parent->capture->GetFrameNumber();
//    cout << "H start = " << historyStartFrame << endl;
    history.clear();
    historyEntries.clear();
    historyEntriesIndex = 0;
}

void Tracker::LoadHistory(string filename)
{
    std::ifstream file(filename);
    std::string str;

    // eat up first line
    for (int i = 0; i < 10; i++)
        file >> str;

    history.clear();

    // read first line separately
    int count = 0;
    unsigned int frameNumber;
    Entity e;
    int readEntities = 0;

    if (!file.eof())
    {
        file >> str >> historyStartFrame >> e.lastFrameDetected >> e.lastFrameNotDetected >> str
             >> e.assigned >> e.zone >> e.size >> e.x >> e.y;

        historyEntries.push_back(HistoryEntry(historyStartFrame, 0));

        do
        {
            history.push_back(e);
            readEntities++;
	    
            file >> str >> frameNumber >> e.lastFrameDetected >> e.lastFrameNotDetected >> str
                 >> e.assigned >> e.zone >> e.size >> e.x >> e.y;
	    
            if (count == 0)
                if (frameNumber != historyStartFrame)
                    count = history.size();
	    
            if (count > 0 && (readEntities % count == 0))
            {
                historyEntries.push_back(HistoryEntry(frameNumber, history.size()-1));

            }
	    
        } while (!file.eof()); // if eof is reached while parsing data, do not record the entity
    }

    SetMaxEntities (count);
}
