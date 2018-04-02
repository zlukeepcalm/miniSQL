#include"basic.h"
#include"BufferManager.h"
#include"RecordManager.h"

#include<iostream>
using namespace std;

CString RecordManager::recordToArray(Record newRecord, int& size)
{
	size=0;
	int recordNum;
	for (recordNum=0; recordNum < newRecord.attributeCount; recordNum++)//count the size
	{
		if (newRecord.value[recordNum].typeNo==0)//if char, add 1
			size+=newRecord.value[recordNum].charCount;	
		else
			size+=4;//if int or float, add 4
	}
	        size=size+4*6;//???????bianhao de daxiao?
	char* recordArray=new char[size];
	CString recordPointer=recordArray;
	for (recordNum=0; recordNum < newRecord.attributeCount;recordNum++)
	{//store them in an Array
		if (newRecord.value[recordNum].typeNo==0)
		{
			for (int i=0;i<newRecord.value[recordNum].charCount;i++,recordArray++)
			{
				*recordArray = newRecord.value[recordNum].charValue[i];
			}
		}
		else if (newRecord.value[recordNum].typeNo==1)
		{
			*((float *)recordArray)=newRecord.value[recordNum].floatValue;
			recordArray+=4;
		}
		else if (newRecord.value[recordNum].typeNo==2)
		{
			*((int *)recordArray)=newRecord.value[recordNum].intValue;
			recordArray+=4;
		}
	}
	cout << "recordToArray done!" << endl;
	return  recordPointer;
}

Record* RecordManager::arrayToRecord(CString recordPointer,TableInformation recordInfo)
{
	Record* returnRecord = new Record;
	(*returnRecord).attributeCount = recordInfo.attributeCount;
	for(int i=0;i<recordInfo.attributeCount;i++)
	{//store array into record
		if(recordInfo.info[i].type=="int")
		{
			char* recordArray = new char[];		
			(*returnRecord).value[i].intValue=*((int *)recordArray);
			(*returnRecord).value[i].typeNo=2;
			recordArray += 4;
			recordPointer = recordArray;
		}
		else if (recordInfo.info[i].type=="char")
		{
			(*returnRecord).value[i].charValue= new char[recordInfo.info[i].typeCount];
			(*returnRecord).value[i].charCount=recordInfo.info[i].typeCount;
			CString p = (*returnRecord).value[i].charValue;
			char* recordArray = new char[];
			for(int j = 0; j < recordInfo.info[i].typeCount; j++)
				p.SetAt(j,recordArray[j]);
			recordArray += recordInfo.info[i].typeCount;
			recordPointer = recordArray;
			(*returnRecord).value[i].typeNo=0;
		}
		else if (recordInfo.info[i].type=="float")
		{
			char* recordArray = new char[];
			(*returnRecord).value[i].floatValue = *((float *)recordArray);
			(*returnRecord).value[i].typeNo=1;
			recordArray += 4;
			recordPointer = recordArray;
		}		
	}
	cout << "arrayToRecord done!" << endl;
	return returnRecord;
}


CArray<Record, Record&>* 
RecordManager::readRecord(CString tableName, TableInformation tableInfo, CArray<Compare, Compare&>* cp)
{
	if(cp == NULL)
	{
		Address add, headadd;
		CArray<Record, Record&>* rp = new CArray<Record, Record&>;
		bmp->findTableEntry(tableName, add);
		if(add.blockAddress == -1 || add.indexAddress == -1)
		{
			delete rp;
			return NULL;
		}

		headadd = add;
		int size;
		CString content;
		Record* p;
		
		add = bmp->nextRecordPosition(add);
		if(add.blockAddress != -1 && add.indexAddress != -1)
		{
			while(add.blockAddress != headadd.blockAddress || add.indexAddress != headadd.indexAddress)
			{
				content = bmp->readData(add,size);
				p = arrayToRecord(content, tableInfo);
				p->addr = add;
				rp->Add(*p);
				//delete [] content;
				content.Empty();
				add = bmp->nextRecordPosition(add);
			}
			return rp;
		}
		else
		{
			delete rp;
			return NULL;
		}
	}
	else
	{
		Address add, headadd;
		CArray<Record, Record&>* rp = new CArray<Record, Record&>;
		bmp->findTableEntry(tableName, add);
		if(add.blockAddress == -1 || add.indexAddress == -1)
		{
			delete rp;
			return NULL;
		}
		
		headadd = add;
		int size;
		CString content;
		Record* p;
		
		add = bmp->nextRecordPosition(add);
		if(add.blockAddress != -1 && add.indexAddress != -1)
		{
			while(add.blockAddress != headadd.blockAddress || add.indexAddress != headadd.indexAddress)
			{
				content = bmp->readData(add,size);
				p = arrayToRecord(content, tableInfo);			
				p->addr = add;
				int i;
				for(i = 0; i < cp->GetSize(); i++)
				{
					AttributeValue av = p->value[(*cp)[i].item1];
					if(!(*cp)[i].compareAll(av))
						break;
				}
				if(i == cp->GetSize())
					rp->Add(*p);
				
				delete [] content;
				add = bmp->nextRecordPosition(add);
			}
			return rp;
		}
		else
		{
			delete rp;
			return NULL;
		}
	}
}



void RecordManager::storeRecord(CString tableName, Record data)
{
	CString head;
	int size;
	head = recordToArray(data, size);
	bmp->addRecord(tableName, head, size);
	delete [] head;
}



void RecordManager::removeRecord(CString tableName, TableInformation* tableInfo, CArray<Compare, Compare&>* cp)
{
	CArray<Record, Record&>* rp;
	if(cp == NULL)
	{
		Address headAdd, add, nextAdd;
		bmp->findTableEntry(tableName, headAdd);
		if(headAdd.blockAddress == -1 || headAdd.indexAddress == -1)
			return;
		else
		{
			add = bmp->nextRecordPosition(headAdd);
			if(add.blockAddress != -1 && add.indexAddress != -1)
			{
				while(headAdd.blockAddress != add.blockAddress 
					|| headAdd.indexAddress != add.indexAddress)
				{
					int i = bmp->readBlockFromDisk(add.blockAddress);
					bmp->buffer[i].getRecordNextLink(add.indexAddress, nextAdd);
					bmp->buffer[i].removeData(add.indexAddress);
					add = nextAdd;
				}
				bmp->removeTableEntry(tableName);
			}
			else
				bmp->removeTableEntry(tableName);
		}
	}
	else
	{
		rp = readRecord(tableName, *tableInfo, cp);
		if(rp != NULL)
		{
			for(int i = 0; i < rp->GetSize(); i++)
			{
				bmp->removeRecord((*rp)[i].addr);
			}
			delete rp;
		}
	}
}
