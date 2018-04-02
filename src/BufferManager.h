#ifndef _BUFFER_MANAGER_H
#define _BUFFER_MANAGER_H

#define _AFXDLL
#include"basic.h"
#include"Block.h"
#include<cstring>
#include<set>
using namespace std;

class BufferManager
{
public:
	DBFile m_file;
	Block buffer[100];
	std::set<int> bufferContainer;

public:
	//  basic function
	BufferManager();
	~BufferManager();
	int readBlockFromDisk(int blockPosition);
	int findReplaceBlock();

	//interface function 
	int storeData(CString head, int size, Address& returnAddress);
	CString readData(Address address, int& size);  												
	void removeData(Address address);

	void addRecord(CString tableName, CString head, int size);  
	void removeRecord(Address add);
	Address nextRecordPosition(Address currentAdd);
	void findTableEntry(CString tableName, Address& returnAddress);
	
	void addInfo(CString head, int size);
	CString readInfo(CString tableName, int& size);
	void removeInfo(CString tableName);	
	int addTableEntry(CString tableName, Address& returnAddress);	
	void removeTableEntry(CString tableName);
	
};


#endif
