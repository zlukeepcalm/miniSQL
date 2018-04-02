#define _AFXDLL

#ifndef _INDEX_MANAGER_H
#define _INDEX_MANAGER_H
#define FANOUT 3

class BTree;
#include"basic.h"

template <class TKeyType>
class Node
{
	bool m_bLeaf;
	int m_iCount;
	TKeyType* m_Key[FANOUT-1];
	int m_iPointer[FANOUT][2];
	int m_iFatherPosition[2];
	int m_iSelfPosition[2];
	int m_iKeyTypeCount;
	CFile* m_file;
	BTree* m_BTree;
public:
	Node(int TypeCount, BTree* Tree);
	~Node();
	bool IsLeaf(){return m_bLeaf;}	
	int GetCount(){return m_iCount;}
	TKeyType* GetKey(int Index);
	void SetKey(int Index, TKeyType* pKey);	

	void GetPointer(int Index, int& PositionBlock, int& PositionIndex);
	void GetFatherPosition(int& Block, int& Index)
	{
		Block = m_iFatherPosition[0];
		Index = m_iFatherPosition[1];
	}
	void GetSelfPosition(int& Block, int& Index)
	{
		Block = m_iSelfPosition[0];
		Index = m_iSelfPosition[1];	
	}
	void GetFrontNodePosition(int& Block, int& Index);
	bool GetEqualLevelNodePosition(int& Block, int& Index);
	int GetFatherLinkToMeIndex();
	int FindGreaterKeyIndex(TKeyType* pValue);
	int FindKeyIndex(TKeyType* pValue);


	void SetLeaf(bool Leaf){m_bLeaf = Leaf;};
	void SetCount(int count){m_iCount = count;}	
	void SetFileOrganize(CFile* file){m_file = file;}	
	void SetPointer(int Index,int PositionBlock, int PositionIndex);
	void SetFatherPosition(int Block, int Index)
	{
		m_iFatherPosition[0] = Block;	
		m_iFatherPosition[1] = Index;
	}
	void SetSelfPosition(int Block, int Index)
	{
		m_iSelfPosition[0] = Block;	
		m_iSelfPosition[1] = Index;	
	}
	void Read();							
	char* OrganizeNode();					

	void InsertKey(TKeyType* pValue, int Block, int Index, bool AutoSave=0);
	void AddNode();								
	void DeleteKey(int Index,bool AutoSave=1);								
	void FreeNode();									
	void Drop();														
	void UpdateNode();														
	void ChangeValue(TKeyType* pSrcValue, TKeyType* pNowValue);	
};


class BTreeReader: public CObject
{
	int m_iFindIndex;
	int m_iBFindIndex;
	void* m_pNode;
public:
	int GetFindIndex(){return m_iFindIndex;}
	void SetFindIndex(int Index){m_iFindIndex = Index;}
	int GetBFindIndex(){return m_iBFindIndex;}
	void SetBFindIndex(int Index){m_iBFindIndex = Index;}
	void* GetNode(){return m_pNode;}
	void SetNode(void* Node){m_pNode = Node;}
};

template <class TKeyType>
Node<TKeyType>::Node(int TypeCount, BTree* Tree)	
{
	m_iCount = -1;
	m_bLeaf = 1;
	m_file = NULL;
	int i;
	for(i = 0; i < FANOUT; ++i) {
		m_iPointer[i][0] = -1;
		m_iPointer[i][1] = -1;
	}

	m_iFatherPosition[0] = -1;
	m_iSelfPosition[0] = -1;

	m_iFatherPosition[1] = -1;
	m_iSelfPosition[1] = -1;

	m_iKeyTypeCount = TypeCount;
	m_BTree = Tree;

	for(int j = 0; i < FANOUT - 1; ++j)
		m_Key[j] = new TKeyType[m_iKeyTypeCount];
}

template <class TKeyType>
Node<TKeyType>::~Node()		
{
	for(int i = 0; i < FANOUT - 1; ++i)
		delete[] m_Key[i];
}

template <class TKeyType>
void Node<TKeyType>::GetPointer(int Index, int& PositionBlock, int& PositionIndex)
{
	if(Index < 0 || Index > FANOUT - 1) {
		PositionBlock = -1;
		PositionIndex = -1;
	}
	PositionBlock = m_iPointer[Index][0];
	PositionIndex = m_iPointer[Index][1];
}

