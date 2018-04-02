#include"basic.h"
#include<iostream>

using namespace std;

//---------------------------AttributeValue-------------------------------
AttributeValue::AttributeValue()
{
	charValue.Empty();
	charCount = 0;
	floatValue = 0;
	intValue = 0;
	typeNo = 0;//0=char,1=int, 2=float
}

AttributeValue::~AttributeValue()
{
	if(typeNo == 0) charValue.Empty();
}


AttributeValue& AttributeValue::operator=(const AttributeValue& a)
{
	switch(a.typeNo){
		case 0:
			typeNo = 0;
			if(charCount!=0) charValue.Empty();
				charCount = a.charCount;
			charValue = new char[charCount];
			int temp;
			for (int i = 0; i < charCount; i++){
				temp = a.charValue[i];
				charValue.SetAt(i, temp);
			}
			break;
		case 1:
			typeNo = 1;
			floatValue = a.floatValue;

			break;
		default:
			typeNo = 2;
			intValue = a.intValue; 
	}
	return *this;
}

AttributeValue::AttributeValue(const AttributeValue& a)
{
	switch(a.typeNo){
		case 0:
			typeNo = 0;
			charValue = new char[a.charCount];
			for(int i = 0; i < a.charCount; i++)
				charValue.SetAt(i,a.charValue[i]);
			charCount = a.charCount;
			intValue = 0;
			floatValue = 0.0;

			break;
		case 1:
			typeNo = 1;
			floatValue = a.floatValue;
			intValue = 0;
			charValue.Empty();
			charCount = 0;

			break;
		default:
			typeNo = 2;
			intValue = a.intValue;
			floatValue = 0.0;
			charValue.Empty();
			charCount = 0;
	}
}

//---------------------------AttributeInformation-------------------------------
AttributeInformation::AttributeInformation()
{
	type.Empty();
	typeCount = 0;
	unique = true;
	attributeName.Empty();
	hasIndex = false;
}

AttributeInformation::AttributeInformation(const AttributeInformation& a)
{
	type = a.type;
	typeCount = a.typeCount;
	unique = a.unique;
	attributeName = a.attributeName;
	hasIndex = a.hasIndex;
}

AttributeInformation& AttributeInformation::operator=(const AttributeInformation& a)
{
	type = a.type;
	typeCount = a.typeCount; 
	unique = a.unique;
	attributeName = a.attributeName;
	hasIndex = a.hasIndex;
	return *this;
}

int AttributeInformation::getSize()
{
	if(type == "int")
	return sizeof(int);
	else if(type == "float")
	return sizeof(float);
	else if(type == "char")
	return typeCount;
}

//---------------------------TableInformation-------------------------------
TableInformation::TableInformation(const TableInformation& t)
{
	attributeCount = t.attributeCount;
	tableName = t.tableName;
	primaryKey = t.primaryKey;
	for(int i = 0; i < attributeCount; i++)
		info[i] = t.info[i];
}

TableInformation& TableInformation::operator=(const TableInformation& t)
{
	attributeCount = t.attributeCount;
	tableName = t.tableName;
	primaryKey = t.primaryKey;
	for(int i = 0; i < attributeCount; i++)
		info[i] = t.info[i];
	return *this;
}

int TableInformation::getTotalSize()
{
	int totalSize = 0;
	for(int i = 0; i < attributeCount; i++)
		totalSize += info[i].getSize();
	return totalSize;
}

//---------------------------Record-------------------------------
Record::Record(const Record& r)
{
	attributeCount = r.attributeCount;
	for (int i = 0; i < r.attributeCount; i++)
		value[i] = r.value[i];
	addr = r.addr;
}

Record& Record::operator=(const Record& r)
{
	attributeCount = r.attributeCount;
	for(int i = 0; i < r.attributeCount; i++)
		value[i] = r.value[i];
	addr = r.addr;
	return *this;
}

//---------------------------DBFile-------------------------------
DBFile::DBFile()
{
	myfile.Open("Date.dat",CFile::modeCreate | CFile::modeNoTruncate| CFile::modeReadWrite);
	if(myfile.GetLength() == 0){//if file is empty(new), then leave the first store the metadata	
		int content[4096/4];//divide into 1024 line, each line is a word wide
		content[0]=3;	//usage of every content[],??????
		content[1]=4096-3*8-1;
		content[2] = content[4] = content[6] = 8;
		content[3] = 4096-8;
		content[5] = 4096-16;
		content[7] = 4096-24;
		for(int i = 8; i < 4096/4; i++)
			content[i] = -1;//initialize the rest of byte
		myfile.Write((void *)content,4096);
	}
}

DBFile::~DBFile()
{
	//close the file
	myfile.Close();
}

int DBFile::allocNewBlock()
{
	unsigned int blockCount = myfile.GetLength()/4096;//set the new block whose size is 4096 Byte
	unsigned int base = blockCount * 4096;
	int content[4096/4];
	content[0]=0;		
	content[1]=4096-1;
	myfile.Seek(base, CFile::begin);//seek(),member file of CFile
	myfile.Write((void *)content,4096);//create a block that has 4096 byte
	return blockCount;
}

int DBFile::findEnoughSpace(int offset, int size)
{
	unsigned int blockCount = myfile.GetLength()/4096;
	int array[2];
	if(offset == 0)
		offset = 1;

	for(int i = offset; i < blockCount; i++){
		myfile.Seek(i*4096,CFile::begin);//seek(),member file of CFile
		myfile.Read((void *)array,8);//read(),member file of CFile
		if(array[1] + 1 - 2*4 - array[0]*8 >= size + 8)//if there are enough space, then return
			return i;
	}
	return allocNewBlock();
}

//---------------------------Compare-------------------------------
bool Compare::compareAll(const AttributeValue& a)
{
	if(a.typeNo != typeNo){
		cout << "The two compare type is not identical!!" << endl;
		return 0;
	}
	else{
		bool compareResult[6];
		CString temp;
		float temp1;
		int temp2;
		switch(a.typeNo){
			case 0:
				temp = a.charValue;
				compareResult[0] = (temp == item2);
				compareResult[1] = (temp < item2);
				compareResult[2] = (temp > item2);
				compareResult[3] = (temp <= item2);
				compareResult[4] = (temp >= item2);
				compareResult[5] = (temp != item2);

				break;
			case 1:
				temp1 = a.floatValue;
				compareResult[0] = (temp1 == (float)atof(item2));
				compareResult[1] = (temp1 < (float)atof(item2));
				compareResult[2] = (temp1 > (float)atof(item2));
				compareResult[3] = (temp1 <= (float)atof(item2));
				compareResult[4] = (temp1 >= (float)atof(item2));
				compareResult[5] = (temp1 != (float)atof(item2));

				break;
			case 2:
				temp2 = a.intValue;
				compareResult[0] = (temp2 == atoi(item2));
				compareResult[1] = (temp2 < atoi(item2));
				compareResult[2] = (temp2 > atoi(item2));
				compareResult[3] = (temp2 <= atoi(item2));
				compareResult[4] = (temp2 >= atoi(item2));
				compareResult[5] = (temp2 != atoi(item2));

				break;
	}

	return compareResult[operation];
	}
}
