#pragma warning(disable:4996)

#include"interpreter.h"
#include"CatalogManager.h"
#include"RecordManager.h"
#include"bufferManager.h"
#include<iostream>
#include<iomanip>
#include <stdlib.h>
#include<string.h>
using namespace std;

#include<afxwin.h>

void PrintCString(CString a)
{
	for (int i = 0; i<a.GetLength(); i++)
		cout << a.GetAt(i);
	cout << endl;
}

interpreter::interpreter()			//zll：初始化
{
	cm = new CatalogManager;
	rm = new RecordManager;
	bm = new BufferManager;
	cm->setbufferManager(bm);		//zll:rm、cm都要指定此bm
	rm->setbufferManager(bm);
}

interpreter::~interpreter()
{
	delete cm;
	delete rm;
	delete bm;
}

bool interpreter::InitCommand(CString &cmd)
{
	int i, j, k, l;
	CString stack;
	stack.Empty();
	for (i = 0; i < cmd.GetLength(); i++)
	{
		if (cmd[i] == '(')
			stack.Insert(stack.GetLength(), '(');
		else if (cmd[i] == ')')
		{
			if (stack.GetLength() > 0 && stack[stack.GetLength() - 1] == '(')
				stack.Delete(stack.GetLength() - 1, 1);
			else
			{
				cout << "右括号不匹配!" << endl;
				return false;
			}
		}
	}
	if (stack.GetLength() != 0)
	{
		cout << "左括号不匹配!" << endl;
		return false;
	}
	for (i = 0, j = 0; i<cmd.GetLength(); i++)
	{
		if (cmd[i] == '\'')
			j++;
	}
	if (j % 2 == 1)
	{
		cout << "单引号不匹配!" << endl;
		return false;
	}
	cmd.Replace('\t', ' ');
	cmd.Replace('\n', ' ');
	cmd.Replace('\r', ' ');
	for (; cmd.GetLength() > 0 && cmd[0] == ' ';)
		cmd.Delete(0, 1);
	if (cmd.GetLength() == 0)
	{
		cout << "语句为空!" << endl;
		return false;
	}
	for (; cmd[cmd.GetLength() - 1] == ' ';)
		cmd.Delete(cmd.GetLength() - 1, 1);
	if (cmd[cmd.GetLength() - 1] != ';')
	{
		cout << "表达式应以分号结尾!" << endl;
		return false;
	}
	for (i = 0, j = 0; i<cmd.GetLength(); i++)
	{
		if (cmd[i] == '\'')
			j++;
		if (j % 2 == 0 && cmd[i] == ' ' && cmd[i + 1] == ' ')
		{
			cmd.Delete(i + 1, 1);
			i--;
		}
	}
	for (i = 0, j = 0, k = 0, l = 0; i<cmd.GetLength(); i++)
	{
		if (cmd[i] == '\'')
			j++;
		if (cmd[i] == '(')
			k++;
		if (cmd[i] == ')')
			l++;
		if (k == 1 && j % 2 == 0 && cmd[i] == '(')
		{
			if (cmd[i - 1] != ' ')
			{
				cmd.Insert(i, ' ');
				i++;
			}
			if (cmd[i + 1] == ' ')
				cmd.Delete(i + 1, 1);
		}
		if (k == l && j % 2 == 0 && cmd[i] == ')')
			if (cmd[i - 1] == ' ')
				cmd.Delete(i - 1, 1);
		if (cmd[i] == ',')
		{
			if (cmd[i - 1] == ' ')
			{
				cmd.Delete(i - 1, 1);
				i--;
			}
			if (cmd[i + 1] == ' ')
				cmd.Delete(i + 1, 1);
		}
		if (j % 2 == 0 && cmd[i] == ';')
			if (cmd[i - 1] == ' ')
			{
			cmd.Delete(i - 1, 1);
			i--;
			}
		if (j % 2 == 0 && cmd[i] == '!')
		{
			cmd.Insert(i, ' ');
			i++;
		}
		if (j % 2 == 0 && (cmd[i] == '<' || cmd[i] == '>' || cmd[i] == '='))
		{
			if (i == 0)
			{
				cout << "表达式错误!" << endl;
				return false;
			}
			if (cmd[i - 1] != '<' && cmd[i - 1] != '>' && cmd[i - 1] != '=' && cmd[i - 1] != ' ' &&				cmd[i - 1] != '!')
			{
				cmd.Insert(i, ' ');
				i++;
			}
			if (cmd[i + 1] != '<' && cmd[i + 1] != '>' && cmd[i + 1] != '=' && cmd[i + 1] != ' '				&&cmd[i] != '!')
			{
				cmd.Insert(i + 1, ' ');
				i++;
			}
		}
	}
	return true;
}


