#ifndef Graph_H
#define Graph_H

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <math.h>
#include <unistd.h>
#include <utility>
//#include "BranchingHandler.h"
using namespace std;


class C_Graph;

struct SCIP_ConsData {
  
  
	int v1; 
	int v2;
	bool same; // true for merging v1 v2 and false for adding fake edge   
	struct SCIP_ConsData* father_data; // Pointer on the data of the father node

};

/****************************  Stable Set ****************************/

class StableSet{
public :
	list<int> L_nodes;
	vector<int> nodes;
	C_Graph* G;

	
	bool contains(int n);
	bool add(int n);
	
	void initialize(int nb_nodes);

	int size();

	void print();



};


/****************************  C_edge  *******************************/
class C_link{
public:
  int num;      // Number of the edge
  int v1, v2;   // The two extremities of an edge v1v2 or of an arc (v1,v2)
  float weight;
  
  // return the extremity disctinc from v in O(1).
  int return_other_extrem(int v);
  
};


/***************************  C_node  *****************************/
class C_node{
public :
   int num;     // Number of the node
   float weight;
   
   list<C_link*> L_adjLinks;

   //Test if j is a neighbour of i in O(degre(i))
   bool test_neighbour(int j);

   //Test if j is a successor of i in O(degre(i))
   bool test_successor(int j);
   
   list<int> same;

};


/**************************  C_Graph  ******************************/
class C_Graph{
public:

  bool directed;  // True if directed / False if undirected
  int nb_nodes;   // Number of nodes
  int nb_links;   // Number of links
  vector<vector<int>> adjacency;  //
vector<vector<int>> adjacency2;
  // Encoding of the graph by adjacence list, i.e. a vector of list of edges 
  vector <C_node> V_nodes;

  // Additional encoding: a vector on the edges (on pointers over edges)
  vector <C_link*> V_links;

 int edgeNumber(int v1 , int v2);
  /*********************************************/
  /*********** ALGORITHMS **********************/
  
  bool detect_cycle(vector<int>&sol);
  bool verifyChord(list<int> cycle);
  pair<float,int> ajouteSommetAuCycle(list<int>* cycle,vector<StableSet*>* stables , vector<int>* ss,vector<float>& cost ,  int v1, int v2, float placedispo,vector<int>* sommet_interdit);
  void findViolatedCycleIq(vector<StableSet*>* stables, vector<float>& cost,list<list<int>*> &result, int rhs);
  // to get max stable set from stable set (greedy)
  void completeIntoMaxStable(StableSet* stable);

  bool verifyChord(list<int>* cycle,int v1,int v2,int pred1, int pred2);
  void getGloutColoring(list<StableSet*>& coloration);
  void deleteVertex(int i);
  void cleanGraph();
  C_Graph  getComplementary();
  
  bool respectBranching(StableSet s);

  //list<list<int>>* findViolatedCycleIq(vector<StableSet*> stables, vector<float> cost);   

   list<int> getMaximalClique(int v1, int v2);
  /*********************************************/
  /*********** INPUT-OUTPUT FILES **************/
  
  // Read a DIMACS file and store the corresponding graph in C_Graph
  void read_undirected_DIMACS(istream & fic);

  // Read a TSP file and store the corresponding graph in C_Graph
  void read_undirected_TSP(istream & fic);
  
  // Read a directed "gra" format file and store the corresponding graph in C_Graph
  void read_directed_GRA(istream & in);
  bool isItCovering(list<list<int>> sol);
  // Write a Graphviz File with the DOT format
  void write_dot_G(string InstanceName);
  
  // Write a Graphviz File with the DOT format using an incidence vector of a stable set
  void write_dot_G_stableset(string InstanceName, vector<int> &stable);

  // Write a Graphviz File with the DOT format using a coloration vector
  void write_dot_G_color(string InstanceName, vector<int> &coloring);
  StableSet* findMaxStableSet(vector<pair<int,float>> weight);



  list<pair<int,list<int>>*> getDualOptCouple();  //  for each node w, add DOIs such that w is adjacent to T and T is max stable  (unsorted) adding also the one satisgying |T| = 1
  list<pair<int,list<int>>*> getDualOptCouple2(); // all (w,{t}) such that w is adjacent to t
  list<pair<int,list<int>>*> getDualOptCouple3(); // for each node w, add DOIs such that w is adjacent to T and T is max stable  (sorted)
  list<pair<int,list<int>>*> getDualOptCouple4(); // each (w, {t1,t2}) couple possible such w is adjacent to t1,t2
  list<pair<int,list<int>>*> getDualOptCouple5(); // each (w,{t}), w adjacent to t or not but only add the vertex of largest neighborhood
  list<pair<int,list<int>>*> getDualOptCouple6(); // each (w,{t}), w adjacent to t or not 
  list<pair<int,list<int>>*> getDualOptCouple7(); // each (w,T) with w not necessarily complete to T and T is max stable () adding also the one satisgying |T| = 1
  
  
  list<list<int>> getMaxStableOfNeighborhoodIncludedNONADJ(int k);
  list<int> getNeighborhoodIncludedNONADJ(int k);
  
  list<list<int>> getMaxStableOfNeighborhoodIncluded(int k);
  list<list<int>> getMaxStableOfNeighborhoodIncluded2(int k);

  list<int> getNeighborhoodIncluded(int k);
	bool isCo2plex(list<int> s);
  void modifyGraphInBranching(SCIP_ConsData* data);
  void resetGraphBranching();
  void propagateBranchingOverSameDiffer(int v1, int v2 );
  void propagateBranchingOverSameSame(int v1,int v2 );
  
};

void computeCostOfVertice(vector<StableSet*>* stables , vector<int>*  ss,vector<float>& cost, int v1,pair<float,list<int>>* m);
void majSS(list<int>* indices, vector<int>* ss);
bool cycleIn(list<list<int>*>& l1, list<int>* l2);
bool compareCycle(list<int>* l1, list<int>* l2);
int in(list<int> cycle, int vertice);
#endif
