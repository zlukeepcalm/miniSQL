#define _AFXDLL

#ifndef _ITERPRETER_H
#define _ITERPRETER_H

#include<afx.h>
#include<set>
class BufferManager;
class CatalogManager;
class RecordManager;

class interpreter{
public:
	BufferManager* bm;
	CatalogManager* cm;
	RecordManager* rm;
	std::set<int> iset;
	std::set<float> fset;
	std::set<CString> cset;
	CString currentTableName;
	int current_a;
	bool has_pk;
	void printString(const CString& c);
public:
	interpreter();
	~interpreter();
	bool InitCommand(CString &cmd);
	bool ExecuteCommand(CString cmd);
	bool insert(CString table_name, CString info);
	bool CreateTable(CString TableName,CString TableInfo);
	bool Delete(CString Table , CString info);
	bool print1(CString Table_name , CString attr);
    bool print2(CString Table_name , CString attr , CString info);
	bool DropTable(CString table_name); //
	bool CreateIndex(CString IndexName, CString TableName, CString AttName, bool unique); //
	bool DropIndex(CString IndexName); //
	bool Select(CString Selection, CString TableName, CString Condition); //
};


#endif