#include "Document.h"
#include "md5.h"
#include "fstream"

string fileDigest(const string &file) {
	ifstream in(file.c_str(), ios::binary);
	if (!in)
		return "";

	MD5 md5;
	std::streamsize length;
	char buffer[1024];
	while (!in.eof()) {
		in.read(buffer, 1024);
		length = in.gcount();
		if (length > 0)
			md5.update(buffer, length);
	}
	in.close();
	return md5.toString();
}

bool checkFile(string path) {
	const char * p = path.c_str();
	struct stat s;
	if (stat(p, &s) == 0) {
		if (s.st_mode & S_IFREG) {
			return true;
		}
	}
	return false;

}

Document::Document(string path,long size){
	this->path = path;
	this->size = size;
	this->isFile = checkFile(path);
	this->md5 = fileDigest(path);
}







