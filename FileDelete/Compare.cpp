#include<iostream>
using namespace std;
#include<string>
#include<vector>
#include "Document.h"
#include<io.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<map>
#include<set>
#include<fstream>

#define INSSQL ofs << "insert into compare_file(`md5`,`fpath`,`opath`) values" << endl;
#define SQLVAL ofs << "('" << pos->second.md5 << "','" << pos->second.path << "','" << it->path << "'),"<<endl;

void getDocuments(string path, vector<Document>& files) {
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR))
			{  // 跳过当前目录和父目录
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{	
					files.push_back(Document(p.assign(path).append("/").append(fileinfo.name), fileinfo.size));
					//递归搜索
					getDocuments(p.assign(path).append("/").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(Document(p.assign(path).append("/").append(fileinfo.name), fileinfo.size));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int main() {
	string source;
	cout << "请输入源文件目录：" << endl;
	while (source.empty()) {
		cin >> source;
	}
	string dest;
	cout << "请输入目的文件目录：" << endl;
	while (dest.empty()) {
		cin >> dest;
	}
	vector<Document> sourceDoc;
	vector<Document> destDoc;

	//两个文件目录相同，只计算一次
	if (strcmp(source.c_str(), dest.c_str()) == 0) {
		getDocuments(source, sourceDoc);
		destDoc = sourceDoc;
	}else {
		getDocuments(source, sourceDoc);
		getDocuments(dest, destDoc);
	}
	multimap<string, Document> sourcemap;
	for (vector<Document>::iterator it = sourceDoc.begin(); it != sourceDoc.end(); it++) {
		sourcemap.insert(make_pair(it->md5, *it));
	}

	multimap<string, Document> destmap;
	for (vector<Document>::iterator it = destDoc.begin(); it != destDoc.end(); it++) {
		destmap.insert(make_pair(it->md5, *it));
	}
	
	int file_inx = 0;
	int ins_inx = 0;
	string new_file;

	ofstream ofs;
	ofs.open("compare_file.sql", ios::out);
	INSSQL;

	cout << "开始比对" << endl;
	for (vector<Document>::iterator it = sourceDoc.begin(); it != sourceDoc.end(); it++) {
		if (it->isFile) {
			//查找目标目录和源目录md5重复的文件
			int cunt = destmap.count(it->md5);
			if (cunt > 0) {
				multimap<string, Document>::iterator pos = destmap.find(it->md5);
				cout << "与文件:" << it->path << "内容相同的文件" << endl;
				for (int i = 0; pos != destmap.end() && i < cunt; pos++, i++) {
					//每次写入前，判断是否需要重新写入新文件
					if (ins_inx != 0 && ins_inx % 5 == 0) {
						ofs.close();
						file_inx++;
						ofs.open("compare_file" + to_string(file_inx) + ".sql", ios::out);
						INSSQL;
					}
					//相同路径文件无需保存
					if (pos->second.path!= it->path) {
						cout << pos->second.path << endl;
						SQLVAL;
						ins_inx++;
					}
				}
			}
		}
	}
	ofs.close();
	cout << "比对结束" << endl;
	system("pause");
	return 0;
}
