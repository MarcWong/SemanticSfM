//
// Created by Haowen Xu on 9/29/16.
//

#include "Simplify.h"
#include <map>
#include <set>
#include "Eigen/Dense"
#include "Eigen/SVD"
#include <iostream>
#include "BilateralNormalFilter.h"
using namespace std;
using namespace Eigen;

inline pair<int,int> ledge(int a, int b)
{
	if (a < b) return make_pair(a, b);
	else return make_pair(b, a);
}
inline long long int lcedge(int a,int b)
{
	if (a < b) return (((long long int)a << 32) + b);
	else return (((long long int)b << 32) + a);
}

inline long long int ones()
{
	return ((long long int)1 << 32) - 1;
}

void AlgSimplify::init(TriMesh* inMesh)
{
	mesh = inMesh;
	// delete heap;
	heap = new MxHeap();
	gNormal.clear();
	mp.clear();
}

void AlgSimplify::getQ()
{
	mp.clear();
	/*
	while(heap.size() > 0)
		heap.drop();
	*/
	FACESET &faces = mesh->FaceSet();
	Point3D target;
	for (auto cur = faces.begin(); cur != faces.end(); cur++)
	{
		for (int i = 0; i < 3; i++)
		{
			//pair<int,int> edge = ledge(cur->second.index[i],cur->second.index[(i+1)%3]);
			long long int edge = lcedge(cur->second.index[i], cur->second.index[(i + 1) % 3]);
			if (mp.find(edge) == mp.end())
            {
				double cost = getCost(edge >> 32, edge&(ones()), target);
				//remember to release memory
				SimplifyNode* node = new SimplifyNode(edge >> 32, edge&(ones()), cost, target);
				heap->insert(node);
				//mp[ledge(cur->second.index[i],cur->second.index[(i+1)%3])] = node;
				mp[lcedge(cur->second.index[i], cur->second.index[(i + 1) % 3])] = node;
			}
		}
	}
}

