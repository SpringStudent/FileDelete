#pragma once
#include<iostream>
#include "string"
using namespace std;

class Document {

public:
	string path;
	long size;
	string md5;
	bool isFile;

	Document(string path,long size);

};