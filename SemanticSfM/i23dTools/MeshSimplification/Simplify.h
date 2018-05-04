//
// Created by Haowen Xu on 9/28/16.
//

#ifndef MESHSIMPLIFICATION_SIMPLIFY_H
#define MESHSIMPLIFICATION_SIMPLIFY_H

#include "Mesh.h"
#include "queue"
#include <map>
#include "MxHeap.h"
#include "Eigen/Dense"

class SimplifyNode :public MxHeapable
{
public:
    Point3D targetPos;
    SimplifyNode() { index1 = index2 = -1; }
    SimplifyNode(int a, int b, double c, Point3D target)
    {
        index1 = a;
        index2 = b;
        heap_key(c);
        targetPos.x = target.x;
        targetPos.y = target.y;
        targetPos.z = target.z;
    }

    int index1, index2;
};

class AlgSimplify
{
public:
    AlgSimplify() { mesh = NULL; }
    void init(TriMesh*);
    void getQ();
    double getCost(int, int, Point3D&);
    bool contract();
    void contractOne();
    void filter();
private:
    vector<Eigen::Matrix4d, Eigen::aligned_allocator<Eigen::Matrix4d>> matrixQ;
    MxHeap* heap;
    TriMesh* mesh;
    unordered_map<long long int, SimplifyNode*> mp;
    unordered_map<int, Point3D> gNormal;

};

#endif //MESHSIMPLIFICATION_SIMPLIFY_H