template <class TKeyType>
void Node<TKeyType>::SetPointer(int Index,int PositionBlock,int PositionIndex)
{
	if(Index < 0 || Index > FANOUT - 1)	
	{
		PositionBlock = -1;
		PositionIndex = -1;
	}
	m_iPointer[Index][0] = PositionBlock;
	m_iPointer[Index][1] = PositionIndex;
}

template <class TKeyType>
TKeyType* Node<TKeyType>::GetKey(int Index)
{
	if(Index < 0 || Index > FANOUT - 2)
		throw new Exception("error when getting search key in node");
	return m_Key[Index];
}

template <class TKeyType>
void Node<TKeyType>::SetKey(int Index, TKeyType* pKey)
{
	if(Index < 0 || Index > FANOUT - 2)
		throw new Exception("error when setting search key in node");
	for(int i = 0; i < m_iKeyTypeCount; ++i)
		m_Key[Index][i] = pKey[i];
}

template <class TKeyType>
void Node<TKeyType>::Read()	
{
	if(m_file == NULL)							
		throw new Exception("init node file !");	
	int Block = m_iSelfPosition[0], Index = m_iSelfPosition[1];	
	if(Block < 0 || Index < 0)						
		throw new Exception("No this position node!");
	int Size = Block;
	m_file->Write((void *)&Block, sizeof(int));		
	char* pNodeLink = new char[Size];
	m_file->Write((void *)&Block, sizeof(int));			
	char* p = pNodeLink;
	m_bLeaf = ((*p) < 0);								
	m_iCount = ((*p) & 127);							
	p += sizeof(char);									
	for(int i = 0; i < FANOUT - 1; ++i) {			
		m_iPointer[i][0] = ((int*)p)[0];
		m_iPointer[i][1] = ((int*)p)[1];
		p += sizeof(int) * 2;
		for(int j = 0; j < m_iKeyTypeCount; ++j) {	
			*(m_Key[i] + j * sizeof(TKeyType)) = (*(TKeyType*)p);
			p += sizeof(TKeyType);
		}
	}
	m_iPointer[FANOUT-1][0] = ((int*)p)[0];			
	m_iPointer[FANOUT-1][1] = ((int*)p)[1];
	p += sizeof(int) * 2;
	m_iFatherPosition[0] = ((int*)p)[0];			
	m_iFatherPosition[1] = ((int*)p)[1];
	delete[] pNodeLink;
}

template <class TKeyType>
char* Node<TKeyType>::OrganizeNode()			
{
	if(m_iCount < 0)							
		return NULL;
	int Size = sizeof(TKeyType) * m_iKeyTypeCount * (FANOUT - 1)	
		+ sizeof(int) * 2 * (FANOUT + 1) + 1;
	char* pNodeStore = new char[Size];				
	char* p = pNodeStore;
	if(m_bLeaf)										
		*p = (char)(128 | (m_iCount << 25 >> 25));	
													
	else											
		*p = (char)(m_iCount << 25 >> 25);			
	p += sizeof(char);								
	for(int i = 0; i < FANOUT - 1; ++i) {			
		((int*)p)[0] = m_iPointer[i][0];
		((int*)p)[1] = m_iPointer[i][1];
		p += sizeof(int) * 2;
		for(int j = 0; j < m_iKeyTypeCount; ++j) {		
			(*(TKeyType*)p) = *(m_Key[i] + j * sizeof(TKeyType)); 
			p += sizeof(TKeyType);
		}
	}
	((int*)p)[0] = m_iPointer[FANOUT-1][0];			
	((int*)p)[1] = m_iPointer[FANOUT-1][1];
	p += sizeof(int) * 2;
	((int*)p)[0] = m_iFatherPosition[0];			
	((int*)p)[1] = m_iFatherPosition[1];
	
	return pNodeStore;
}

//插入、删除、修改

