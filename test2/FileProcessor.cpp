#include"FileProcessor.h"


void FileWriter::writeByte(BYTE data)
{
	fwrite(&data,1,1,m_DstFile);
}
void FileWriter::writeWord(WORD data)
{
	fwrite(&data,2,1,m_DstFile);
}
void FileWriter::writeDWord(DWORD data)
{
	fwrite(&data,4,1,m_DstFile);
}
void FileWriter::writeFloat(float data)
{
	fwrite(&data,4,1,m_DstFile);
}

void FileWriter::writeString(char* data)
{
	char* p=data;
	do{
		writeByte(*p);
	}while(*p++);
}


BYTE FileReader::readByte(){
	BYTE r;
	fread(&r,1,1,m_DstFile);
	return r;

}
WORD FileReader::readWord(){
	return readByte()+(readByte()<<8);

}
DWORD FileReader::readDWord(){
	return readWord()+(readWord()<<16);

}
float FileReader::readFloat(){
	float r;
	fread(&r,1,4,m_DstFile);
	return r;

}
void FileReader::readString(char* buf,int max){
	
	char* p=buf;
	do{
		*p=readByte();
	}while(*p++ && p-buf < max);
	buf[max-1]=0;
}