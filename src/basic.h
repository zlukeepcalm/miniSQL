#define _AFXDLL

#ifndef _BASIC_H
#define _BASIC_H
#include <string.h>
#include <afx.h>

class AttributeValue
{
public:
	CString charValue;
	int charCount; 
	float floatValue;
	int intValue;
	int typeNo; //Define type

	AttributeValue();
	AttributeValue(const AttributeValue&);
	AttributeValue& operator=(const AttributeValue& a);
	~AttributeValue();
};

class Record
{
public:
	AttributeValue value[32];
	int attributeCount;
	Address addr;

	Record() {};
	Record(const Record&);
	Record& operator=(const Record& r);
};

class AttributeInformation//for every single attribute
{
public:
	CString type; 
	int typeCount; 
	bool unique; 
	CString attributeName; 
	bool hasIndex; 

	AttributeInformation();
	AttributeInformation(const AttributeInformation&);
	AttributeInformation& operator=(const AttributeInformation& a);
	int getSize();
};


class TableInformation
{
public:
	AttributeInformation info[32];//32 attributes at most
	int attributeCount;
	CString tableName;
	CString primaryKey;

	TableInformation() {};
	TableInformation(const TableInformation&);
	TableInformation& operator=(const TableInformation& t);
	int getTotalSize();
};

class Address
{
public:
	int blockAddress;
	int indexAddress;
};

class DBFile
{
public:
	CFile myfile;

	DBFile();
	~DBFile();
	int allocNewBlock();
	int findEnoughSpace(int,int);//create a space whose size and start address offset are set

};

class Compare
{
public:
	int item1;
	CString item2;
	int operation; // 0=,1<,2>,3<=,4>=,5!=
	int typeNo; 

	bool compareAll(const AttributeValue& a);
};


#endif