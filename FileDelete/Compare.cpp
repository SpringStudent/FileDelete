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
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR))
			{  // ������ǰĿ¼�͸�Ŀ¼
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{	
					files.push_back(Document(p.assign(path).append("/").append(fileinfo.name), fileinfo.size));
					//�ݹ�����
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
	cout << "������Դ�ļ�Ŀ¼��" << endl;
	while (source.empty()) {
		cin >> source;
	}
	string dest;
	cout << "������Ŀ���ļ�Ŀ¼��" << endl;
	while (dest.empty()) {
		cin >> dest;
	}
	vector<Document> sourceDoc;
	vector<Document> destDoc;

	//�����ļ�Ŀ¼��ͬ��ֻ����һ��
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

	cout << "��ʼ�ȶ�" << endl;
	for (vector<Document>::iterator it = sourceDoc.begin(); it != sourceDoc.end(); it++) {
		if (it->isFile) {
			//����Ŀ��Ŀ¼��ԴĿ¼md5�ظ����ļ�
			int cunt = destmap.count(it->md5);
			if (cunt > 0) {
				multimap<string, Document>::iterator pos = destmap.find(it->md5);
				cout << "���ļ�:" << it->path << "������ͬ���ļ�" << endl;
				for (int i = 0; pos != destmap.end() && i < cunt; pos++, i++) {
					//ÿ��д��ǰ���ж��Ƿ���Ҫ����д�����ļ�
					if (ins_inx != 0 && ins_inx % 5 == 0) {
						ofs.close();
						file_inx++;
						ofs.open("compare_file" + to_string(file_inx) + ".sql", ios::out);
						INSSQL;
					}
					//��ͬ·���ļ����豣��
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
	cout << "�ȶԽ���" << endl;
	system("pause");
	return 0;
}
