#pragma warning(disable:4996)

#include"basic.h"
#include"CatalogManager.h"
#include"BufferManager.h"
#include <stdlib.h>

wchar_t* CatalogManager::transformTableInfo(TableInformation a, int& count)
{
	int size[32];
	int table_size, attribute_size = 0;
	for(int i = 0; i < a.attributeCount; i++)
	{
	     size[i] = a.info[i].attributeName.GetLength() + a.info[i].type.GetLength() + sizeof(int) + sizeof(char)*2 + 2;
	}
	int i;
	for(i = 0; i < a.attributeCount; i++)
		attribute_size += size[i];
    table_size = a.tableName.GetLength() + a.primaryKey.GetLength() + a.attributeCount*sizeof(int) + attribute_size + 2 +8;
		
    wchar_t* b = new wchar_t[table_size];
	wchar_t* p = b;
	_tcscpy(p, a.tableName);
	p += a.tableName.GetLength();
	*p = '\0';
	p += 1;
    _tcscpy(p, a.primaryKey);
    p += a.primaryKey.GetLength();
	*p = '\0';
    p += 1;
	
		
	for(i=0; i < a.attributeCount; i++)
	{	
		*((int*)p) = size[i];
		p += 4; 
        	_tcscpy(p, a.info[i].attributeName);
		p += a.info[i].attributeName.GetLength();
 		*p = '\0';
		p += 1;
       		*p = (char)a.info[i].unique;
		p += 1;
		_tcscpy(p, a.info[i].type);
		p += a.info[i].type.GetLength();
		*p = '\0';
		p += 1;
		*((int*)p) = a.info[i].typeCount;
		p += 4;
        	*p = (char)a.info[i].hasIndex;
		p += 1;
	}
    count = table_size;
	return b;
}

TableInformation CatalogManager::transformArrayToTableInfo(CString b, int count)
{
	TableInformation a;
	int i, k;
	int size, upper;
	char *tb = (LPSTR)(LPCTSTR)b;
	for(i = 0; i < count && *(tb + i) != '\0'; i++)
    {
        a.tableName += *(tb + i);
	}
    for(i += 1; i < count && *(tb + i) != '\0' ; i++) 
    {
		a.primaryKey += *(tb + i);
	}
	i += 1; 

	for(k = 0; i < count; i++,k++)
	{
		size = *((int*)(tb + i));
		i += 4;
		upper = i + size;
		for(; i < upper && *(tb + i) != '\0'; i++)
       	{
			a.info[k].attributeName += *(tb + i);
		}

		i += 1;
		a.info[k].unique = (*(tb + i)) == 1 ? true : false;

        for(i += 1; i < upper && *(tb + i) != '\0'; i++)
        {
			a.info[k].type += *(tb + i);			
		}
		i += 1;
		a.info[k].typeCount = *((int*)(tb + i));
		i += 4;
		a.info[k].hasIndex = (*(tb + i)) == 1 ? true : false;
	}
	a.attributeCount = k - 1;
	return a;
} 

void CatalogManager::saveTable(TableInformation a)
{
	int arrysize;
	wchar_t* p = transformTableInfo(a, arrysize);
	bmp->addInfo(p, arrysize);
	delete [] p;
}


bool CatalogManager::readTable(CString table_name, TableInformation& tableInfo)
{
	int arrysize;
	CString p;
	
	p = bmp->readInfo(table_name, arrysize);
	if(arrysize != -1)
	{
		tableInfo = transformArrayToTableInfo(p, arrysize);
		return true;
	}
	else
		return false;
}

void CatalogManager::dropTable(CString table_name)
{
	bmp->removeInfo(table_name);
}