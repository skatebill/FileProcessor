#include"Chunk.h"

bool chunkWriter::saveChunkTo(mainChunk &source,char* dstFile){
	if(openFile(dstFile,"wb")){
		if(!source.length)
			source.computeLength();
		saveChunk(source);
		close();
		return true;
	}
	return false;
}
void chunkWriter::saveChunk(chunk& chunk){
	switch(chunk.id)
	{
	case MAINCHUNK:
		saveMainChunk(*(mainChunk*)&chunk);
		break;
	case MESHCHUNK:
		saveMeshChunk(*(meshChunk*)&chunk);
		break;
	case MATRIALCHUNK:
		saveMatrialChunk((*(matrialChunk*)&chunk));
	case MESHGROUPCHUNK:
	case MATRIALGROUPCHUNK:
		saveChunkHead(chunk);
		saveSubChunk(*(ChunkContainer*)&chunk);
		break;
	default:
		break;
	}
}
void chunkWriter::saveSubChunk(ChunkContainer& container){
	for(vector<chunk*>::iterator ite=container.subchunk.begin();ite!=container.subchunk.end();ite++){
		saveChunk(**ite);
	}

}
void chunkWriter::saveMainChunk(mainChunk& chunk){
	saveChunkHead(chunk);
	saveSubChunk(chunk);
}
void chunkWriter::saveMeshChunk(meshChunk& chunk){
	saveChunkHead(chunk);

	writeDWord(chunk.vertexlist.size());
	writeDWord(chunk.texcoordlist.size());
	writeDWord(chunk.normallist.size());
	writeDWord(chunk.colorlist.size());
	writeDWord(chunk.indexlist.size());

	for(vector<vertex*>::iterator ite=chunk.vertexlist.begin();ite!=chunk.vertexlist.end();ite++){
		writeFloat((**ite).pos.x);
		writeFloat((**ite).pos.y);
		writeFloat((**ite).pos.z);
	}
	for(vector<point2*>::iterator ite=chunk.texcoordlist.begin();ite!=chunk.texcoordlist.end();ite++){
		writeFloat((**ite).u);
		writeFloat((**ite).v);

	}
	for(vector<point3*>::iterator ite=chunk.normallist.begin();ite!=chunk.normallist.end();ite++){
		writeFloat((**ite).x);
		writeFloat((**ite).y);
		writeFloat((**ite).z);

	}
	for(vector<point4*>::iterator ite=chunk.colorlist.begin();ite!=chunk.colorlist.end();ite++){
		writeFloat((**ite).x);
		writeFloat((**ite).y);
		writeFloat((**ite).z);
		writeFloat((**ite).w);

	}
	for(vector<face*>::iterator ite=chunk.indexlist.begin();ite!=chunk.indexlist.end();ite++){
		writeWord((**ite).a);
		writeWord((**ite).b);
		writeWord((**ite).c);


	}
}

void chunkWriter::saveMatrialChunk(matrialChunk &chunk){
	saveChunkHead(chunk);
	//mat id
	writeWord(chunk.matID);
	//abm
	writeFloat(chunk.amb.x);
	writeFloat(chunk.amb.y);
	writeFloat(chunk.amb.z);
	writeFloat(chunk.amb.w);
	//dif
	writeFloat(chunk.dif.x);
	writeFloat(chunk.dif.y);
	writeFloat(chunk.dif.z);
	writeFloat(chunk.dif.w);
	//spec
	writeFloat(chunk.spec.x);
	writeFloat(chunk.spec.y);
	writeFloat(chunk.spec.z);
	writeFloat(chunk.spec.w);

	//tex
	writeByte(chunk.hasTexture);
	if(chunk.hasTexture)
	{
		writeString(chunk.textureName);
	}
	
}

