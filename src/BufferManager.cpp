#include"basic.h"
#include"Block.h"
#include"BufferManager.h"

// --------------------------- basic buffer function --------------------------

BufferManager::BufferManager()
{
	bufferContainer.clear();
}

BufferManager::~BufferManager()
{
	for(int i = 0; i < 100; i++)
		buffer[i].saveToDisk();
}

int BufferManager::readBlockFromDisk(int blockAddress)	
{
	std::set<int>::const_iterator cit;
	cit = bufferContainer.find(blockAddress);
	if(cit != bufferContainer.end())
	{
		for(int i = 0; i < 100; i++)
			if(buffer[i].getBlockAddress() == blockAddress)
				return i;
	}

	int i = findReplaceBlock();

	buffer[i].saveToDisk();
	if(buffer[i].IsValid())
		bufferContainer.erase(buffer[i].getBlockAddress());
	buffer[i].getFromDisk(blockAddress, &m_file.myfile);
	bufferContainer.insert(blockAddress);
	return i;
}

int BufferManager::findReplaceBlock()
{	
	int replacePosition = 0;
	CTime earliestTime = buffer[0].getUseTime();
	int i;
	for(i = 0; i < 100; i++)
	{
		if(!buffer[i].IsValid())
			break;
		if(earliestTime > buffer[i].getUseTime())
		{
			replacePosition = i;
			earliestTime = buffer[i].getUseTime();
		}
	}
	if(i != 100)
		return i;
	else
		return replacePosition;
}

// ----------------------------- user interface function -------------------------------

int BufferManager::storeData(CString head, int size, Address& returnAddress)
{
	int replacePosition = 0;
	CTime earliestTime = buffer[0].getUseTime();
	int i;
	for(i = 0; i < 100; i++)
	{
		if(!buffer[i].IsValid() || 
			(buffer[i].haveEnoughSize(size) && buffer[i].getBlockAddress() != 0))
			break;
		if(earliestTime > buffer[i].getUseTime())
		{
			replacePosition = i;
			earliestTime = buffer[i].getUseTime();
		}
	}
	if(i != 100)
	{
		if(buffer[i].IsValid())
		{
			returnAddress.blockAddress = buffer[i].getBlockAddress();
			returnAddress.indexAddress = buffer[i].addData(head,size);
		}
		else
		{
			int free_block = 0;
			std::set<int>::const_iterator cit;
			while(1)
			{
				free_block = m_file.findEnoughSpace(free_block + 1, size);
				cit = bufferContainer.find(free_block);
				if(cit == bufferContainer.end())
					break;
			}
			buffer[i].getFromDisk(free_block, &m_file.myfile);
			bufferContainer.insert(free_block);
			returnAddress.blockAddress = free_block;
			returnAddress.indexAddress = buffer[i].addData(head, size);
		}
		return i;
	}
	else
	{
		int free_block = 0;
		std::set<int>::const_iterator cit;
		while(1)														
		{
			free_block = m_file.findEnoughSpace(free_block + 1, size);
			cit = bufferContainer.find(free_block);
			if(cit == bufferContainer.end())
				break;
		}		
		buffer[replacePosition].saveToDisk();
		bufferContainer.erase(buffer[replacePosition].getBlockAddress());
		buffer[replacePosition].getFromDisk(free_block, &m_file.myfile);
		bufferContainer.insert(free_block);
		returnAddress.blockAddress = free_block;
		returnAddress.indexAddress = buffer[replacePosition].addData(head, size);
		return replacePosition;
	}
}


CString BufferManager::readData(Address address, int& size)
{
	int i = readBlockFromDisk(address.blockAddress);
	CString head;
	size = buffer[i].getDataSize(address.indexAddress);
	head = new char[size];
	buffer[i].readinData(address.indexAddress, head);
	return head;
}

void BufferManager::removeData(Address address)
{
	int i = readBlockFromDisk(address.blockAddress);
	buffer[i].removeData(address.indexAddress);
}

