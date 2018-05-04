//
// Created by Haowen Xu on 9/28/16.
//

#include "Feature.h"

bool FeaturePatch::addFaceToPatch(int faceName, int i)
{
    if (patches.size() <= i)
    {
        return false;
    }

    reverseTable[faceName] = i;
    patches[i].insert(faceName);
    return true;
}

int FeaturePatch::addPatch(unordered_set<int> patch)
{
    int num = patches.size();
    for (auto i = patch.begin(); i != patch.end(); ++i)
    {
        reverseTable[*i] = num;
    }
    patches.push_back(patch);
    return patches.size() - 1;
}

int FeaturePatch::addPatch(vector<int> patch)
{
    unordered_set<int> pp;
    for (int i = 0; i < patch.size(); i++)
    {
        pp.insert(patch[i]);
    }
    addPatch(pp);
    return patches.size() - 1;
}

void FeaturePatch::deleteFace(int i)
{
    patches[reverseTable[i]].erase(i);
    reverseTable.erase(i);
}