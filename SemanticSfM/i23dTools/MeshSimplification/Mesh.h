//

#ifndef MESHSIMPLIFICATION_MESH_H
#define MESHSIMPLIFICATION_MESH_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Feature.h"

using namespace std;

#define PTSET unordered_map<int, Vertex>
#define FACESET unordered_map<int, Face>
#define ADJSET unordered_set<int>

class Point3D
{
public:
    Point3D() { x = y = z = 0; }
    Point3D(double a, double b, double c) : x(a), y(b), z(c) {}

    Point3D operator+(const Point3D& b) const
    {
        return Point3D(x + b.x, y + b.y, z + b.z);
    }

    Point3D operator-(const Point3D& b) const
    {
        return Point3D(x - b.x, y - b.y, z - b.z);
    }

    Point3D operator*(double b) const
    {
        return Point3D(x * b, y * b, z * b);
    }

    double operator*(const Point3D& b) const
    {
        return x * b.x + y * b.y + z * b.z;
    }

    Point3D operator/(double b) const
    {
        return Point3D(x / b, y / b, z / b);
    }

    Point3D operator^(const Point3D& b) const
    {
        return Point3D(y * b.z - b.y * z, b.x * z - x * b.z, x * b.y - y * b.x);
    }

    double getLen() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize()
    {
        auto l = getLen();
        if (l > 0)
        {
            x /= l;
            y /= l;
            z /= l;
        }
    }

    double x, y, z;
};

class Vertex
{
public:
    Vertex() { adjFaces.clear(); }
    Vertex(Point3D p) { adjFaces.clear(); pos = p; red = 0; green = 0; blue = 0; nx = 0; ny = 0; nz = 0;flags = 0; }

    Point3D pos;
    int red, green, blue, flags;
    double nx,ny,nz;
    vector<int> adjFaces;
};

class Face
{
public:
    Face() {}
    Face(int x, int y, int z) { index[0] = x; index[1] = y, index[2] = z; flags = 0; }

    int index[3];
    int flags;
    Point3D normal;
};

class TriMesh
{
public:
    TriMesh() { pointSet.clear(); faceSet.clear(); maxPtIndex = maxFaceIndex = 0; featurePatch = nullptr; vertex_property = 0;face_property = 0;}
    int addVertex(Point3D);
    int addFace(int, int, int);
    int addPlyVertex(Point3D,int,int,int,double,double,double,int);
    int addPlyFace(int,int,int,int);
    bool deleteVertex(int);
    bool deleteFace(int);
    bool buildTopo();
    bool contract(int, int);
    bool contract(int, int, Point3D);
    bool flip(int, int);

    ADJSET getAdjVertexOfVertex(int);
    vector<int> getAdjFaceOfEdge(int, int);
    bool isBoundaryEdge(int, int);
    bool isBoundaryVertex(int);

    void loadMeshFromObjFile(const char*);
    void saveMeshToObjFile(const char*);
    void loadMeshFromPlyFile(const char*);
    void saveMeshToPlyFile(const char*,const char*);
    void computeNormal();

    bool constractFeatureGraph();

    double getArea(int);

    PTSET getPointSet() { return pointSet; }
    FACESET getFaceSet() { return faceSet; }
    PTSET& PointSet() { return pointSet; }
    FACESET& FaceSet() { return faceSet; }
    int getMaxPtIndex() { return maxPtIndex; }

    FeaturePatch* featurePatch;

private:
    PTSET pointSet;
    FACESET faceSet;
    int maxPtIndex;
    int maxFaceIndex;
    int vertex_property;
    int face_property;
    char property_name[10][512];
};

#endif //MESHSIMPLIFICATION_MESH_H