bool interpreter::ExecuteCommand(CString cmd)
{
	if (InitCommand(cmd))
	{
		int i, j, k;
		CString temp = cmd;
		temp.MakeLower();
		if (temp.Find("create table") == 0)
		{
			CString TableName, TableInfo;
			for (i = 0, j = 0; cmd[i] != '(' && i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 3)
			{
				for (i = 13; cmd.GetAt(i) != ' '; i++)
					TableName += cmd.GetAt(i);
				if (cmd.GetAt(i + 1) != '(')
				{
					cout << "在create table附近有语法错误!" << endl;
					return false;
				}
				for (i += 1; cmd.GetAt(i) != ';'; i++)
					TableInfo += cmd.GetAt(i);
			}
			else
			{
				cout << "在create table附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(TableName);
			//	AfxMessageBox(TableInfo);
			if (CreateTable(TableName, TableInfo) == 0)
				return false;
		}
		else if (temp.Find("drop table") == 0)
		{
			CString TableName;
			for (i = 0, j = 0; i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 2)
			{
				for (i = 11; cmd[i] != ';'; i++)
					TableName += cmd[i];
			}
			else
			{
				cout << "在drop table附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(TableName);
			if (DropTable(TableName) == 0)
				return false;
		}
		else if (temp.Find("create index") == 0 || temp.Find("create unique index") == 0)
		{
			CString IndexName, On, TableName, AttName;
			bool unique;
			for (i = 0, j = 0; i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 5)
			{
				unique = 0;
				for (i = 13; cmd[i] != ' '; i++)
					IndexName += cmd[i];
				for (i += 1; temp[i] != ' '; i++)
					On += temp[i];
				if (On = "on")
				{
					for (i += 1; cmd[i] != ' ' && cmd[i] != '('; i++)
						TableName += cmd[i];
					if (cmd[i] == '(')
					{
						for (i += 1; cmd[i] != ')'; i++)
							AttName += cmd[i];
					}
					if (cmd[i + 1] == '(')
					{
						for (i += 2; cmd[i] != ')'; i++)
							AttName += cmd[i];
					}
					else
					{
						cout << "在create index附近有语法错误!" << endl;
						return false;
					}
				}
				else
				{
					cout << "在create index附近有语法错误!" << endl;
					return false;
				}
			}
			else
			{
				if (j == 6)
				{
					unique = 1;
					for (i = 20; cmd[i] != ' '; i++)
						IndexName += cmd[i];
					for (i += 1; temp[i] != ' '; i++)
						On += temp[i];
					if (On = "on")
					{
						for (i += 1; cmd[i] != ' ' && cmd[i] != '('; i++)
							TableName += cmd[i];
						if (cmd[i] == '(')
						{
							for (i += 1; cmd[i] != ')'; i++)
								AttName += cmd[i];
						}
						if (cmd[i + 1] == '(')
						{
							for (i += 2; cmd[i] != ')'; i++)
								AttName += cmd[i];
						}
						else
						{
							cout << "在create index附近有语法错误!" <<								endl;
							return false;
						}
					}
				}
				else
				{
					cout << "在create index附近有语法错误!" << endl;
					return false;
				}
			}
			//	AfxMessageBox(IndexName);
			//	AfxMessageBox(TableName);
			//	AfxMessageBox(AttName);
			if (CreateIndex(IndexName, TableName, AttName, unique) == 0)
				return false;
		}

		else if (temp.Find("drop index") == 0)
		{
			CString IndexName;
			for (i = 0, j = 0; i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 2)
			{
				for (i = 11; cmd[i] != ';'; i++)
					IndexName += cmd[i];
			}
			else
			{
				cout << "在drop index附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(IndexName);
			if (DropIndex(IndexName) == 0)
				return false;
		}
		else if (temp.Find("select") == 0)
		{
			CString Selection, From, TableName, Where, Compare;
			for (i = 0, j = 0; i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 3)
			{
				for (i = 7; cmd[i] != ' '; i++)
					Selection += cmd[i];
				Selection += ";";
				for (i += 1; temp[i] != ' '; i++)
					From += temp[i];
				if (From == "from")
					for (i += 1; cmd[i] != ';'; i++)
						TableName += cmd[i];
				else
				{
					cout << "在select附近有语法错误!" << endl;
					return false;
				}
			}
			else if (j >= 5)
			{
				for (i = 7; cmd[i] != ' '; i++)
					Selection += cmd[i];
				Selection += ";";
				for (i += 1; temp[i] != ' '; i++)
					From += temp[i];
				if (From == "from")
				{
					for (i += 1; cmd[i] != ' '; i++)
						TableName += cmd[i];
					for (i += 1; temp[i] != ' '; i++)
						Where += temp[i];
					if (Where == "where")
						for (i += 1; cmd[i] != ';'; i++)
							Compare += cmd[i];
					Compare = " where " + Compare + ";";
				}
				else
				{
					cout << "在select附近有语法错误!" << endl;
					return false;
				}
			}
			else
			{
				cout << "在select附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(Selection);
			//	AfxMessageBox(TableName);
			//	AfxMessageBox(Compare);
			if (Select(Selection, TableName, Compare) == 0)
				return false;
		}
		else if (temp.Find("insert into") == 0)
		{
			CString TableName, Values, TableInfo;
			for (i = 0, j = 0, k = 0; i<cmd.GetLength(); i++)
			{
				if (cmd[i] == '\'')
					k++;
				if (k % 2 == 0 && cmd[i] == ' ')
					j++;
			}
			if (j == 4)
			{
				for (i = 12; cmd.GetAt(i) != ' '; i++)
					TableName += cmd.GetAt(i);
				for (i += 1; cmd.GetAt(i) != ' '; i++)
					Values += temp.GetAt(i);
				if (Values == "values")
				{
					if (cmd.GetAt(i + 1) != '(')
					{
						cout << "insert into附近有语法错误!" << endl;
						return false;
					}
					else
					{
						for (i += 1; cmd.GetAt(i) != ';'; i++)
							TableInfo += cmd.GetAt(i);
					}
				}
				else
				{
					cout << "insert into附近有语法错误!" << endl;
					return false;
				}
			}
			else
			{
				cout << "在insert into附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(TableName);
			//	AfxMessageBox(TableInfo);
			if (insert(TableName, TableInfo) == 0)
				return false;
		}
		else if (temp.Find("delete from") == 0)
		{
			CString TableName, Where, Compare;
			for (i = 0, j = 0; i<cmd.GetLength(); i++)
				if (cmd[i] == ' ')
					j++;
			if (j == 2)
				for (i = 12; cmd.GetAt(i) != ';'; i++)
					TableName += cmd.GetAt(i);
			else if (j >= 4)
			{
				for (i = 12; cmd.GetAt(i) != ' '; i++)
					TableName += cmd.GetAt(i);
				if (temp.Find("where") == i + 1)
				{
					for (i += 1; temp.GetAt(i) != ' '; i++)
						Where += temp.GetAt(i);
					if (Where == "where")
					{
						for (i += 1; cmd.GetAt(i) != ';'; i++)
							Compare += cmd.GetAt(i);
						Compare = " where " + Compare + ";";
					}
					else
					{
						cout << "在delete from附近有语法错误!" << endl;
						return false;
					}
				}
				else
				{
					cout << "在delete from附近有语法错误!" << endl;
					return false;
				}
			}
			else
			{
				cout << "在delete from附近有语法错误!" << endl;
				return false;
			}
			//	AfxMessageBox(TableName);
			//	AfxMessageBox(Compare);
			if (Delete(TableName, Compare) == 0)
				return false;
		}
		if (temp.Find("create table") == 0 || temp.Find("drop table") == 0 ||
			temp.Find("create index") == 0 || temp.Find("create unique index") == 0 ||
			temp.Find("drop index") == 0 || temp.Find("select") == 0 ||
			temp.Find("insert into") == 0 || temp.Find("delete from") == 0)
			
			return true;
		else
			return false;
	}
	else
		return false;
}


bool interpreter::insert(CString table_name, CString info)
{
	char temp[255];
	TableInformation tableInfo;
	int ii = 0, jj = 0, flag = 1, count = 0, size;
	CString type;
	Record record;

	flag = cm->readTable(table_name, tableInfo);

	if (flag == 0)
	{
		cout << "不存在这个表" << endl;
		return 0;
	}

	if (table_name != currentTableName)
	{
		currentTableName = table_name;
		iset.clear();
		fset.clear();
		cset.clear();
		CString pk;
		pk = tableInfo.primaryKey;
		int i;
		for (i = 0; i < tableInfo.attributeCount; i++)
		{
			if (tableInfo.info[i].attributeName == pk)
			{
				current_a = i;
				break;
			}
		}
		if (i == tableInfo.attributeCount)
			has_pk = false;
		else
			has_pk = true;
	}


	while (info[ii] != ')')
	{
		ii++;
		jj = 0;
		size = 0;
		type = "int";

		while (info[ii] != ',' && info[ii] != ')')
		{
			if (info[ii] == '\'')       // find the type
				type = "char";
			if (info[ii] == '.')
				type = "float";

			temp[jj++] = info[ii++]; //get the info
			size++;
		}

		if (tableInfo.info[count].type != type)
		{
			cout << "第" << count + 1 << "个格式出错" << endl;
			return 0;
		}
		else
			if (type == "char")
			{
			record.value[count].typeNo = 0;
			record.value[count].charValue = new char[tableInfo.info[count].typeCount];
			record.value[count].charCount = tableInfo.info[count].typeCount;
			int k;
			for (k = 1; temp[k] != '\'';)
			{
				record.value[count].charValue.SetAt(k - 1,temp[k]);
				k++;
				if (k - 1 > tableInfo.info[count].typeCount - 1)
				{
					cout << "超过字符数限制!" << endl;
					return 0;
				}
			}
			record.value[count].charValue.SetAt(k - 1, '\0');
			}
			else if (type == "float"){
			int k = 0;
			float kk = 1.0;
			record.value[count].typeNo = 1;
			record.value[count].floatValue = 0.0;
			while (temp[k] != '.')
			{
				record.value[count].floatValue = record.value[count].floatValue * 10 + (temp[k++] - '0');
			}
			k++;
			while (k<size)
			{
				kk = kk*0.1;
				record.value[count].floatValue = record.value[count].floatValue + (temp[k++] - '0')*kk;
			}
				}
				else
				{
					int k = 0;
					record.value[count].typeNo = 2;
					record.value[count].intValue = 0;
					while (k<size)
						record.value[count].intValue = record.value[count].intValue * 10 + temp						[k++] - '0';
				}
		count++;
	}

	if (tableInfo.attributeCount != count)
	{
		cout << "属性个数不符合." << endl;
		return 0;
	}
	else
		record.attributeCount = tableInfo.attributeCount;

	if (record.value[current_a].typeNo == 0 && has_pk == true)
	{
		CString temp;
		temp = record.value[current_a].charValue;
		set<CString>::const_iterator cit;
		cit = cset.find(temp);
		if (cit == cset.end())
			cset.insert(temp);
		else
		{
			cout << "有重复值输入!" << endl;
			return 0;
		}
	}

	if (record.value[current_a].typeNo == 1 && has_pk == true)
	{
		float temp;
		temp = record.value[current_a].floatValue;
		set<float>::const_iterator cit;
		cit = fset.find(temp);
		if (cit == fset.end())
			fset.insert(temp);
		else
		{
			cout << "有重复值输入!" << endl;
			return 0;
		}
	}

	if (record.value[current_a].typeNo == 2 && has_pk == true)
	{
		int temp;
		temp = record.value[current_a].intValue;
		set<int>::const_iterator cit;
		cit = iset.find(temp);
		if (cit == iset.end())
		{
			iset.insert(temp);
		}
		else
		{
			cout << "有重复值输入!" << endl;
			return 0;
		}
	}
	rm->storeRecord(table_name, record);
	return 1;
}


bool interpreter::CreateTable(CString TableName, CString TableInfo)
{
	int i, j, k;
	TableInformation tb;
	tb.tableName = TableName;
	TableInfo.Delete(0, 1);
	TableInfo.Insert(TableInfo.GetLength() - 1, ' ');
	wchar_t* c = new wchar_t[TableInfo.GetLength() + 1];
	_tcscpy(c, TableInfo);
	for (i = 0, j = 0; i<TableInfo.GetLength(); i++)
	{
		if (c[i] == '\'')
			j++;
		if (j % 2 == 0 && c[i] == ',' || c[i] == ' ')
			c[i] = '\0';
	}
	wchar_t* p = c;
	for (k = 0;; k++)
	{
		CString AttName, AttType, s, temp;
		int AttTypeCount;
		AttName = p;
		p += AttName.GetLength() + 1;
		if (AttName == ")")
			break;
		if (AttName == "primary")
		{
			s = p;
			if (s.GetLength() > 3 && s[3] == '(' && s[s.GetLength() - 1] == ')')
			{
				for (i = 0; i<3; i++)
					temp += s[i];
				if (temp == "key")
				{
					for (i = 4; i<s.GetLength() - 1; i++)
						tb.primaryKey += s[i];
					p += s.GetLength() + 1;
					s = p;
				}
				else
				{
					cout << "在primary附近有语法错误!" << endl;
					return false;
				}
			}
			else if (s == "key")
			{
				p += s.GetLength() + 1;
				s = p;
				if (s[0] != '(' || s[s.GetLength() - 1] != ')')
				{
					cout << "在primary key附近有语法错误!" << endl;
					return false;
				}
				p += s.GetLength() + 1;
				s.Delete(0, 1);
				s.Delete(s.GetLength() - 1, 1);
				tb.primaryKey = s;
				s = p;
			}
			else
			{
				cout << "在primary附近有语法错误!" << endl;
				return false;
			}
			if (s != ")")
			{
				cout << "在;附近有语法错误!" << endl;
				return false;
			}
			for (i = 0; i<k; i++)
				if (tb.info[i].attributeName == tb.primaryKey)
				{
				tb.info[i].unique = true;
				tb.info[i].hasIndex = true;
				}
			break;
		}
		if (k >= 32)
		{
			cout << "列数必须小于32!" << endl;
			return false;
		}
		tb.info[k].attributeName = AttName;
		s = p;
		for (i = 0; i<s.GetLength() && s.GetAt(i) != '('; i++)
			AttType += s[i];
		p += AttType.GetLength() + 1;
		if (AttType != "int" && AttType != "float" && AttType != "char")
		{
			cout << "类型错误!" << endl;
			return false;
		}
		tb.info[k].type = AttType;
		if (AttType == "char" && i != s.GetLength() && s[i] == '(' && s[s.GetLength() - 1] == ')')
		{
			s.TrimLeft(AttType);
			s.Delete(0, 1);
			s.Delete(s.GetLength() - 1, 1);
			AttTypeCount = atoi(s) + 1;
			tb.info[k].typeCount = AttTypeCount;
			p += s.GetLength() + 2;
			s = p;
		}
		else if (AttType != "char")
		{
			AttTypeCount = 1;
			tb.info[k].typeCount = AttTypeCount;
		}
		else
		{
			cout << "在char附近有语法错误!" << endl;
			return false;
		}
		if (s == "unique")
		{
			tb.info[k].unique = true;
			s = p;
			p += s.GetLength() + 1;
		}
		else
			tb.info[k].unique = false;
	}
	tb.attributeCount = k;
	cm->saveTable(tb);
	delete[] c;
	return true;
}


bool interpreter::Delete(CString table_name, CString info)
{
	char temp[255];
	CString s;
	TableInformation tableInfo;
	int ii, jj, flag = 1, count = 0, size, i;
	CString type;
	CArray<Compare, Compare&>* rp = new CArray<Compare, Compare&>;

	Compare* cp;

	flag = cm->readTable(table_name, tableInfo);


	if (flag == 0)
	{
		cout << "不存在该表" << endl;
		return 0;
	}

	if (info.GetLength() == 0)
	{
		rm->removeRecord(table_name);
		return 1;
	}

	ii = 0;
	size = 0;
	while (info[ii] != ';')
	{
		if (info[ii++] == ' ')size++;
	}

	if (size % 4 != 0)
	{
		cout << "格式有误" << endl;
		return 0;
	}

	size = size / 4;
	cp = new Compare[size];

	ii = 0;
	while (info[ii] != ';')
	{
		ii++;
		while (info[ii++] != ' '){}

		jj = 0;
		while (info[ii] != ' ')
		{
			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';

		for (i = 0; i<tableInfo.attributeCount; i++)
		{
			if (temp == tableInfo.info[i].attributeName)
			{
				cp[count].item1 = i;
				break;
			}
		}
		if (i == tableInfo.attributeCount)
		{
			cout << "第 " << count + 1 << "个条件的属性不存在!" << endl;
			return 0;
		}

		ii++;
		jj = 0;
		while (info[ii] != ' ')
		{
			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';
		s = temp;

		if (s == "=")
			cp[count].operation = 0;
		else
			if (s == "<")
				cp[count].operation = 1;
			else
				if (s == ">")
					cp[count].operation = 2;
				else
					if (s == "<=")
						cp[count].operation = 3;
					else
						if (s == ">=")
							cp[count].operation = 4;
						else
							if (s == "<>")
								cp[count].operation = 5;

		ii++;
		jj = 0;
		type = "int";
		while (info[ii] != ' ' && info[ii] != ';')
		{
			if (info[ii] == '\'')
				type = "char";
			if (info[ii] == '.')
				type = "float";

			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';



		if (tableInfo.info[i].type != type)
		{
			cout << "类型不符" << endl;

			return 0;
		}
		else
		{
			if (type == "int")
			{
				cp[count].typeNo = 2;
				cp[count].item2 = temp;
			}
			else
				if (type == "float")
				{
				cp[count].typeNo = 1;
				cp[count].item2 = temp;
				}
				else
					if (type == "char")
					{
				cp[count].typeNo = 0;
				int iii = 1;
				while (temp[iii] != '\'')
				{
					temp[iii - 1] = temp[iii];
					iii++;
				}
				temp[iii - 1] = '\0';
				cp[count].item2 = temp;
					}
		}
		rp->Add(cp[count]);
		count++;
	}
	rm->removeRecord(table_name, &tableInfo, rp);
	delete rp;
	return 1;
}

void interpreter::printString(const CString &c)
{
	int size = c.GetLength();
	char* array = new char[size + 1];
   (wchar_t*) strcpy(array, c);
	cout.width(20);
	cout << array;
	delete[] array;
}


bool interpreter::Select(CString Selection, CString TableName, CString Compare)
{
	bool flag;
	if (Compare.GetLength() == 0)
		flag = print1(TableName, Selection);
	else
		flag = print2(TableName, Selection, Compare);
	return flag;
}

bool interpreter::print1(CString Table_name, CString attr)
{
	int flag;
	char temp[255];
	CString s;
	TableInformation tableInfo;
	CArray<Record, Record&>* rp;
	int num, jj;
	int attrflag;
	int at[32];

	flag = cm->readTable(Table_name, tableInfo);

	if (flag == 0)
	{
		cout << "不存在该表" << endl;
		return 0;
	}

	rp = rm->readRecord(Table_name, tableInfo);

	if (rp == NULL || rp->GetSize() == 0)
	{
		cout << "没有找到记录1!" << endl;
		return 1;
	}

	num = rp->GetSize();

	cout.setf(ios::right);
	if (attr == "*;")
	{
		int a;
		for (a = 0; a<tableInfo.attributeCount; a++)
		{
			printString("列名");
		}
		cout << endl;
		for (a = 0; a<tableInfo.attributeCount; a++)
		{
			printString(tableInfo.info[a].attributeName);
		}
		cout << endl;

		for (int b = num - 1; b >= 0; b--)
		{
			for (a = 0; a<tableInfo.attributeCount; a++)
			{
				if (tableInfo.info[a].type == "char")
					cout << setw(20) << (*rp)[b].value[a].charValue;
				else
					if (tableInfo.info[a].type == "float")
						cout << setw(20) << (*rp)[b].value[a].floatValue;
					else
						if (tableInfo.info[a].type == "int")
							cout << setw(20) << (*rp)[b].value[a].intValue;
			}
			cout << endl;
		}
	}
	else
	{
		int attrCount = 0;
		int ai = 0;
		int na;
		attr.Insert(0, ',');
		while (attr[ai] != ';')
		{
			jj = 0;
			ai++;
			while ((attr[ai] != ',') && (attr[ai] != ';'))
			{
				temp[jj++] = attr[ai++];
			}
			temp[jj] = '\0';
			s = temp;
			attrflag = 0;
			for (na = 0; na<tableInfo.attributeCount; na++)
			{
				if (s == tableInfo.info[na].attributeName)
				{
					at[attrCount] = na;
					attrflag = 1;
					break;
				}
			}
			if (attrflag == 0)
			{
				cout << "第" << attrCount + 1 << "个属性名未找到" << endl;
				return 0;
			}
			attrCount++;
		}
		int a;
		for (int a = 0; a<attrCount; a++)
		{
			printString("列名");
		}
		cout << endl;
		for (a = 0; a<attrCount; a++)
		{
			printString(tableInfo.info[(at[a])].attributeName);
		}
		cout << endl;

		for (int b = num - 1; b >= 0; b--)
		{
			for (a = 0; a<attrCount; a++)
			{
				if (tableInfo.info[at[a]].type == "char")
					cout << setw(20) << (*rp)[b].value[at[a]].charValue;
				else
					if (tableInfo.info[at[a]].type == "float")
						cout << setw(20) << (*rp)[b].value[at[a]].floatValue;
					else
						if (tableInfo.info[at[a]].type == "int")
							cout << setw(20) << (*rp)[b].value[at[a]].intValue;
			}
			cout << endl;
		}
	}
	cout.unsetf(ios::right);
	delete rp;
	return 1;
}

bool interpreter::DropIndex(CString indexName)
{
	return 1;
}

bool interpreter::print2(CString Table_name, CString attr, CString info)
{
	int flag;
	char temp[255];
	CString s, type;
	TableInformation tableInfo;
	CArray<Record, Record&>* rp;
	CArray<Compare, Compare&>* ccp = new CArray<Compare, Compare&>;
	Compare* cp;
	int num, jj, ii, size, i, count;
	int attrflag;
	int at[32];

	flag = cm->readTable(Table_name, tableInfo);

	if (flag == 0)
	{
		cout << "不存在该表" << endl;
		return 0;
	}

	ii = 0;
	size = 0;
	while (info[ii] != ';')
	{
		if (info[ii++] == ' ')size++;
	}

	if (size % 4 != 0)
	{
		cout << "格式有误" << endl;
		return 0;
	}

	size = size / 4;
	cp = new Compare[size];

	ii = 0;
	count = 0;
	while (info[ii] != ';')
	{
		ii++;
		while (info[ii++] != ' '){}

		jj = 0;
		while (info[ii] != ' ')
		{
			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';

		for (i = 0; i<tableInfo.attributeCount; i++)
		{
			if (temp == tableInfo.info[i].attributeName)
			{
				cp[count].item1 = i;
				break;
			}
		}

		if (i == tableInfo.attributeCount)
		{
			cout << "第 " << count + 1 << "个条件的属性不存在!" << endl;
			return 0;
		}

		ii++;
		jj = 0;
		while (info[ii] != ' ')
		{
			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';
		s = temp;

		if (s == "=")
			cp[count].operation = 0;
		else
			if (s == "<")
				cp[count].operation = 1;
			else
				if (s == ">")
					cp[count].operation = 2;
				else
					if (s == "<=")
						cp[count].operation = 3;
					else
						if (s == ">=")
							cp[count].operation = 4;
						else
							if (s == "<>")
								cp[count].operation = 5;

		ii++;
		jj = 0;
		type = "int";
		while (info[ii] != ' ' && info[ii] != ';')
		{
			if (info[ii] == '\'')
				type = "char";
			if (info[ii] == '.')
				type = "float";

			temp[jj++] = info[ii++];
		}
		temp[jj] = '\0';



		if (tableInfo.info[i].type != type)
		{
			cout << "类型不符" << endl;

			return 0;
		}
		else
		{
			if (type == "int")
			{
				cp[count].typeNo = 2;
				cp[count].item2 = temp;
			}
			else
				if (type == "float")
				{
				cp[count].typeNo = 1;
				cp[count].item2 = temp;
				}
				else
					if (type == "char")
					{
				cp[count].typeNo = 0;
				int iii = 1;
				while (temp[iii] != '\'')
				{
					temp[iii - 1] = temp[iii];
					iii++;
				}
				temp[iii - 1] = '\0';
				cp[count].item2 = temp;
					}
		}
		ccp->Add(cp[count]);
		count++;
	}

	rp = rm->readRecord(Table_name, tableInfo, ccp);

	if (rp == NULL || rp->GetSize() == 0)
	{
		cout << "没有找到记录!" << endl;
		return 1;
	}
	num = rp->GetSize();
	cout.setf(ios::right);
	if (attr == "*;")
	{
		int a;
		for (a = 0; a<tableInfo.attributeCount; a++)
		{
			printString("列名");
		}
		cout << endl;
		for (a = 0; a<tableInfo.attributeCount; a++)
		{
			printString(tableInfo.info[a].attributeName);
		}
		cout << endl;

		for (int b = num - 1; b >= 0; b--)
		{
			for (a = 0; a<tableInfo.attributeCount; a++)
			{
				if (tableInfo.info[a].type == "char")
					cout << setw(20) << (*rp)[b].value[a].charValue;
				else
					if (tableInfo.info[a].type == "float")
						cout << setw(20) << (*rp)[b].value[a].floatValue;
					else
						if (tableInfo.info[a].type == "int")
							cout << setw(20) << (*rp)[b].value[a].intValue;
			}
			cout << endl;
		}
	}
	else
	{
		int attrCount = 0;
		int ai = 0;
		int na;
		attr.Insert(0, ',');
		while (attr[ai] != ';')
		{
			jj = 0;
			ai++;
			while ((attr[ai] != ',') && (attr[ai] != ';'))
			{
				temp[jj++] = attr[ai++];
			}
			temp[jj] = '\0';
			s = temp;
			attrflag = 0;
			for (na = 0; na<tableInfo.attributeCount; na++)
			{
				if (s == tableInfo.info[na].attributeName)
				{
					at[attrCount] = na;
					attrflag = 1;
					break;
				}
			}
			if (attrflag == 0)
			{
				cout << "第" << attrCount + 1 << "个属性名未找到" << endl;
				return 0;
			}
			attrCount++;
		}
		int a;
		for (a = 0; a<attrCount; a++)
		{
			printString("列名");
		}
		cout << endl;
		for (a = 0; a<attrCount; a++)
		{
			printString(tableInfo.info[(at[a])].attributeName);
		}
		cout << endl;

		for (int b = num - 1; b >= 0; b--)
		{
			for (a = 0; a<attrCount; a++)
			{
				if (tableInfo.info[at[a]].type == "char")
					cout << setw(20) << (*rp)[b].value[at[a]].charValue;
				else
					if (tableInfo.info[at[a]].type == "float")
						cout << setw(20) << (*rp)[b].value[at[a]].floatValue;
					else
						if (tableInfo.info[at[a]].type == "int")
							cout << setw(20) << (*rp)[b].value[at[a]].intValue;
			}
			cout << endl;
		}
	}
	cout.unsetf(ios::right);
	delete rp;
	delete ccp;
	return 1;
}

bool interpreter::CreateIndex(CString IndexName, CString TableName, CString AttName, bool unique)
{
	TableInformation a;
	if (cm->readTable(TableName, a))
	{
		int i;
		for (i = 0; i < a.attributeCount; i++)
			if (a.info[i].attributeName == AttName)
				break;
		if (i == a.attributeCount)
		{
			cout << "不存在此属性!" << endl;
			return 0;
		}
		if (a.info[i].unique == false && unique == false)
		{
			cout << "该属性不唯一，无法创建index!" << endl;
			return 0;
		}
	}
	else
	{
		cout << "不存在此表" << endl;
		return 0;
	}
	return 1;
}

bool interpreter::DropTable(CString tableName)
{
	rm->removeRecord(tableName);
	cm->dropTable(tableName);
	return 1;
}