int BufferManager::addTableEntry(CString tableName, Address &returnAddress)
{
	int size = 0;
	size = tableName.GetLength() + 1 + 6*sizeof(int);
	char* tableEntryHead = new char[size];
	int i;
	for(i = 0; i < tableName.GetLength(); i++)
		tableEntryHead[i] = tableName.GetAt(i);
	tableEntryHead[i++] = '\0';
	((int *)(tableEntryHead + i))[0] = ((int *)(tableEntryHead + i))[1]
		= ((int *)(tableEntryHead + i))[2] = ((int *)(tableEntryHead + i))[3]
		= ((int *)(tableEntryHead + i))[4] = ((int *)(tableEntryHead + i))[5]
		= -1;

	int j = storeData(tableEntryHead, size, returnAddress);
	i = readBlockFromDisk(0);
	Address head_add;
	buffer[i].readinData(0,(char *)&head_add);
	buffer[j].setNextLink(returnAddress.indexAddress, head_add);
	buffer[i].setNextLink(0,returnAddress);
	delete [] tableEntryHead;
	return j;
}


// --------------------------- advanced interface function -------------------------------

// ***********************  tableInfo functions *****************************
void BufferManager::addInfo(CString head, int size)
{
	Address store_add;
	int j = storeData(head,size,store_add);
	int i = readBlockFromDisk(0);
	Address head_add;
	buffer[i].readinData(1,(char *)&head_add);
	buffer[j].setNextLink(store_add.indexAddress, head_add);
	buffer[i].setNextLink(1,store_add);
}

CString BufferManager::readInfo(CString tableName, int& size)
{	
	int num;
	CString a;
	Address addr;
	CString info;

    addr.blockAddress = 0;
	addr.indexAddress = 1;
	num = readBlockFromDisk(addr.blockAddress);
    buffer[num].getNextLink(addr.indexAddress,addr);
	
    while (addr.indexAddress != -1)
	{
	   num = readBlockFromDisk(addr.blockAddress);
	   size = buffer[num].getDataSize(addr.indexAddress);
       info = new char[size];
       buffer[num].readinData(addr.indexAddress, info);
	   a = info;
       if (a == tableName)
		   break;
	   else
		   delete [] info;
       buffer[num].getNextLink(addr.indexAddress,addr);
	}

	if (addr.indexAddress == -1)
	{
	    size = -1;
     	info.Empty();
	}
	return info;
}

void BufferManager::removeInfo(CString tableName)
{
	int i = readBlockFromDisk(0);
	Address add;
	buffer[i].getNextLink(1,add);

	Address oldAdd, nextAdd;
	CString name;
	int size;
	oldAdd.blockAddress = 0;
	oldAdd.indexAddress = 1;
	int j;
	while(add.blockAddress != -1 && add.indexAddress != -1)
	{
		j = readBlockFromDisk(add.blockAddress);
		size = buffer[j].getDataSize(add.indexAddress);
		CString TableInfo = new char[size];
		buffer[j].readinData(add.indexAddress,TableInfo);
		buffer[j].getNextLink(add.indexAddress,nextAdd);
		name = TableInfo;
		if(name == tableName)
		{
			int front = readBlockFromDisk(oldAdd.blockAddress);
			buffer[front].setNextLink(oldAdd.indexAddress, nextAdd);
			removeData(add);
			delete [] TableInfo;
			break;
		}
		delete [] TableInfo;
		oldAdd.blockAddress = add.blockAddress;
		oldAdd.indexAddress = add.indexAddress;
		add.blockAddress = nextAdd.blockAddress;
		add.indexAddress = nextAdd.indexAddress;
	}
}


//  ****************************** record functions **********************************

void BufferManager::findTableEntry(CString tableName, Address& returnAddress)
{
	int i = readBlockFromDisk(0);
	Address add;
	buffer[i].readinData(0, (char *)&add);

	CString table;
	int size;
	CString tableEntry;
	while(add.blockAddress != -1 && add.indexAddress != -1)
	{
		tableEntry = readData(add, size);	
		table = tableEntry;
		if(table == tableName)
			break;
		char *tempchar = (LPSTR)(LPCTSTR)tableEntry;
		add.blockAddress = ((int *)(tempchar + size - 8))[0];
		add.indexAddress = ((int *)(tempchar + size - 8))[1];
		delete [] tableEntry;
	}
	returnAddress.blockAddress = add.blockAddress;
	returnAddress.indexAddress = add.indexAddress;
}

