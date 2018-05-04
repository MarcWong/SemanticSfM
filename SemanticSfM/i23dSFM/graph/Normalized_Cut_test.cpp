//
// Created by haibao637 on 17-11-23.
//



#include<fstream>
#include<vector>
#include<iterator>

#include "Normalized_Cut.h"
using namespace i23dSFM::graph;
int main(int argc,char** argv){
    ifstream fin(argv[1]);


    set<unsigned > nodes;
    set<pair<unsigned ,unsigned>> edges;
    int x,y;
    while(fin>>x>>y){
        edges.emplace(pair<unsigned,unsigned>(x,y));
        nodes.emplace(x);
        nodes.emplace(y);
    }

    i23dSFM::graph::Graph<unsigned> graph(nodes,edges);
    graph.Normalized_Cut();
}