template <class TKeyType>
void Node<TKeyType>::InsertKey(TKeyType* pValue, int Block, int Index, bool AutoSave)	
{
	if(pValue == NULL)									
		throw new Exception("error input in insertKey");	
	if((m_file == NULL && AutoSave) || m_iCount < 0)		
		throw new Exception("init file before insertKey");	
	if(IsLeaf()) {											
		if(m_iCount < FANOUT - 1) {							
			int KIndex = FindKeyIndex(pValue);			
			if(KIndex >= 0)								
				throw new Exception("Key Existed");;		
			KIndex = FindGreaterKeyIndex(pValue);			
			if(KIndex < 0)									
				KIndex = m_iCount;						
			for(int i = m_iCount; i > KIndex; --i) {		
				SetKey(i, GetKey(i - 1));					
				int PBlock, PIndex;
				GetPointer(i - 1, PBlock, PIndex);
				SetPointer(i, PBlock, PIndex);
			}
			SetKey(KIndex, pValue);							
			SetPointer(KIndex, Block, Index);				
			++m_iCount;										

			int FBlock, FIndex;
			GetFatherPosition(FBlock, FIndex);				
			if(FBlock >= 0 && FIndex >= 0 && KIndex == 0)	{		
				Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
				FatherNode.SetSelfPosition(FBlock, FIndex);			
				FatherNode.SetFileOrganize(m_file);				
				FatherNode.Read();									
				FatherNode.ChangeValue(GetKey(1), GetKey(0));		
			}
			if(AutoSave)									
				UpdateNode();							
		}
		else {													
			Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree);	
			tmpNode.SetLeaf(1);									
			tmpNode.SetCount(0);							
			tmpNode.SetFileOrganize(m_file);					
			tmpNode.AddNode();									
			int KBlock, KIndex;
			GetPointer(FANOUT - 1, KBlock, KIndex);				
			tmpNode.SetPointer(FANOUT - 1, KBlock, KIndex);		
			GetPointer(FANOUT - 2, KBlock, KIndex);			
			tmpNode.InsertKey(GetKey(FANOUT - 2), KBlock, KIndex, 0);	
			DeleteKey(FANOUT - 2, 0);							
			InsertKey(pValue, Block, Index, 0);				
			for(int i = FANOUT / 2; i < FANOUT - 1; ++i) {	
				GetPointer(i, KBlock, KIndex);
				tmpNode.InsertKey(GetKey(i), KBlock, KIndex, 0);
				DeleteKey(i, 0);
			}
			int FBlock, FIndex;
			GetFatherPosition(FBlock, FIndex);				
			
			Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
			FatherNode.SetFileOrganize(m_file);				
			if(FBlock == -1 || FIndex == -1) {					
				FatherNode.SetLeaf(0);							
				FatherNode.SetCount(0);							
 				FatherNode.AddNode();						
				GetSelfPosition(KBlock, KIndex);			
				FatherNode.SetPointer(0, KBlock, KIndex);		
				FatherNode.GetSelfPosition(FBlock, FIndex);		
				m_BTree->SetRoot(FBlock, FIndex);			
				m_BTree->UpdateBPlusTree();					
			}
			else {												
				FatherNode.SetSelfPosition(FBlock, FIndex);		
				FatherNode.Read();							
			}
			tmpNode.SetFatherPosition(FBlock, FIndex);		
			SetFatherPosition(FBlock, FIndex);				
			tmpNode.UpdateNode();							
			tmpNode.GetSelfPosition(KBlock, KIndex);		
			SetPointer(FANOUT - 1, KBlock, KIndex);			
															
			if(AutoSave)									
				UpdateNode();								
			FatherNode.InsertKey(tmpNode.GetKey(0), KBlock, KIndex);
		}
	}
	else {														
		if(m_iCount < FANOUT - 1) {							
			int KIndex = FindKeyIndex(pValue);					
			if(KIndex >= 0)										
				throw new Exception("Key Existed");;		
			KIndex = FindGreaterKeyIndex(pValue);			
			if(KIndex < 0)									
				KIndex = m_iCount;							
			for(int i = m_iCount; i > KIndex; --i) {		
				SetKey(i, GetKey(i - 1));
				int PBlock, PIndex;
				GetPointer(i, PBlock, PIndex);
				SetPointer(i + 1, PBlock, PIndex);
			}
			SetKey(KIndex, pValue);							
			SetPointer(KIndex + 1, Block, Index);			
			++m_iCount;										

			Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);	
			tmpNode.SetSelfPosition(Block, Index);			
			tmpNode.SetFileOrganize(m_file);				
			tmpNode.Read();								
			int tmpBlock, tmpIndex;
			GetSelfPosition(tmpBlock, tmpIndex);			
			tmpNode.SetFatherPosition(tmpBlock, tmpIndex);	
			tmpNode.UpdateNode();							

			if(AutoSave)										
				UpdateNode();									
		}
		else {													
			Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree);	
			tmpNode.SetLeaf(0);									
			tmpNode.SetCount(0);								
			tmpNode.SetFileOrganize(m_file);					
			tmpNode.AddNode();									
			
			int KBlock, KIndex;

			GetPointer(FANOUT - 1, KBlock, KIndex);			
			tmpNode.InsertKey(GetKey(FANOUT - 2), KBlock, KIndex, 0);	
			DeleteKey(FANOUT - 2, 0);							
			InsertKey(pValue, Block, Index, 0);				
			for(int i = FANOUT / 2; i < FANOUT - 1; ++i) {		
				GetPointer(i + 1, KBlock, KIndex);
				tmpNode.InsertKey(GetKey(i), KBlock, KIndex, 0);
				DeleteKey(i);
			}
			int FBlock, FIndex;
			GetFatherPosition(FBlock, FIndex);					
			Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
			if(FBlock == -1 || FIndex == -1) {					
				FatherNode.SetLeaf(0);
				FatherNode.SetCount(0);
				FatherNode.SetFileOrganize(m_file);
				FatherNode.AddNode();
				GetSelfPosition(KBlock, KIndex);
				FatherNode.SetPointer(0, KBlock, KIndex);		
				FatherNode.GetSelfPosition(FBlock, FIndex);		
				m_BTree->SetRoot(FBlock, FIndex);
				m_BTree->UpdateBPlusTree();
			}
			else {												
				FatherNode.SetSelfPosition(FBlock, FIndex);
				FatherNode.SetFileOrganize(m_file);
				FatherNode.Read();								
			}
			tmpNode.SetFatherPosition(FBlock, FIndex);			
			SetFatherPosition(FBlock, FIndex);					
			tmpNode.GetSelfPosition(KBlock, KIndex);
			if(AutoSave)
				UpdateNode();
			int tmpBlock, tmpIndex;
			FatherNode.InsertKey(tmpNode.GetKey(0), KBlock, KIndex);		
			tmpNode.GetPointer(1, tmpBlock, tmpIndex);					
			tmpNode.SetPointer(0, tmpBlock, tmpIndex);
			tmpNode.DeleteKey(0);
			tmpNode.GetSelfPosition(FBlock, FIndex);
			for(int m = 0; m <= tmpNode.GetCount(); ++m) {				
				tmpNode.GetPointer(m, tmpBlock, tmpIndex);
				FatherNode.SetSelfPosition(tmpBlock, tmpIndex);
				FatherNode.Read();
				FatherNode.SetFatherPosition(FBlock, FIndex);
				FatherNode.UpdateNode();
			}
		}
	}
}

