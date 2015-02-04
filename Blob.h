#ifndef BLOB_H
#define BLOB_H

class Blob 
{
public:
    unsigned int size;
    bool available;
    unsigned int assignment;
    unsigned int zone;
    unsigned int tagId;
    float angle;

    int x = 0;
    int y = 0;
    unsigned int index = 0;

    Blob ()
    {
	size = 0;
	available = true;
    }

    Blob (int x, int y, unsigned int index, unsigned int size = 0)
    {
	this->x = x;
	this->y = y;
	this->index = index;

	this->size = size;
	available = true;
    }
};


#endif
