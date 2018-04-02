#define _AFXDLL

#ifndef _CATAlOG_MANAGER_H
#define _CATALOG_MANAGER_H

#include"basic.h"
class BufferManager;

class CatalogManager
{
private:
	BufferManager* bmp;
public:
	// ------------------------------------ basic function ------------------------------
	void setbufferManager(BufferManager* b)
	{
		this->bmp = b;
	}
	wchar_t* transformTableInfo(TableInformation a, int& count);
	TableInformation transformArrayToTableInfo(CString b, int count);

	// ------------------------------------ user interface function ----------------------
    void saveTable(TableInformation a);
    bool readTable(CString table_name, TableInformation& tableInfo);
	void dropTable(CString table_name);

};

#endif