double AlgSimplify::getCost(int a, int b, Point3D& target)
{
	double ret = 0;
	Matrix4d M = Eigen::Matrix4d::Zero(4, 4), M2 = Eigen::Matrix4d::Zero(4, 4);
	//get a,b's adj faces;
	//multiSet
	unordered_set<int> faces;
	PTSET &pts = mesh->PointSet();
	FACESET &fcs = mesh->FaceSet();
	for (int i = 0; i < pts[a].adjFaces.size(); i++)
		faces.insert(pts[a].adjFaces[i]);
	for (int i = 0; i < pts[b].adjFaces.size(); i++)
		faces.insert(pts[b].adjFaces[i]);
	vector<int> oldNormals;
	for (auto u = faces.begin(); u != faces.end(); u++)
	{
		Point3D nor = (pts[fcs[*u].index[0]].pos - pts[fcs[*u].index[1]].pos) ^ (pts[fcs[*u].index[0]].pos - pts[fcs[*u].index[2]].pos);
		if(nor.getLen() == 0)
			continue;
		nor.normalize();
		Point3D midPt = (pts[fcs[*u].index[0]].pos + pts[fcs[*u].index[1]].pos + pts[fcs[*u].index[2]].pos) / 3;
		Vector4d v2(nor.x, nor.y, nor.z, -nor.x * (midPt.x) - nor.y * (midPt.y) - nor.z * (midPt.z));

		Point3D guideNormal = gNormal[(*u)];
		//cout<<guideNormal.x<<" "<<guideNormal.y<<" "<<guideNormal.z<<endl;
		if (guideNormal.getLen() == 0)
			continue;
		guideNormal.normalize();

		//guideNormal = nor;

		midPt = (pts[fcs[*u].index[0]].pos + pts[fcs[*u].index[1]].pos + pts[fcs[*u].index[2]].pos) / 3;
		Vector4d v(guideNormal.x, guideNormal.y, guideNormal.z, -guideNormal.x * (midPt.x) - guideNormal.y * (midPt.y) - guideNormal.z * (midPt.z));
		/*
		cout<<v<<endl<<endl;
		cout<<v2<<endl;
		*/
		M = M + ((v2 * v2.transpose()) * mesh->getArea(*u)) * 0.2 + ((v * v.transpose()) * mesh->getArea(*u)) * 0.8;
	}

	//±ß½çÏî

	bool isBoundary = false;
	if (mesh->isBoundaryVertex(a) || mesh->isBoundaryVertex(b))
    {
		isBoundary = true;
		if (mesh->isBoundaryVertex(a))
        {
			ADJSET st = mesh->getAdjVertexOfVertex(a);
			for (auto u = st.begin(); u != st.end(); ++u)
            {
				if (mesh->isBoundaryEdge(*u, a))
                {
					unordered_map<int,int> face_mp;
					for (int i = 0; i<pts[a].adjFaces.size(); i++)
						face_mp[pts[a].adjFaces[i]] = 1;
					for(int i = 0; i < pts[*u].adjFaces.size(); i++)
						if (face_mp[pts[*u].adjFaces[i]]==1)
                        {
							Point3D normal = fcs[pts[*u].adjFaces[i]].normal ^ (pts[*u].pos - pts[a].pos);
							normal.normalize();
							Vector4d v(normal.x,normal.y,normal.z, -normal.x * (pts[a].pos.x) - normal.y * (pts[a].pos.y) - normal.z * (pts[a].pos.z));
							M2 = M2 + v * v.transpose() * mesh->getArea(pts[*u].adjFaces[i]);
						}
				}
			}
		}

		if (mesh->isBoundaryVertex(b))
        {
			ADJSET st = mesh->getAdjVertexOfVertex(b);
			for (auto u = st.begin(); u != st.end(); ++u)
            {
				if (mesh->isBoundaryEdge(*u, b))
                {
					unordered_map<int,int> face_mp;
					for (int i = 0; i < pts[b].adjFaces.size(); i++)
						face_mp[pts[b].adjFaces[i]] = 1;
					for( int i = 0; i < pts[*u].adjFaces.size(); i++)
						if (face_mp[pts[*u].adjFaces[i]] == 1)
                        {
							Point3D normal = fcs[pts[*u].adjFaces[i]].normal ^ (pts[*u].pos - pts[b].pos);
							normal.normalize();
							Vector4d v(normal.x, normal.y, normal.z, -normal.x * (pts[b].pos.x) - normal.y * (pts[b].pos.y) - normal.z * (pts[b].pos.z));
							M2 = M2 + v * v.transpose() * mesh->getArea(pts[*u].adjFaces[i]);
						}
				}
			}
		}

		if (mesh->isBoundaryEdge(a,b))
        {
			unordered_map<int,int> face_mp;
            for (int i = 0; i < pts[b].adjFaces.size(); i++)
                face_mp[pts[b].adjFaces[i]] = 1;
            for (int i = 0; i < pts[a].adjFaces.size(); i++)
                if (face_mp[pts[a].adjFaces[i]] == 1)
                {
                    Point3D normal = fcs[pts[a].adjFaces[i]].normal ^ (pts[a].pos - pts[b].pos);
                    normal.normalize();
                    Vector4d v(normal.x, normal.y, normal.z, -normal.x * (pts[b].pos.x) - normal.y * (pts[b].pos.y) - normal.z * (pts[b].pos.z));
                    M2 = M2 - v * v.transpose() * mesh->getArea(pts[a].adjFaces[i]);
                }
		}
	}

	if (isBoundary)
		M = M * 0.1 + M2 * 0.9;

	/*svd*/

	Eigen::Matrix3d A;
	Eigen::Vector3d f(-M(0, 3), -M(1, 3), -M(2, 3));
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			A(i, j) = M(i, j);


	Eigen::JacobiSVD<Eigen::Matrix3Xd> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
	Eigen::Matrix3d m1(Eigen::Matrix3d::Zero(3, 3));
	m1(0, 0) = fabs(svd.singularValues()[0] / svd.singularValues()[0]) > 1e-3 ? 1.0 / svd.singularValues()[0] : 0;
	m1(1, 1) = fabs(svd.singularValues()[1] / svd.singularValues()[0]) > 1e-3 ? 1.0 / svd.singularValues()[1] : 0;
	m1(2, 2) = fabs(svd.singularValues()[2] / svd.singularValues()[0]) > 1e-3 ? 1.0 / svd.singularValues()[2] : 0;


	Point3D _x = (pts[a].pos + pts[b].pos) / 2;
	Eigen::Vector3d Eigen_x(_x.x,_x.y,_x.z);
	Eigen::Vector3d newx = Eigen_x + svd.matrixV() * m1 * svd.matrixU().transpose()*(f - (A*Eigen_x));
	/*
	if(m1(2,2) == 0)
		newx = Eigen_x;
	*/
	target.x = newx.x();
	target.y = newx.y();
	target.z = newx.z();

	Eigen::Vector4d vb(newx.x(), newx.y(), newx.z(), 1);
	// error
	ret = ret + vb.transpose() * M * vb;

	//normal error.
	double mincos = 1;
	for (auto u = faces.begin(); u != faces.end(); u++)
	{
		Point3D oldNormal = (pts[fcs[*u].index[0]].pos - pts[fcs[*u].index[1]].pos) ^ (pts[fcs[*u].index[0]].pos - pts[fcs[*u].index[2]].pos);
		if(oldNormal.getLen() == 0)
			continue;
		oldNormal.normalize();

		//¸ÄÓÃguide normal

		//ÔÝÊ±ÓÃguideÊÔÊÔ£¬Ò²¿ÉÒÔÓÃÔ­·¨Ïò£»
		/*
		Point3D oldNormal = gNormal[(*u)];
		if(oldNormal.getlen() == 0)
			continue;
		oldNormal.norm();
		*/
		int cntab = 0;
		Point3D newFace[3];
		for (int i = 0; i < 3; i++)
			if(fcs[*u].index[i] == a)
            {
                newFace[i] = target;
				cntab++;
			}
			else if(fcs[*u].index[i] == b)
            {
                newFace[i] = target;
				cntab++;
			}
			else
                newFace[i] = pts[fcs[*u].index[i]].pos;
		Point3D newNormal = (newFace[0] - newFace[1]) ^ (newFace[0] - newFace[2]);
		if (newNormal.getLen() == 0)
			continue;
		newNormal.normalize();

		if (cntab < 2)
        {
			mincos = min(mincos, newNormal * oldNormal);
		}
	}

	//triQuality
	double minquality = 1;
	for (auto u = faces.begin(); u != faces.end(); u++)
	{
		int cntab = 0;
		Point3D newFace[3];
		for (int i = 0; i < 3; i++)
			if (fcs[*u].index[i] == a)
            {
                newFace[i] = target;
				cntab++;
			}
			else if (fcs[*u].index[i] == b)
            {
                newFace[i] = target;
				cntab++;
			}
			else
                newFace[i] = pts[fcs[*u].index[i]].pos;
		double maxLen = 0;
		for (int i = 0; i < 3; i++)
			maxLen = max(maxLen, (newFace[(i + 1) % 3] - newFace[i]).getLen());

		double S = ((newFace[0] - newFace[1]) ^ (newFace[0] - newFace[2])).getLen();

		if (cntab < 2)
        {
			minquality = min(minquality, S / maxLen / maxLen);
		}

	}
	//ret = ret + (pts[a].pos - pts[b].pos).getlen() * 0.05;
	//cout<<"ret:"<<ret<<endl;
	if (mincos < 0)
    {
		mincos = 0;
		//ret /= fabs((mincos +1.000001)/2);
		//cout<<"cos: "<<mincos<<":"<<ret<<endl;
	}
	if(minquality < 0.3)
    {
		minquality = 0.3;
		//ret /= (minquality+1e-8);
		//cout<<"quality: "<<minquality<<":"<<ret<<endl;
	}
	ret = ret / ((mincos + 1.000000001) / 2) / (minquality + 1e-8);
	//ret = ret / (minquality + 1e-8);
	//ret = ret / ((mincos+1.000000001)/2);
	//length;
	//ret = ret + (pts[a].pos - pts[b].pos).getlen() * 0.05;

	return -ret;
}


