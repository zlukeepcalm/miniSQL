#include<afx.h>
#include "IndexManager.h"

void BTree::InsertValue(void *pValue, int Block, int Index)
{
	if(m_iRoot[0] < 0 || m_iRoot[1] < 0 || m_file == NULL)
		throw new Exception("no root when find value in B+tree");
	if((m_sKeyType != "char" && m_sKeyType != "int" && m_sKeyType != "float") ||
		m_iKeyTypeCount <= 0 )
		throw new Exception("error input for find value in b+");
	if(m_sKeyType == "char") {
		Node<char> tmpNode(m_iKeyTypeCount + 1, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((char*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();			
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);	
			tmpNode.Read();
		}
		tmpNode.InsertKey((char*)pValue, Block, Index);	
	}
	else if(m_sKeyType == "int") {
		Node<int> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((int*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		tmpNode.InsertKey((int*)pValue, Block, Index);
	}
	else if(m_sKeyType == "float") {
		Node<float> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((float*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		tmpNode.InsertKey((float*)pValue, Block, Index);
	}
}

void BTree::DeleteValue(void* pValue)
{
	if(m_iRoot[0] < 0 || m_iRoot[1] < 0 || m_file == NULL)
		throw new Exception("no root when find value in B+tree");
	if((m_sKeyType != "char" && m_sKeyType != "int" && 
		m_sKeyType != "date" && m_sKeyType != "float") ||
		m_iKeyTypeCount <= 0 )
		throw new Exception("error input for find value in b+");
	if(m_sKeyType == "char") {
		Node<char> tmpNode(m_iKeyTypeCount + 1, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {			
			FindIndex = tmpNode.FindGreaterKeyIndex((char*)pValue);
			if(FindIndex == -1)										
				FindIndex = tmpNode.GetCount();						
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);	
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);		
			tmpNode.Read();
		}
		tmpNode.DeleteKey(tmpNode.FindKeyIndex((char*)pValue));	
	}
	else if(m_sKeyType == "int") {
		Node<int> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((int*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		tmpNode.DeleteKey(tmpNode.FindKeyIndex((int*)pValue));
	}
	else if(m_sKeyType == "float") {
		Node<float> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((float*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		tmpNode.DeleteKey(tmpNode.FindKeyIndex((float*)pValue));
	}
}

void BTree::DropNode(int Block, int Index)			
{
	if(m_file == NULL)
		throw new Exception("no root when find value in B+tree");
	if((m_sKeyType != "char" && m_sKeyType != "int" && m_sKeyType != "float") ||
		m_iKeyTypeCount <= 0 )
		throw new Exception("error input for find value in b+");
	if(m_sKeyType == "char") {
		Node<char> tmpNode(m_iKeyTypeCount + 1, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetSelfPosition(Block, Index);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		if(tmpNode.IsLeaf())				
			tmpNode.FreeNode();					
		else {												
			for(int i = 0; i < tmpNode.GetCount(); ++i) {
				tmpNode.GetPointer(i, tmpBlock, tmpIndex);
				DropNode(tmpBlock, tmpIndex);
			}
			tmpNode.FreeNode();										
		}
	}
	else if(m_sKeyType == "int") {
		Node<int> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetSelfPosition(Block, Index);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		if(tmpNode.IsLeaf())
			tmpNode.FreeNode();
		else {
			for(int i = 0; i < tmpNode.GetCount(); ++i) {
				tmpNode.GetPointer(i, tmpBlock, tmpIndex);
				DropNode(tmpBlock, tmpIndex);
			}
			tmpNode.FreeNode();	
		}
	}
	else if(m_sKeyType == "float") {
		Node<float> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetSelfPosition(Block, Index);
		tmpNode.Read();

		int tmpBlock, tmpIndex;
		if(tmpNode.IsLeaf())
			tmpNode.FreeNode();
		else {
			for(int i = 0; i < tmpNode.GetCount(); ++i) {
				tmpNode.GetPointer(i, tmpBlock, tmpIndex);
				DropNode(tmpBlock, tmpIndex);
			}
			tmpNode.FreeNode();	
		}
	}
}

void BTree::Drop()								
{
	if(m_iRoot[0] < 0 || m_iRoot[1] < 0 ||
		m_iFirstLeaf[0] < 0 || m_iFirstLeaf[1] < 0
		|| m_file == NULL)
		throw new Exception("error when drop B+");
	DropNode(m_iRoot[0], m_iRoot[1]);						

	m_sKeyType = "";
	m_iKeyTypeCount = 0;

	m_iRoot[0] = -1;
	m_iRoot[1] = -1;
	m_iFirstLeaf[0] = -1;
	m_iFirstLeaf[1] = -1;

	m_iTreePosition[0] = -1;
	m_iTreePosition[1] = -1;
}

void BTree::UpdateBPlusTree()	
{
	int tmpBlock, tmpIndex;
	GetSelfPosition(tmpBlock, tmpIndex);
	int Size = 100;
	m_file->Write(&m_iRoot[0],2*sizeof(int));
	m_file->Write(&m_iRoot[1], 2*sizeof(int));
	m_file->Write(&m_iFirstLeaf[0], 2*sizeof(int));
	m_file->Write(&m_iRoot[0], sizeof(int)*2);
}

BTree::BTree()
{
	m_sKeyType = "";
	m_iKeyTypeCount = 0;

	m_iRoot[0] = -1;
	m_iRoot[1] = -1;
	m_iFirstLeaf[0] = -1;
	m_iFirstLeaf[1] = -1;

	m_iTreePosition[0] = -1;
	m_iTreePosition[1] = -1;
	m_file = NULL;
}

BTree::~BTree(void)	
{
	while(m_aReaders.GetSize() != 0) {
		if(m_aReaders.GetAt(0) == NULL) {
			m_aReaders.RemoveAt(0);
		}
		else {
			delete (BTreeReader*)(m_aReaders.GetAt(0));
			m_aReaders.RemoveAt(0);
		}
	}
}

void BTree::CreateBPlusTree(CString KeyType, int KeyTypeCount)
{
	if(m_file == NULL)
		throw new Exception("init before create b+tree");
	m_sKeyType = KeyType;
	m_iKeyTypeCount = KeyTypeCount;
	if((m_sKeyType != "char" && m_sKeyType != "int" && m_sKeyType != "float") ||
		m_iKeyTypeCount <= 0)
		throw new Exception("error input for create B+ Tree");

	if(m_sKeyType=="char") {							
		Node<char> tmpNode(m_iKeyTypeCount + 1, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetCount(0);
		tmpNode.SetLeaf(1);
		tmpNode.AddNode();
		tmpNode.GetSelfPosition(m_iRoot[0], m_iRoot[1]);
	}
	else if(m_sKeyType=="int") {
		Node<int> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetCount(0);
		tmpNode.SetLeaf(1);
		tmpNode.AddNode();
		tmpNode.GetSelfPosition(m_iRoot[0],m_iRoot[1]);
	}
	else if(m_sKeyType=="float") {
		Node<float> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);
		tmpNode.SetCount(0);
		tmpNode.SetLeaf(1);
		tmpNode.AddNode();
		tmpNode.GetSelfPosition(m_iRoot[0],m_iRoot[1]);
	}
	
	
	m_iFirstLeaf[0]=m_iRoot[0];
	m_iFirstLeaf[1]=m_iRoot[1];

	int Size = 0;
	Size = m_sKeyType.GetLength() + 1 + sizeof(int) * 5;
	char* c = new char[Size];
	char* p = c;
	//_tcscpy_s(c, m_sKeyType);
	p += m_sKeyType.GetLength();
	*p = '\0';
	p++;
	*(int*)p = m_iKeyTypeCount;
	p += sizeof(int);
	((int*)p)[0] = m_iRoot[0];
	((int*)p)[1] = m_iRoot[1];
	p += (2*sizeof(int));
	((int*)p)[0] = m_iFirstLeaf[0];
	((int*)p)[1] = m_iFirstLeaf[1];
	m_file->Write(&m_iTreePosition[0], sizeof(int)*2);
	delete[] c;
}

void BTree::Read()
{	
	if(m_file == NULL)
		throw new Exception("error now store in b+tree");
	if(m_iTreePosition[0] < 0 || m_iTreePosition[1] < 0  )
		throw new Exception("no init position before read b+ tree");

	int Size = 0;
	m_file->Write((void *)&m_iTreePosition[0], sizeof(int));
	char* c = new char[Size];
	char* p = c;
	m_file->Write((void *)&m_iTreePosition[0], sizeof(int));	
	m_sKeyType = p;
	p += m_sKeyType.GetLength() + 1;

	m_iKeyTypeCount = *(int*)p;
	p += sizeof(int);
	m_iRoot[0] = ((int*)p)[0];
	m_iRoot[1] = ((int*)p)[1];
	p += 2 * sizeof(int);
	m_iFirstLeaf[0] = ((int*)p)[0];
	m_iFirstLeaf[1] = ((int*)p)[1];
	delete[] c;
}

//等值查找
bool BTree::FindValue(void* pValue, int& Block, int& Index)
{
	if(m_iRoot[0] < 0 || m_iRoot[1] < 0 || m_file == NULL)
		throw new Exception("no root when find value in B+tree");
	if((m_sKeyType != "char" && m_sKeyType != "int" &&  m_sKeyType != "float") ||
		m_iKeyTypeCount <= 0)
		throw new Exception("error input for find value in b+");
	if(m_sKeyType == "char") {
		Node<char> tmpNode(m_iKeyTypeCount + 1, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0],m_iRoot[1]);
		tmpNode.Read();
		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {	
			FindIndex = tmpNode.FindGreaterKeyIndex((char*)pValue);	
			if(FindIndex == -1)									
				FindIndex = tmpNode.GetCount();					
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);	
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		FindIndex = tmpNode.FindKeyIndex((char*)pValue);		
		if(FindIndex < 0)										
			return false;
		tmpNode.GetPointer(FindIndex, Block, Index);			
	}
	else if(m_sKeyType == "int")
	{
		Node<int> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();
		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((int*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		FindIndex = tmpNode.FindKeyIndex((int*)pValue);
		if(FindIndex < 0)
			return false;
		tmpNode.GetPointer(FindIndex, Block, Index);
	}

	else if(m_sKeyType == "float")
	{
		Node<float> tmpNode(m_iKeyTypeCount, this);
		tmpNode.SetFileOrganize(m_file);	
		tmpNode.SetSelfPosition(m_iRoot[0], m_iRoot[1]);
		tmpNode.Read();
		int tmpBlock, tmpIndex;
		int FindIndex = 0;
		while(!tmpNode.IsLeaf()) {
			FindIndex = tmpNode.FindGreaterKeyIndex((float*)pValue);
			if(FindIndex == -1)
				FindIndex = tmpNode.GetCount();
			tmpNode.GetPointer(FindIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		FindIndex = tmpNode.FindKeyIndex((float*)pValue);
		if(FindIndex < 0)
			return false;
		tmpNode.GetPointer(FindIndex, Block, Index);
	}
	return true;
}
