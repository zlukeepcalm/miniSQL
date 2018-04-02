#define _AFXDLL

#ifndef _BLOCK_H
#define _BLOCK_H

#include"basic.h"
#include<cstring>
#include<afx.h>

class DBFile;
class Address;

const int BLOCKSIZE = 4096;


struct item
{
	int index;
	int size;
	int point;
};

class Block{
public:
	char space[BLOCKSIZE];//set free space, using array to store data
	int block_position;//point out the address of block in the whole file
	CTime last_use;//update the last access time to ensure serializability
	bool valid;//valid and modified flag
	bool modified;
	bool deleted;
	CFile* file;
	
public:	
	//basic function
	Block();
	~Block();
    bool haveEnoughSize(int);
	char* getHead();
	bool IsValid();
	void setValid(bool);
	CTime getUseTime();

    //gets and sets, basically record and index operations
	int getRecordNum();
	void setRecordNum(int);
	int getFreeSpaceEnd();
	void setFreeSpaceEnd(int);
	int getDataSize(int);
	void setDataSize(int,int);
	int getDataPoint(int);
	void setDataPoint(int,int);
	void setBlockAddress(int);
	int getBlockAddress();
	void setNextLink(int , Address);
	void getNextLink(int , Address&);
	void setRecordNextLink(int , Address);
	void getRecordNextLink(int , Address&);
	void setRecordFrontLink(int , Address);
	void getRecordFrontLink(int , Address&);
	void setIndexNext(int , Address);
	void getIndexNext(int , Address);
    
    //user interface
	int addData(CString data, int data_size);
	void readinData(int, CString);
	void removeData(int index);
	void saveToDisk();
	void getFromDisk(int, CFile*);

};

#endif