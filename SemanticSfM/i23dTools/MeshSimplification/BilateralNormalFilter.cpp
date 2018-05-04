//
// Created by Haowen Xu on 9/29/16.
//

#include "BilateralNormalFilter.h"
#include <map>
#include <set>
#include <iostream>

void AlgBilateralNormalFilter::computeAreaAndLength()
{
	int edgeNum = 0;

	for(auto u = pMesh->FaceSet().begin(); u != pMesh->FaceSet().end();u++)
	{
		avgeLen += ((pMesh->PointSet())[u->second.index[0]].pos - (pMesh->PointSet())[u->second.index[1]].pos).getLen();
		avgeLen += ((pMesh->PointSet())[u->second.index[2]].pos - (pMesh->PointSet())[u->second.index[1]].pos).getLen();
		avgeLen += ((pMesh->PointSet())[u->second.index[0]].pos - (pMesh->PointSet())[u->second.index[2]].pos).getLen();
        edgeNum += 3;
	}
	avgeLen = avgeLen / (1.0 * edgeNum);
}

bool AlgBilateralNormalFilter::Init(TriMesh* mMesh)
{
	faceNormal.clear();
	dualVertexPos.clear();
	pMesh = mMesh;
	computeAreaAndLength();
	pMesh->computeNormal();
	faceNormal.clear();
	for (auto u = pMesh->FaceSet().begin(); u != pMesh->FaceSet().end(); u++)
		faceNormal[u->first] = (u->second).normal;
	return true;
}

bool AlgBilateralNormalFilter::BilateralNormalSmoothing()
{
	if(pMesh == NULL)
		return false;
	computeDualPosition();
	for (int i = 0; i < normalIteration; ++i)
	{
		stepNormalFiltering();
	}
	for (auto u = pMesh->FaceSet().begin(); u!=pMesh->FaceSet().end(); u++)
    {
		if (faceNormal.find(u->first) == faceNormal.end())
			cout << "error" << endl;
		faceNormal[(*u).first].normalize();
		u->second.normal = faceNormal[(*u).first];
	}
	return true;
}

void AlgBilateralNormalFilter::computeDualPosition()
{
	for (auto u = pMesh->PointSet().begin(); u != pMesh->PointSet().end(); u++)
		for (int j = 0; j < (u->second).adjFaces.size(); j++)
			dualVertexPos[u->second.adjFaces[j]] = dualVertexPos[u->second.adjFaces[j]] + u->second.pos;

	for (auto u = dualVertexPos.begin(); u != dualVertexPos.end(); u++)
		dualVertexPos[u->first] = dualVertexPos[u->first] / 3.0;
}

double AlgBilateralNormalFilter::faceWeight(int f, int nbr)
{
	//int i = f * 3, j = nbr * 3;
	Point3D f1 = faceNormal[f];//new Vector3d(mesh.FaceNormal, i);
	Point3D f2 = faceNormal[nbr];//new Vector3d(mesh.FaceNormal, j);
	Point3D v1 = dualVertexPos[f];// Vector3d(mesh.DualVertexPos, i);
	Point3D v2 = dualVertexPos[nbr];//new Vector3d(mesh.DualVertexPos, j);

	double w = 1;
	double ss = pMesh->getArea(nbr);//triArea[nbr];
	double eij = (v1 - v2).getLen();

	w = ss;

	double x = (f1 - f2).getLen();// 1 - tcos;
	//double l = opt.useSpatialWeights ? eij : 0;
	double l = eij;
	//double spatial = opt.useAdaptiveSpatialDeviation ? perFaceSpatialDist[f] : avgeLen;
	double spatial = avgeLen;

	double d2 = gs * gs, d1 = gc * gc * spatial * spatial;


	double e = w * exp(-l * l / d1 / 2);// *Math.Exp(-x * x / d2 / 2); /--> modified here

	if (useSimilarityWeights)
	{
		e = e * exp(-x * x / d2 / 2);
	}

	return e;
}


void AlgBilateralNormalFilter::stepNormalFiltering()
{
	int fn = pMesh->FaceSet().size();
	double r = 1 - smoothNess;
	set<int> fSet;
	unordered_map<int,Point3D> updatedNormal;
	for (auto u = pMesh->FaceSet().begin(); u != pMesh->FaceSet().end(); ++u)
	{
		for (int j = 0; j < 3; ++j)
        {
			int c = u->second.index[j];
			for (int f = 0; f < pMesh->PointSet()[c].adjFaces.size(); f++)
				fSet.insert(pMesh->PointSet()[c].adjFaces[f]);
		}
		fSet.erase(u->first);

		double sum = 0;
		int count = 0;
		vector<double>w;
		w.resize(fSet.size());
		//double[] w = new double[fSet.Count];
		for (set<int>::iterator iter = fSet.begin(); iter != fSet.end(); iter++)
		{
			w[count] = faceWeight(u->first, *iter);
			sum += w[count++];
		}
		count = 0;
		if (!(sum > -1e-8 && sum < 1e-8))
		{
			for (set<int>::iterator iter = fSet.begin(); iter != fSet.end(); iter++)
			//foreach (int f in fSet)
			{
				Point3D nf = faceNormal[*iter];
				double e = w[count++] / sum; // »¹Òª * flapWeight[i]£¬flapWeight[i] = 1.0;
				updatedNormal[u->first] = updatedNormal[u->first] + nf * e;
			}
		}
		else
		{
			updatedNormal[u->first] = faceNormal[u->first];
		}
		fSet.clear();
	}
	for (auto u = pMesh->FaceSet().begin(); u != pMesh->FaceSet().end(); u++)
	//for (int i = 0; i < fn; ++i)
	{
		updatedNormal[u->first].normalize();
		faceNormal[u->first] = updatedNormal[u->first];
	}
}