template <class TKeyType>
void Node<TKeyType>::DeleteKey(int Index, bool AutoSave)	
{
	if(Index >= m_iCount || Index < 0)
		throw new Exception("error input in deleteKey");
	if((m_file == NULL && AutoSave) || m_iCount < 0)
		throw new Exception("init file before insertKey");	
	
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);
	tmpNode.SetFileOrganize(m_file);
	int tmpBlock, tmpIndex;
	if(IsLeaf()) {												
		if(m_iCount - 1 == 0) {									
			GetFatherPosition(tmpBlock, tmpIndex);
			if(tmpBlock >= 0 && tmpIndex >= 0)				
				Drop();										
			else {											
				--m_iCount;									
				if(AutoSave)
					UpdateNode();
			}
			return;
		}
		if(Index == 0) {										
			GetFatherPosition(tmpBlock, tmpIndex);
			if(tmpBlock >= 0 && tmpIndex >= 0) {				
				tmpNode.SetSelfPosition(tmpBlock, tmpIndex);	
				tmpNode.Read();
				tmpNode.ChangeValue(GetKey(0), GetKey(1));
			}
		}
		for(int i = Index; i < m_iCount - 1; --i) {			
			SetKey(i, GetKey(i + 1));
			GetPointer(i + 1, tmpBlock, tmpIndex);
			SetPointer(i, tmpBlock, tmpIndex);
		}
		--m_iCount;
		if(m_iCount < FANOUT / 2) {								
			int GetKeyIndex = 0;
			GetPointer(FANOUT - 1, tmpBlock, tmpIndex);			
			if(tmpBlock < 0 || tmpIndex < 0) {					
				GetKeyIndex = -1;
				GetFrontNodePosition(tmpBlock, tmpIndex);		
			}
			if(tmpBlock < 0 || tmpIndex < 0) {				
				if(AutoSave)
					UpdateNode();
				return;
			}
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);		
			tmpNode.Read();
			if(tmpNode.GetCount() + m_iCount > FANOUT - 1) {
				if(GetKeyIndex == -1)						
					GetKeyIndex = tmpNode.GetCount() - 1;
				tmpNode.GetPointer(GetKeyIndex, tmpBlock, tmpIndex);	
				InsertKey(tmpNode.GetKey(GetKeyIndex), tmpBlock, tmpIndex);	
				tmpNode.DeleteKey(GetKeyIndex);							
				return;
			}
			if(GetKeyIndex == -1) {								
				for(int i = 0; i < m_iCount - 1; ++i) {		
					GetPointer(i, tmpBlock, tmpIndex);
					tmpNode.InsertKey(GetKey(i), tmpBlock, tmpIndex, 0);
				}
				tmpNode.UpdateNode();
				Drop();										
				return;
			}
			else {															
				for(int i = 0; i < tmpNode.GetCount() - 1; ++i) {			
					tmpNode.GetPointer(i, tmpBlock, tmpIndex);
					InsertKey(tmpNode.GetKey(i), tmpBlock, tmpIndex, 0);
				}
				if(AutoSave)
					UpdateNode();
				tmpNode.Drop();								
				return;
			}
		}
		if(AutoSave)
			UpdateNode();
	}
	else {													
		if(m_iCount - 1 == 0) {								
			--m_iCount;
			if(AutoSave)
				UpdateNode();
			Drop();
			return;
		}
		for(int i = Index; i < m_iCount - 1; ++i) {		
			SetKey(i, GetKey(i + 1));
			GetPointer(i + 2, tmpBlock, tmpIndex);
			SetPointer(i + 1, tmpBlock, tmpIndex);
		}
		--m_iCount;
		if(AutoSave)
			UpdateNode();
	}
}

