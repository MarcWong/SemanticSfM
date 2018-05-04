#include <stdio.h>
#include "Mesh.h"
#include "Simplify.h"

using namespace std;

int main(int argc, char* argv[])
{
    TriMesh mesh;
    int tarNum;
    argc = 4;
    if(strstr(argv[1],"ply")!=NULL)
    {
		
	    mesh.loadMeshFromPlyFile(argv[1]);
    }
    else
    	mesh.loadMeshFromObjFile(argv[1]);
    
    sscanf(argv[3], "%d", &tarNum);
    AlgSimplify alg;
    alg.init(&mesh);
    printf("alg init complete\n");

    alg.filter();
	printf("alg filter complete\n");

    alg.getQ();
	printf("alg getQ complete\n");
    while (mesh.FaceSet().size() > tarNum)
    {
        alg.contractOne();
    }

    printf("start saving now\n");	
    if(strstr(argv[1],"ply")!=NULL)
{
    mesh.saveMeshToPlyFile(argv[2],argv[1]);
	
 }  
 else
	mesh.saveMeshToObjFile(argv[2]);
     puts("Successfully saved.");
    return 0;
}
