#define _AFXDLL

#ifndef _RECORD_MANAGER_H
#define _RECORD_MANAGER_H

#include"basic.h"
#include"BufferManager.h"
#include"afxtempl.h"
class BufferManager;

class RecordManager
{
public:

	void setbufferManager(BufferManager* bmp)
	{
		this->bmp = bmp;
	}
	CString recordToArray(Record newRecord, int& size);
	Record* arrayToRecord(CString recordPointer,TableInformation recordInfo);

CArray<Record, Record&>* readRecord(CString tableName, TableInformation tableInfo, CArray<Compare, Compare&>* cp = NULL);
	void storeRecord(CString tableName, Record data);
	void removeRecord(CString tableName, TableInformation* tableInfo = NULL, CArray<Compare, Compare&>* cp = NULL);
	
private:
	BufferManager* bmp;
};

#endif  //_RECORD_MANAGER_H
