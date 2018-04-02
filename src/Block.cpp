#include"basic.h"
#include"Block.h"

#include<cstdlib>
#include<iostream>
using namespace std;

Block::Block()
{
	block_position = -1;
	last_use = CTime::GetCurrentTime();
	valid = 0;
	modified = 0;
	dirty = 0;
	locked = 0;
	setRecordNum(0);
	setFreeSpaceEnd(BLOCKSIZE - 1);
	file = NULL;
}

Block::~Block()
{
}

bool Block::haveEnoughSize(int data_size)
{
	int left = getFreeSpaceEnd();
	int num = getRecordNum();
	left = left + 1 - 2*4 - 2*num*4;//reserve space for metadata
	return left >= data_size + 4*2;
}

int Block::addData(CString data, int data_size)
{
	if(haveEnoughSize(data_size) == 0)//if the Block:: do not have enough space, return error
		return -1;

	int total = getRecordNum();
	int i;
	for(i = 0; i < total; i++)//check whether there are empty record in the middle
		if(getDataSize(i) == 0)
			break;
	if(i != total){//if there exists empty space in the middle, start storing from the empty space	
		int index = i;
		setDataSize(i,data_size);
		int FreeSpaceEnd = getFreeSpaceEnd();//compute the space
		FreeSpaceEnd = FreeSpaceEnd - data_size;
		setFreeSpaceEnd(FreeSpaceEnd);
		setDataPoint(i,FreeSpaceEnd + 1);
		for(i = 0; i < data_size; i++)//store the data into Block::
			space[FreeSpaceEnd + 1 + i] = data[i];
		last_use = CTime::GetCurrentTime();
		modified = 1;
		return index;
	}
	else{//if not, start storing from the end of records	
		setRecordNum(total+1);
		setDataSize(total,data_size);
		int FreeSpaceEnd = getFreeSpaceEnd();//compute the space
		FreeSpaceEnd = FreeSpaceEnd - data_size;
		setFreeSpaceEnd(FreeSpaceEnd);
		setDataPoint(total,FreeSpaceEnd + 1);
		for(i = 0; i < data_size; i++)//store the data into Block
			space[FreeSpaceEnd + 1 + i] = data[i];
		last_use = CTime::GetCurrentTime();
		modified = 1;
		return total;
	}
}

void Block::readinData(int index, CString temp)
{
	int size = getDataSize(index);

	int length = 0;
	int p = getDataPoint(index);
	int end = p + getDataSize(index) - 1;
	for(; p <= end; p++, length++)//create a temp to get the data and return
		temp.GetAt(length,space[p]);
	last_use = CTime::GetCurrentTime();
}

int compare(const void*, const void*);
int compare(const void * a, const void * b)//set the function pointer for qsort()
{
	return ((item *)a)->point > ((item *)b)->point;
}
void Block::removeData(int index)
{
	int total = getRecordNum();
	if(index >= total)//the index you want to delete do not exist, return error
		return;
	
	if(getDataSize(index) == 0)//the record is already empty, cancel the action
		return;

	int oldSize = getDataSize(index);
	setDataSize(index,0);
	int current = getDataPoint(index);

    //since the insert is not in a strict order
    //when deleting is called, we have to set the records in order
	int size = 0;
	item* array = new item[total];
	for(int i =0; i < total; i++){//set an item array for quicksort	
		int p = getDataPoint(i);
		int s = getDataSize(i);
		if(s != 0 && p < current){		
			array[size].index = i;//item array is an empty record but have the same index,size and point
			array[size].size = s;
			array[size].point = p;
			size++;
		}
	}	
	
	int temp;
	if (size != 0)
	{
		qsort((void *)array, size, sizeof(item), compare);//sort following the order of point
/*
#ifdef DEBUG
		for (int i = 0; i < size; i++){
		cout << "[" << array[i].index << "," << array[i].point << "]" << endl;
		//printf( "%s", array[i].index );
		//printf( "%s", array[i].point );
	}
#endif
*/	
		for(int i = size - 1; i >= 0; i--)
		{
			if(i == size - 1)//the last record
			{
				temp = current + oldSize - array[i].size;
				for(int j = 1; j <= array[i].size; j++)
				    //replace target record with empty array
					space[current+oldSize-j] = space[array[i].point+array[i].size-j]; 
				setDataPoint(array[i].index,temp);
			}
			else//the other record
			{
				for(int j = 1; j <= array[i].size; j++)
				    //replace target record with empty array
					space[temp - j] = space[array[i].point + array[i].size - j];
				temp = temp - array[i].size;
				setDataPoint(array[i].index,temp);
			}
		}
	}
	//delete redundant space and set the pointer
	delete array;
	if(size != 0)
		setFreeSpaceEnd(temp-1);
	else
		setFreeSpaceEnd(current+oldSize-1);
	if(index == total - 1)
		setRecordNum(total - 1);
	last_use = CTime::GetCurrentTime();
	modified = 1;
}