template <class TKeyType>
void Node<TKeyType>::AddNode()									
{
	if(m_file == NULL)											
		throw new Exception("init file in node");			
	int Size = sizeof(TKeyType) * m_iKeyTypeCount * (FANOUT - 1) +		
			sizeof(int) * 2 * (FANOUT + 1) + 1;
	char* c = OrganizeNode();
	if(c == NULL)												
		throw new Exception("cant organize node");				
	int block, index = 0;
	m_file->Write((void*)c, Size);				
	//SetSelfPosition(block, index);
	delete[] c;
}

template <class TKeyType>
void Node<TKeyType>::FreeNode()									
{
	if(m_file == NULL)											
		throw new Exception("init file in node before drop");	
	int Block, Index;
	GetSelfPosition(Block, Index);								
	m_file->Write((void *)&Block, sizeof(int));							
}

template <class TKeyType>
bool Node<TKeyType>::GetEqualLevelNodePosition(int& Block, int& Index)	
{
	Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree);
	tmpNode.SetFileOrganize(m_file);
	int tmpBlock, tmpIndex;
	GetFatherPosition(tmpBlock, tmpIndex);				
	if(tmpBlock < 0 || tmpIndex < 0) {					
		Block = -1;
		Index = -1;
		return false;
	}
	int LinkMe = GetFatherLinkToMeIndex();				
	if(LinkMe == 0) {									
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
		tmpNode.Read();
		tmpNode.GetPointer(1, Block, Index);
		return false;
	}
	else if(LinkMe > 0) {								
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
		tmpNode.Read();
		tmpNode.GetPointer(LinkMe - 1, Block, Index);
		return true;
	}
	else 
		throw new Exception("get error link me in get equal level node");
}

