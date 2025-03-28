#ifndef ODDHOLE
#define ODDHOLE

#include <ilcplex/ilocplex.h>
#include <vector>
#include <list>

//#include "constraints.h"
#include "Graph.h"


#include "Find_Stable_set_with_Cplex.h"
#include "objscip/objscip.h"
#include "technique.h"


using namespace std;

class Cplex_Noproj_algo;
class Cplex_pricing_algo;

class Constraint{ //abstract class wich has a method to change the pricer every time we add inequalities in master problem

	public :
	unsigned int rhs; 
	
	virtual IloNumVar* modifyPricer(Cplex_pricing_algo* pricer) = 0;
	virtual int coeffOfStable(StableSet* s)=0;
	virtual vector<SCIP_Real>* getLinearization()=0;
	virtual ~Constraint(){}
	virtual int coeffDoi(int w, list<int> T , int typeOfCoeff)=0;
};






class ChvatalGomoryCst : public Constraint { 
	public : 
	unsigned int nb_nodes;
	
	vector<SCIP_Real>* u;
	
	ChvatalGomoryCst(vector<SCIP_Real>* u_,int nb_nodes_);
	IloNumVar* modifyPricer(Cplex_pricing_algo* pricer);  
	int coeffOfStable(StableSet* s);
	vector<SCIP_Real>* getLinearization(){return u;}
	int coeffDoi(int w, list<int> T, int typeOfCoeff);
};

class triangleCst : public Constraint {
	public :
	
	int u;
	int v;
	int w;
	list<int> tt;
	int nb_nodes;
	triangleCst(int u_,int v, int w, int nb_nodes);
	IloNumVar* modifyPricer(Cplex_pricing_algo* pricer);  
	int coeffOfStable(StableSet* s);
	vector<SCIP_Real>* getLinearization();
	int coeffDoi(int w, list<int> T, int typeOfCoeff);
	IloNumVar* modifyPricer(Cplex_Noproj_algo* pricer);
	virtual ~triangleCst(){}
	

};

#endif
