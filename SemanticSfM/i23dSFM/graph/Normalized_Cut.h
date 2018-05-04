//
// Created by haibao637 on 17-11-23.
//

#ifndef I23DSFM_NORMALIZED_CUT_H
#define I23DSFM_NORMALIZED_CUT_H
#include<Eigen/Core>
#include<Eigen/Dense>
#include<set>
#include<vector>
#include<i23dSFM/types.hpp>
#include<iostream>
using namespace std;
using namespace Eigen;

namespace i23dSFM{

    namespace graph{

        template <typename Type>
        class Graph{
        public:
            typedef set<Type> Nodes;
            Nodes nodes;
            typedef set<pair<Type ,Type>> Edges;
            Edges edges;
            MatrixXd W;

            Graph(set<Type>& p_nodes,set<pair< Type,Type >>& p_edges):nodes(p_nodes),edges(p_edges){



                W=MatrixXd::Zero(nodes.size(),nodes.size());
                for(int i=0;i<nodes.size();++i){
                    auto iter_i=nodes.begin();
                    advance(iter_i,i);

                    auto value_i=*iter_i;
                    for(int j=i+1;j<nodes.size();++j){
                        auto iter_j=nodes.begin();
                        advance(iter_j,j);

                        auto value_j=*iter_j;
                        if(edges.find(pair<unsigned ,unsigned >(value_i,value_j))!=edges.end()||edges.find(pair<unsigned ,unsigned >(value_j,value_i))!=edges.end()){
                            W(i,j)=1;
                            W(j,i)=1;
                        }else{
                            W(i,j)=0;
                            W(j,i)=0;
                        }
                    }
                }
                cout<<W<<endl;
                MatrixXd D=MatrixXd::Zero(nodes.size(),nodes.size());


                for(int i=0;i<nodes.size();++i){
                    D(i,i)=W.col(i).sum();
                }

                W=D-W;

                for(int i=0;i<nodes.size();++i){
                    D(i,i)=1.0/sqrt(D(i,i));
                }

                W=D*W;
                W=W*D;


            }

            vector<Graph<Type>> Normalized_Cut(){
                vector<Graph<Type>> graph;
                SelfAdjointEigenSolver<MatrixXd> eigen_solver(W);

                MatrixXd vertex=eigen_solver.eigenvectors().col(1);
                MatrixXd dis_v=vertex;
                double best=0;
                double best_nassoc=2;
                for(int i=0;i<vertex.rows();++i){
                    double tmp=vertex(i,0);
                    for(int j=0;j<vertex.rows();++j){
                        dis_v(j,0)= (vertex(j,0) >= tmp);
                    }
                    double assoc_a=0,assoc_b=0,assoc_av=0,assoc_bv=0;
                    for(auto edge:edges){
                        auto iter_first=nodes.find(edge.first);
                        auto first=distance(nodes.begin(),iter_first);
                        auto iter_second=nodes.find(edge.second);
                        auto second=distance(nodes.begin(),iter_second);
                        if(dis_v(first,0)==dis_v(second,0)){
                            if(dis_v(first,0)==0){
                                assoc_b++;
                                assoc_bv++;
                            }else{
                                assoc_a++;
                                assoc_av++;
                            }

                        }else{
                            assoc_bv++;
                            assoc_av++;
                        }
                    }
                    double nassoc=2-assoc_a/assoc_av-assoc_b/assoc_bv;
                    if(nassoc<best_nassoc){
                        best_nassoc=nassoc;
                        best=i;
                    }

                }
                vector<set<Type>> vnodes(2);
                for(int i=0;i<vertex.rows();++i){
                    dis_v(i,0)=(vertex(i,0)>=vertex(best,0));
                    auto iter=nodes.begin();
                    advance(iter,i);
                    vnodes[dis_v(i,0)].insert(*iter);
                }
                cout<<dis_v<<endl;
                vector<set<pair<Type,Type>>> vedges(2);
                for(auto edge:edges){
                    auto iter_first=nodes.find(edge.first);
                    auto first=distance(nodes.begin(),iter_first);
                    auto iter_second=nodes.find(edge.second);
                    auto second=distance(nodes.begin(),iter_second);
                    if(dis_v(first,0)==dis_v(second,0)){


                            vedges[dis_v(first,0)].insert(edge);

                    }
                }
                vector<Graph<Type>> graphs;
                graphs.emplace_back(vnodes[0],vedges[0]);
                graphs.emplace_back(vnodes[1],vedges[1]);
                return graphs;



            }

        };
    }
}

#endif //I23DSFM_NORMALIZED_CUT_H