template <class TKeyType>
int Node<TKeyType>::GetFatherLinkToMeIndex()						
{
	int tmpBlock, tmpIndex;
	int MeBlock, MeIndex;

	GetSelfPosition(MeBlock, MeIndex);
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);
	tmpNode.SetFileOrganize(m_file);
	GetFatherPosition(tmpBlock, tmpIndex);						
	
	if(tmpBlock < 0 || tmpIndex < 0)
		throw new Exception("No father to get father link to me");
	tmpNode.SetSelfPosition(tmpBlock, tmpIndex);					
	tmpNode.Read();
	
	for(int i = 0; i <= tmpNode.GetCount(); ++i) {				
		int FBlock, FIndex;
		tmpNode.GetPointer(i, FBlock, FIndex);						
		if(FBlock == MeBlock && FIndex == MeIndex)				
			return i;
	}
	throw new Exception("Father no link to me");
}

template <class TKeyType>
void Node<TKeyType>::Drop()										
{
	int tmpBlock, tmpIndex;
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);
	tmpNode.SetFileOrganize(m_file);
	if(IsLeaf()) {												
		GetFrontNodePosition(tmpBlock, tmpIndex);
		if(tmpBlock < 0 || tmpIndex < 0) {					
			GetPointer(FANOUT - 1, tmpBlock, tmpIndex);			
			m_BTree->SetFirstLeaf(tmpBlock, tmpIndex);
			m_BTree->UpdateBPlusTree();
			GetFatherPosition(tmpBlock, tmpIndex);			
			if(tmpBlock < 0 || tmpIndex < 0)
				throw new Exception("Drop root node error");

			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);	
			tmpNode.Read();
			tmpNode.GetPointer(1, tmpBlock, tmpIndex);
			tmpNode.SetPointer(0, tmpBlock, tmpIndex);
			tmpNode.DeleteKey(0);							
			FreeNode();
			return;
		}
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);			
		tmpNode.Read();
		GetPointer(FANOUT - 1, tmpBlock, tmpIndex);				
		tmpNode.SetPointer(FANOUT - 1, tmpBlock, tmpIndex);
		tmpNode.UpdateNode();

		GetFatherPosition(tmpBlock, tmpIndex);
		if(tmpBlock < 0 || tmpIndex < 0)
			throw new Exception("Drop node error");
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);			
		tmpNode.Read();
		int ValueIndex = tmpNode.FindKeyIndex(GetKey(0));		
		if(ValueIndex >= 0)									
			tmpNode.DeleteKey(ValueIndex);
		else {												
			tmpNode.GetPointer(1, tmpBlock, tmpIndex);		
			tmpNode.SetPointer(0, tmpBlock, tmpIndex);
			tmpNode.ChangeValue(GetKey(0), tmpNode.GetKey(0));
			tmpNode.DeleteKey(0);
		}
		FreeNode();
		return;
	}
	GetFatherPosition(tmpBlock, tmpIndex);
	if(tmpBlock < 0 || tmpIndex < 0) {						
		GetPointer(0, tmpBlock, tmpIndex);
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
		tmpNode.Read();
		tmpNode.SetFatherPosition(-1, -1);
		tmpNode.UpdateNode();
		m_BTree->SetRoot(tmpBlock, tmpIndex);
		m_BTree->UpdateBPlusTree();
		FreeNode();
		return ;
	}
	int ELevelBlock, ELevelIndex;
	bool ENodeFront = GetEqualLevelNodePosition(ELevelBlock, ELevelIndex);
	GetPointer(0, tmpBlock, tmpIndex);
	Node<TKeyType> ChildNode(m_iKeyTypeCount, m_BTree);	
	ChildNode.SetFileOrganize(m_file);
	ChildNode.SetSelfPosition(tmpBlock, tmpIndex);
	ChildNode.Read();
	TKeyType* InsertV = new TKeyType[m_iKeyTypeCount];
	while(1) {											
		tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
		tmpNode.Read();
		if(tmpNode.IsLeaf())
			break;
		tmpNode.GetPointer(0, tmpBlock, tmpIndex);
	}
	TKeyType* p = tmpNode.GetKey(0);
	for(int i = 0; i < m_iKeyTypeCount; ++i)
		InsertV[i] = p[i];
	int InsertBlock, InsertIndex;
	ChildNode.GetSelfPosition(InsertBlock, InsertIndex);

	int FLMIndex = GetFatherLinkToMeIndex();
	GetFatherPosition(tmpBlock, tmpIndex);
	tmpNode.SetSelfPosition(tmpBlock, tmpIndex);			
	tmpNode.Read();
	if(FLMIndex == 0) {									
		tmpNode.GetPointer(1, tmpBlock, tmpIndex);		
		tmpNode.SetPointer(0, tmpBlock, tmpIndex);
		tmpNode.DeleteKey(0);
	}
	else {
		tmpNode.DeleteKey(FLMIndex-1);
	}
	if(!ENodeFront) {									
		TKeyType* ChangeV = new TKeyType[m_iKeyTypeCount];
		tmpNode.SetSelfPosition(ELevelBlock, ELevelIndex);
		tmpNode.Read();
		tmpNode.GetPointer(0, InsertBlock, InsertIndex);
		while(!(tmpNode.IsLeaf())) {
			tmpNode.GetPointer(0,tmpBlock,tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock,tmpIndex);
			tmpNode.Read();
		}
		int i;
		for(i = 0; i < m_iKeyTypeCount; ++i)
			ChangeV[i] = InsertV[i];
		p = tmpNode.GetKey(0);
		for(i = 0; i < m_iKeyTypeCount; ++i)
			InsertV[i] = p[i];
		tmpNode.SetSelfPosition(ELevelBlock, ELevelIndex);
		tmpNode.Read();
		tmpNode.GetSelfPosition(tmpBlock, tmpIndex);
		ChildNode.SetFatherPosition(tmpBlock, tmpIndex);
		ChildNode.UpdateNode();
		ChildNode.GetSelfPosition(tmpBlock, tmpIndex);
		tmpNode.SetPointer(0,tmpBlock, tmpIndex);		
		tmpNode.ChangeValue(InsertV, ChangeV);
		tmpNode.InsertKey(InsertV, InsertBlock, InsertIndex);
		FreeNode();
		delete[] ChangeV;
		delete[] InsertV;
	}
	else {
		tmpNode.SetSelfPosition(ELevelBlock, ELevelIndex);
		tmpNode.Read();
		tmpNode.GetSelfPosition(tmpBlock, tmpIndex);
		ChildNode.SetFatherPosition(tmpBlock, tmpIndex);
		ChildNode.UpdateNode();
		tmpNode.InsertKey(InsertV, InsertBlock, InsertIndex);
		FreeNode();
		delete[] InsertV;
	}
}

