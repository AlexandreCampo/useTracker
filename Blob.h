#ifndef BLOB_H
#define BLOB_H

class Blob 
{
public:
    unsigned int size = 0;
    bool available = true;
    unsigned int assignment;
    unsigned int zone;
    unsigned int tagId;
    float angle = 0.0;
    float length = 0.0;

    int x = 0;
    int y = 0;
    unsigned int index = 0;

    Blob ()
    {
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
