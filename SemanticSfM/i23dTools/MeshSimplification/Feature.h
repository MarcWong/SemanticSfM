//
// Created by Haowen Xu on 9/27/16.
//

#ifndef MESHSIMPLIFICATION_FEATURE_H
#define MESHSIMPLIFICATION_FEATURE_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Eigen/Dense"
using namespace std;

class FeaturePatch
{
public:
    FeaturePatch() {}
    bool addFaceToPatch(int, int);
    int addPatch(unordered_set<int>);
    int addPatch(vector<int>);
    void deleteFace(int);
    int getPatchNum() { return patches.size(); }
    const unordered_set<int>& getPatch(int i) { return patches[i]; }
    int getColor(int i) { if (reverseTable.find(i) != reverseTable.end()) return reverseTable[i]; return -1; }
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d>> planar;
    vector<Eigen::Vector3f> cenPos;
    unordered_set<long long int> patchGraphEdges;

private:
    unordered_map<int, int> reverseTable;
    vector<unordered_set<int>> patches;
};

#endif //MESHSIMPLIFICATION_FEATURE_H
