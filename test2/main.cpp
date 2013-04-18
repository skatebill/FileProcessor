#include"Chunk.h"
void main(){
	/*FileWriter t;
	t.openFile("123.BINARY","wb");
	t.writeString("test");
	t.close();
	FileReader r;
	r.openFile("123.BINARY","rb");
	char buf[256];
	r.readString(buf);
	r.close();*/


	meshChunk test;
	test.vertexlist.push_back(new vertex(1,2,3));
	meshGroupChunk group;
	group.addSunChunk(test);

	matrialChunk mat;
	mat.hasTexture=true;
	mat.textureName="test.png";
	mat.amb=point4(1,0,0,1);

	matrialGroupChunk mgroup;
	mgroup.addSunChunk(mat);

	mainChunk m;
	m.addSunChunk(group);
	m.addSunChunk(mgroup);
	int length=m.computeLength();
	
	chunkWriter writer;

	writer.saveChunkTo(m,"test.chunk");

	chunkReader reader;

	mainChunk* wanted=reader.readFile("test.chunk");

}	