bool AlgSimplify::contract()
{
	PTSET &pts = mesh->PointSet();
	FACESET &fcs = mesh->FaceSet();
	Point3D target;
	SimplifyNode * nd = nullptr;
	while(heap->size() > 0)
	{
		nd = (SimplifyNode *) heap->top();

		if (pts.find(nd->index1) != pts.end() && pts.find(nd->index2) != pts.end())
			break;
		else
        {
			//mp.erase(ledge(nd->index1,nd->index2));
			mp.erase(lcedge(nd->index1, nd->index2));
			heap->remove(nd);
		}
	}

	if (nd != nullptr && mesh->contract(nd->index1, nd->index2, nd->targetPos))
    {
		//pts[nd.index2].pos = nd.targetPos;
		//cout<<"point:"<<nd->index1<<" "<<nd->index2<<endl;
		//cout<<nd->heap_key()<<endl;
		for (int j = 0; j < pts[nd->index2].adjFaces.size(); j++)
		{
			for (int i = 0; i < 3; i++)
			{
				//ÐÞ¸Ä¶ÑÀïÄÚÈÝ
				int tmpIndex = fcs[pts[nd->index2].adjFaces[j]].index[i];
				//cout<<"point:"<<nd->index2<<" "<<tmpindex<<endl;
				if (tmpIndex != nd->index2)
				{
					//cout<<"S"<<endl;
					double cost = getCost(tmpIndex, nd->index2, target);
					//cout<<"E"<<endl;
					//pair<int,int> edge = ledge(tmpindex,nd->index2);
					long long int edge = lcedge(tmpIndex, nd->index2);
					if (mp.find(edge) == mp.end())
                    {
						SimplifyNode* newNode = new SimplifyNode(edge >> 32, edge&(ones()), cost, target);
						heap->insert(newNode);
						mp[edge] = newNode;
						//»ØÀ´½Ó×Åµ÷
					}
					else
                    {
						SimplifyNode * tmp = mp[edge];
						tmp->targetPos = target;
						tmp->heap_key(cost);
						heap->update(tmp);
					}
				}
			}
		}
		//mp.erase(ledge(nd->index1,nd->index2));
		mp.erase(lcedge(nd->index1, nd->index2));
		heap->remove(nd);
		return true;
	}
	else
    {
		if (nd != nullptr)
        {
			//mp.erase(ledge(nd->index1,nd->index2));
			mp.erase(lcedge(nd->index1,nd->index2));
			heap->remove(nd);
		}
		return false;
	}
}

void AlgSimplify::contractOne()
{
	while(!contract());
}


void AlgSimplify::filter()
{
	gNormal.clear();
	AlgBilateralNormalFilter filter;
	filter.Init(mesh);
	FACESET &faces = mesh->FaceSet();
	unordered_map<int,Point3D> tmp;
	for (auto u = faces.begin(); u != faces.end(); ++u)
    {
		tmp[(*u).first] = u->second.normal;
	}
	filter.BilateralNormalSmoothing();
	for (auto u = faces.begin(); u != faces.end(); ++u)
    {
		gNormal[u->first] = u->second.normal;
		//cout<<gNormal[(*u).first].x<<" "<<gNormal[(*u).first].y<<" "<<gNormal[(*u).first].z<<endl;
	}

	for (auto u = faces.begin(); u != faces.end(); ++u)
    {
		u->second.normal = tmp[u->first];
	}
}