//access disk derictly
void Block::saveToDisk()
{
	if(file == NULL)
		return;
	if(valid && modified)
	{
		if(block_position != -1)
		{
			file->Seek(BLOCKSIZE*block_position,CFile::begin);
			file->Write((void*)space,BLOCKSIZE);
		}
	}
}

void Block::getFromDisk(int block_position, CFile* file)
{
	if((unsigned int)block_position >= file->GetLength()/BLOCKSIZE)	//testing whether there is an overflow occured
	{
		this->block_position = -1;
		return;
	}
	this->file = file;
	file->Seek(block_position*BLOCKSIZE,CFile::begin);
	file->Read(space,BLOCKSIZE);
	this->block_position = block_position;
	last_use = CTime::GetCurrentTime();
	valid = 1;
	modified = 0;
}

//some basic function
char* Block::getHead()
{
	return space;
}

void Block::setBlockAddress(int position)
{
	block_position = position;
}

int Block::getBlockAddress()
{
	return block_position;
}

bool Block::IsValid()
{
	return valid;
}

void Block::setValid(bool flag)
{
	valid = flag;
}

CTime Block::getUseTime()
{
	return last_use;
}

int Block::getRecordNum()
{
	int temp;
	temp = ((int *)space)[0];
	return temp;
}

void Block::setRecordNum(int num)
{
	((int*)space)[0] = num;
}

int Block::getFreeSpaceEnd()
{
	return ((int *)space)[1];
}

void Block::setFreeSpaceEnd(int end)
{
	((int*)space)[1] = end;
}

void Block::setDataSize(int index, int size)
{
	((int*)space)[2+2*index] = size;
}

int Block::getDataSize(int index)
{
	return ((int*)space)[2+2*index];
}

void Block::setDataPoint(int index, int position)
{
	((int*)space)[2+2*index+1] = position;
}

int Block::getDataPoint(int index)
{
	return ((int*)space)[2+2*index+1];
}

//these function is used to move the pointer into former or next record and index
void Block::setNextLink(int index, Address next)
{
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 8;
	((int *)(space + p))[0] = next.blockAddress;
	((int *)(space + p))[1] = next.indexAddress;
	last_use = CTime::GetCurrentTime();
	modified = 1;
}

void Block::getNextLink(int index, Address& next)
{	
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 8;
	next.blockAddress = ((int *)(space + p))[0];
	next.indexAddress = ((int *)(space + p))[1];
	last_use = CTime::GetCurrentTime();
}

void Block::setRecordNextLink(int index, Address next)
{
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 16;
	((int *)(space + p))[0] = next.blockAddress;
	((int *)(space + p))[1] = next.indexAddress;
	last_use = CTime::GetCurrentTime();
	modified = 1;
}

void Block::getRecordNextLink(int index, Address& next)
{
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 16;
	next.blockAddress = ((int *)(space + p))[0];
	next.indexAddress = ((int *)(space + p))[1];
	last_use = CTime::GetCurrentTime();
}

void Block::setRecordFrontLink(int index, Address front)
{
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 24;
	((int *)(space + p))[0] = front.blockAddress;
	((int *)(space + p))[1] = front.indexAddress;
	last_use = CTime::GetCurrentTime();
	modified = 1;
}

void Block::getRecordFrontLink(int index, Address& front)
{
	int p, size;
	p = getDataPoint(index);
	size = getDataSize(index);
	p = p + size - 24;
	front.blockAddress = ((int *)(space + p))[0];
	front.indexAddress = ((int *)(space + p))[1];
	last_use = CTime::GetCurrentTime();
}

void Block::setIndexNext(int index, Address next)
{
	setRecordNextLink(index, next);
}

void Block::getIndexNext(int index, Address next)
{
	getRecordNextLink(index, next);
}