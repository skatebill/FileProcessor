#pragma once
#include<iostream>
#include<fstream>
#include<windows.h>
#define BYTE unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long

class FileBase{
protected:
	FILE* m_DstFile;
public:
	FileBase():m_DstFile(0){};
	virtual ~FileBase(){close();};

	bool openFile(const char* filename,const char* mode){
		if(m_DstFile)
			close();
		fopen_s(&m_DstFile,filename,mode);
		if(m_DstFile) return true;
		return false;
		
	};
	void close(){
		if(m_DstFile)
		{
			fclose(m_DstFile);
		}
		m_DstFile=0;
	}
};




class FileWriter:public FileBase{	
public:
	FileWriter():FileBase(){};
	virtual ~FileWriter(){};
	
	void writeByte(BYTE);
	void writeWord(WORD);
	void writeDWord(DWORD);
	void writeFloat(float);
	void writeString(char*);
	
};


class FileReader:public FileBase{
private:
public:
	FileReader():FileBase(){};
	virtual ~FileReader(){};

	BYTE readByte();
	WORD readWord();
	DWORD readDWord();
	float readFloat();
	void readString(char*,int=256);
	
};