template <class TKeyType>
void Node<TKeyType>::UpdateNode()						
{
	if(m_file == NULL || m_iSelfPosition[0] < 0 || m_iSelfPosition[1] < 0)	
		throw new Exception("init file in node");			
	int Size = sizeof(TKeyType) * m_iKeyTypeCount * (FANOUT-1) +
			sizeof(int) * 2 * (FANOUT+1) + 1;			
	char* c = OrganizeNode();							
	int Block = m_iSelfPosition[0], Index = m_iSelfPosition[1];	
	m_file->Write((void*)c,Size);	
	delete[] c;												
}

template <class TKeyType>
int Node<TKeyType>::FindGreaterKeyIndex(TKeyType* pValue)
{
	if(pValue==NULL)
		return -1;
	int i;
	for(i = 0; i < GetCount(); ++i) {						
		int j;
		for(j = 0; j < m_iKeyTypeCount; ++j) {			
			if(m_Key[i][j] > pValue[j])					
				return i;								
			else if(m_Key[i][j] == pValue[j])			
				continue;								
			else if(m_Key[i][j] < pValue[j])			
				break;							
		}
	}
	return -1;
}

template <class TKeyType>
int Node<TKeyType>::FindKeyIndex(TKeyType* pValue)			
{
	if(pValue==NULL)
		return -1;
	int i;
	for(i = 0; i < GetCount(); ++i) {					
		int j;
		for(j = 0; j < m_iKeyTypeCount; ++j) {			
			if(m_Key[i][j] == pValue[j])				
				continue;								
			else										
				break;
		}
		if(j == m_iKeyTypeCount)						
			return i;								
	}
	return -1;											
}

