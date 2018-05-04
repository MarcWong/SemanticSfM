//
// Created by Haowen Xu on 9/29/16.
//

#ifndef MESHSIMPLIFICATION_BILATERALNORMALFILTER_H
#define MESHSIMPLIFICATION_BILATERALNORMALFILTER_H

#pragma once
#include "Mesh.h"

class AlgBilateralNormalFilter
{
public:
    AlgBilateralNormalFilter()
    {
        pMesh = NULL;
        normalIteration = 5;
        dualVertexPos.clear();
        smoothNess = 0.1;
        gc = 1;
        gs = 0.35;
        useSimilarityWeights = true;
        faceNormal.clear();
        avgeLen = 0;
    };
    ~AlgBilateralNormalFilter() {}
    bool BilateralNormalSmoothing();
    bool Init(TriMesh*);

private:
    void computeAreaAndLength();
    double faceWeight(int, int);
    void stepNormalFiltering();
    void computeDualPosition();

    TriMesh * pMesh;
    int normalIteration;
    unordered_map<int, Point3D> dualVertexPos;
    unordered_map<int, Point3D> faceNormal;
    double smoothNess;
    double gc;// = 1;  first term gaussian width.
    double gs;// = 0.35; second term gaussian width.
    bool useSimilarityWeights;
    double avgeLen;
};

#endif //MESHSIMPLIFICATION_BILATERALNORMALFILTER_H