void chunkReader::skipChunk(ChunkHead& head){
	fseek(m_DstFile,head.length-CHUNK_HEAD_SIZE,SEEK_CUR);
}
ChunkHead chunkReader::readChunkHead(){
	ChunkHead head;
	head.id=readWord();
	head.length=readDWord();
	return head;
}
mainChunk* chunkReader::readMainChunk(){
	ChunkHead head=readChunkHead();
	if(head.id!=MAINCHUNK)
	{
		return 0;
	}
	mainChunk* m=new mainChunk;
	m->length=head.length;

	int num=0;
	while(num<head.length-CHUNK_HEAD_SIZE)
	{
		ChunkHead head=readChunkHead();
		switch(head.id)
		{
		case MESHGROUPCHUNK:
			readMeshGroupChunk(head,*m);
			break;
		case MATRIALGROUPCHUNK:
			readMatrialGroupChunk(head,*m);
			break;
		default:
			skipChunk(head);
			break;
		}
		num+=head.length;
	}
	return m;
}	
void chunkReader::readMeshGroupChunk(ChunkHead &grouphead,mainChunk &chunk){
	meshGroupChunk *group=new meshGroupChunk;
	group->length=grouphead.length;
	chunk.addSunChunk(*group);
	int num=0;
	while(num<grouphead.length-CHUNK_HEAD_SIZE)
	{
		ChunkHead head=readChunkHead();
		switch(head.id)
		{
		case MESHCHUNK:
			readMeshChunk(head,*group);
			break;
		default:
			skipChunk(head);
			break;
		}
		num+=head.length;
	}
}
void chunkReader::readMatrialGroupChunk(ChunkHead &grouphead,mainChunk &chunk){
	matrialGroupChunk *group=new matrialGroupChunk;
	group->length=grouphead.length;
	chunk.addSunChunk(*group);
	int num=0;
	while(num<grouphead.length-CHUNK_HEAD_SIZE)
	{
		ChunkHead head=readChunkHead();
		switch(head.id)
		{
		case MATRIALCHUNK:
			readMatrialChunk(head,*group);
			break;
		default:
			skipChunk(head);
			break;
		}
		num+=head.length;
	}
}
void chunkReader::readMatrialChunk(ChunkHead &meshhead,matrialGroupChunk &chunk){
	matrialChunk *m=new matrialChunk;
	m->length=meshhead.length;
	chunk.addSunChunk(*m);
	//mat id
	m->matID=readWord();
	//amb
	m->amb.x=readFloat();
	m->amb.y=readFloat();
	m->amb.z=readFloat();
	m->amb.w=readFloat();
	//dif
	m->dif.x=readFloat();
	m->dif.y=readFloat();
	m->dif.z=readFloat();
	m->dif.w=readFloat();
	//spec
	m->spec.x=readFloat();
	m->spec.y=readFloat();
	m->spec.z=readFloat();
	m->spec.w=readFloat();
	//tex
	m->hasTexture=readByte();
	if(m->hasTexture)
	{
		m->textureName=(char*)malloc(64);
		readString(m->textureName,64);
	}

}

void chunkReader::readMeshChunk(ChunkHead &meshhead,meshGroupChunk &chunk){
	meshChunk *m=new meshChunk;
	m->length=meshhead.length;
	chunk.addSunChunk(*m);

	DWORD vertexnum=readDWord();
	DWORD texcoordnum=readDWord();
	DWORD normalnum=readDWord();
	DWORD colornum=readDWord();
	DWORD indexnum=readDWord();

	for(int i=0;i<vertexnum;i++){
		float x,y,z;
		x=readFloat();
		y=readFloat();
		z=readFloat();
		m->vertexlist.push_back(new vertex(x,y,z));

	}
	for(int i=0;i<texcoordnum;i++){
		float u,v;
		u=readFloat();
		v=readFloat();

	}
	for(int i=0;i<normalnum;i++){
		float nx,ny,nz;
		nx=readFloat();
		ny=readFloat();
		nz=readFloat();

	}
	for(int i=0;i<colornum;i++){
		float x,y,z,w;
		x=readFloat();
		y=readFloat();
		z=readFloat();
		w=readFloat();

	}
	for(int i=0;i<indexnum;i++){
		WORD a,b,c;
		a=readWord();
		b=readWord();
		c=readWord();
	}

}
mainChunk* chunkReader::readFile(char* dstFile){
	openFile(dstFile,"rb");
	mainChunk* r=readMainChunk();
	close();
	return r;
}