template <class TKeyType>
void Node<TKeyType>::GetFrontNodePosition(int& Block, int& Index)
{
	if(!IsLeaf())
		throw new Exception("No front node of none leaf");
	int tmpBlock, tmpIndex;
	m_BTree->GetRoot(tmpBlock, tmpIndex);
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);
	tmpNode.SetFileOrganize(m_file);
	tmpNode.SetSelfPosition(tmpBlock, tmpIndex);	
	tmpNode.Read();
	while(!tmpNode.IsLeaf()) {									
		int GIndex = tmpNode.FindKeyIndex(GetKey(0));			
		if(GIndex < 0)											
			GIndex = tmpNode.FindGreaterKeyIndex(GetKey(0));	
		else {													
			tmpNode.GetPointer(GIndex, tmpBlock, tmpIndex);		
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
			while(!tmpNode.IsLeaf()) {						
				GIndex = tmpNode.GetCount();
				tmpNode.GetPointer(GIndex, tmpBlock, tmpIndex);
				tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
				tmpNode.Read();
			}
			tmpNode.GetSelfPosition(Block, Index);			
			return;
		}
		if(GIndex > 0 || GIndex == 0) {						
			tmpNode.GetPointer(GIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
		/*else if(GIndex == 0) {
			tmpNode.GetPointer(GIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}*/
		else {													
			GIndex = tmpNode.GetCount();			
			tmpNode.GetPointer(GIndex, tmpBlock, tmpIndex);
			tmpNode.SetSelfPosition(tmpBlock, tmpIndex);
			tmpNode.Read();
		}
	}
	Block = -1;
	Index = -1;
}

template <class TKeyType>
void Node<TKeyType>::ChangeValue(TKeyType* pSrcValue,TKeyType* pNowValue)
{
	if(pSrcValue == NULL || pNowValue == NULL)							
		throw new Exception("error input in change value in B+");		
	if(IsLeaf())														
		throw new Exception("error use change function");				
	int Index = FindKeyIndex(pSrcValue);								
	if(Index >= 0) {											
		SetKey(Index, pNowValue);							
		UpdateNode();										
	}
	else {														
		int FBlock, FIndex;
		GetFatherPosition(FBlock, FIndex);
		if(FBlock < 0 || FIndex < 0)
			return;
		Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);			
		FatherNode.SetSelfPosition(FBlock, FIndex);						
		FatherNode.SetFileOrganize(m_file);							
		FatherNode.Read();											
		FatherNode.ChangeValue(pSrcValue, pNowValue);				
	}
}

//树类
class BTree : public CObject
{
	CString m_sKeyType;						
	int m_iKeyTypeCount;					
	int m_iRoot[2];							
	int m_iFirstLeaf[2];					
	int m_iTreePosition[2];					
	CObArray m_aReaders;
	CFile* m_file;				
public:
	BTree();
	~BTree();
	void CreateBPlusTree(CString KeyType, int KeyTypeCount);

	void GetRoot(int &Block, int& Index)
	{
		Block = m_iRoot[0];
		Index = m_iRoot[1];
	}

	void GetFirstLeaf(int &Block, int& Index)
	{
		Block = m_iFirstLeaf[0];
		Index = m_iFirstLeaf[1];
	}

	void GetSelfPosition(int &Block, int& Index)
	{
		Block = m_iTreePosition[0];
		Index = m_iTreePosition[1];
	}

	bool FindValue(void* pValue, int& Block, int& Index);

	bool FindValueBigger(void* pValue, Record& Record);
	bool FindValueSmaller(void* pValue, Record& Record);
	bool FindValueBetween(void* pBValue, void* pSValue, Record& Record);

	void SetFileOrganize(CFile* file)
	{
		m_file = file;
	}

	void SetRoot(int Block,int Index)
	{
		m_iRoot[0] = Block;
		m_iRoot[1] = Index;
	}

	void SetFirstLeaf(int Block, int Index)
	{
		m_iFirstLeaf[0] = Block;
		m_iFirstLeaf[1] = Index;
	}

	void SetSelfPosition(int Block, int Index)
	{
		m_iTreePosition[0] = Block;
		m_iTreePosition[1] = Index;
	}

	void Read();

	void InsertValue(void* pValue, int Block, int Index);
	void DeleteValue(void* pValue);
	void DropNode(int Block, int Index);
	void Drop();
	void UpdateBPlusTree();
};

class Exception
{
public:
	CString m_str;
	Exception(CString a = "")
	{
		m_str = a;
	}
};

#endif