void BufferManager::removeTableEntry(CString tableName)
{
	Address add, oldAdd, nextAdd;
	oldAdd.blockAddress = 0;
	oldAdd.indexAddress = 0;
	int i = readBlockFromDisk(0);
	buffer[i].getNextLink(0,add);
	CString name;
	while(add.blockAddress != -1 && add.indexAddress != -1)
	{
		CString temp;
		int size;
		temp = readData(add, size);
		name = temp;
		char *tempchar = (LPSTR)(LPCTSTR)temp;
		nextAdd.blockAddress = ((int *)(tempchar + size - 8))[0];
		nextAdd.indexAddress = ((int *)(tempchar + size - 8))[1];
		if(name == tableName)
		{
			int j = readBlockFromDisk(oldAdd.blockAddress);
			buffer[j].setNextLink(oldAdd.indexAddress, nextAdd);
			int m = readBlockFromDisk(add.blockAddress);
			buffer[m].removeData(add.indexAddress);
			break;
		}
		delete [] temp;
		oldAdd = add;
		add = nextAdd;
	}
}

void BufferManager::addRecord(CString tableName, CString head, int size)
{
	Address add, dataAdd, nextAdd;
	findTableEntry(tableName, add);
	if(add.blockAddress == -1 && add.indexAddress == -1)
	{
		int i = addTableEntry(tableName,add);
		int j = storeData(head,size,dataAdd);
		buffer[i].setRecordFrontLink(add.indexAddress, dataAdd);
		buffer[i].setRecordNextLink(add.indexAddress, dataAdd);
		buffer[j].setRecordFrontLink(dataAdd.indexAddress, add);
		buffer[j].setRecordNextLink(dataAdd.indexAddress, add);
	}
	else
	{
		int i = readBlockFromDisk(add.blockAddress);
		buffer[i].getRecordNextLink(add.indexAddress, nextAdd);
		if(nextAdd.blockAddress != -1 && nextAdd.indexAddress != -1)
		{
			int j = storeData(head, size, dataAdd);
			buffer[i].getRecordNextLink(add.indexAddress, nextAdd);
			int m = readBlockFromDisk(nextAdd.blockAddress);

			buffer[j].setRecordFrontLink(dataAdd.indexAddress, add);
			buffer[j].setRecordNextLink(dataAdd.indexAddress, nextAdd);
			buffer[i].setRecordNextLink(add.indexAddress, dataAdd);
			buffer[m].setRecordFrontLink(nextAdd.indexAddress, dataAdd);
		}
		else
		{
			int j = storeData(head, size, dataAdd);
			buffer[j].setRecordFrontLink(dataAdd.indexAddress, add);
			buffer[j].setRecordNextLink(dataAdd.indexAddress, add);
			buffer[i].setRecordFrontLink(add.indexAddress, dataAdd);
			buffer[i].setRecordNextLink(add.indexAddress, dataAdd);
		}
	}
}

Address BufferManager::nextRecordPosition(Address currentAdd)
{
	Address nextAdd;
	int i = readBlockFromDisk(currentAdd.blockAddress);
	buffer[i].getRecordNextLink(currentAdd.indexAddress, nextAdd);
	return nextAdd;
}

void BufferManager::removeRecord(Address add)
{
	int i = readBlockFromDisk(add.blockAddress);
	Address frontAdd, nextAdd, end;
	end.blockAddress = end.indexAddress = -1;
	buffer[i].getRecordFrontLink(add.indexAddress, frontAdd);
	buffer[i].getRecordNextLink(add.indexAddress, nextAdd);
	if(frontAdd.blockAddress == nextAdd.blockAddress && 
		frontAdd.indexAddress == nextAdd.indexAddress)
	{
		buffer[i].removeData(add.indexAddress);
		i = readBlockFromDisk(frontAdd.blockAddress);
		buffer[i].setRecordFrontLink(frontAdd.indexAddress, end);
		buffer[i].setRecordNextLink(frontAdd.indexAddress, end);
	}
	else
	{
		buffer[i].removeData(add.indexAddress);
		i = readBlockFromDisk(frontAdd.blockAddress);
		buffer[i].setRecordNextLink(frontAdd.indexAddress, nextAdd);
		int j = readBlockFromDisk(nextAdd.blockAddress);	
		buffer[j].setRecordFrontLink(nextAdd.indexAddress, frontAdd);
	}
}