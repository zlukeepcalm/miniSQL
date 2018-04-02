#include"basic.h"
#include"DBFile.h"
#include"block.h"
#include"bufferManager.h"
#include"recordManager.h"
#include"CategoryManager.h"
#include"interpreter.h"
#include<afxwin.h>
#include <stdlib.h>

#include<iostream>
#include<fstream>
using namespace std;


int main()
{
	char cmd[100];
	CString a, temp, tmp;
	char* filename=NULL;
	interpreter b;
	char  in;
	cout << "Welcome to Minisql: " << endl;
	while(1)
	{
		cout << "*********************************************************** " << endl;
		cout << "Please input sql command: " << endl;
		a = "";
		do
		{
			gets_s(cmd);
			a += " ";
			a += cmd;

		}
		while(a.GetAt(a.GetLength() - 1) != ';');
		
	//	AfxMessageBox(a);

		if(a == " quit;")
			break;
		CString temp = a;
		temp.MakeLower();
		if(temp.Find("create") != -1 || temp.Find("drop") != -1
			|| temp.Find("select") != -1 || temp.Find("insert") != -1
			|| temp.Find("delete") != -1)
		{	
			if (b.ExecuteCommand(a)){
				cout << "指令成功运行!" << endl;
				cout << "总时间为" << b.Total_time << "秒" << endl;
			}
			else
				cout << "指令失败!" << endl;
		}
		else if (temp.Find("execfile") != -1){
			for (int i = 10; i < temp.GetLength()-1; i++){
				filename += temp.GetAt(i);
			}
			//cout << filename << endl;
			
			ifstream fin("e:\sql.txt");
			a = "";
			if (!fin) // check whether file opened successfully
				cout << "File not exist!" << endl;
			else
			{
				while (!fin.eof()){
					//cout << "innnnnn" << endl;
					in = fin.get();
					a += in;
					//cout << a << endl;
					if (a.GetAt(a.GetLength() - 1) == ';')
					{
						cout << a << endl;
						CString tmp = a;
						tmp.MakeLower();
						if (tmp.Find("create") != -1 || tmp.Find("drop") != -1
							|| tmp.Find("select") != -1 || tmp.Find("insert") != -1
							|| tmp.Find("delete") != -1)
						{
							if (b.ExecuteCommand(a)){
								cout << "指令成功运行!" << endl;
								cout << "总时间为" << b.Total_time << "秒" << endl;
							}
							else
								cout << "指令失败!" << endl;
							a = "";
						}
					}
				}
				fin.close();
			}
		}
		else
		{
			cout << "指令格式错误!" << endl;
		}
	}
	return 0;
}
