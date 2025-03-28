#ifndef cplexnoproj_algo

#define cplexnoproj_algo

#include <ilcplex/ilocplex.h>
#include <vector>
#include <list>
#include "objscip/objscip.h"
#include "Graph.h"
#include "Constraint.h"


#include "BranchingHandler.h"

//#include "Cplex_proj_algo.h"

//class Cplex_pricing_algo;
#include "Find_Stable_set_with_Cplex.h"


using namespace std;



class Cplex_Noproj_algo : public Cplex_pricing_algo{
	public:
	unsigned int nbIt = 0;
	int nb_additional_variable=0;
	C_Graph *G;
  
	//IloEnv   env;
	//IloModel model;
	//IloObjective obj;
	//IloCplex cplex;
      
	//IloNumVarArray x;
	IloNumVarArray y;
 	IloNumVarArray additional;   // la liste des variables ajoutées dans la fonction objectif

	IloConstraintArray branchingCt;
	
	IloConstraint bound;
	int nbBranchingDecision;

	 int edgeNumber(int v1 , int v2);
	virtual void create_MIP(C_Graph *G,int maxNodes);

	 virtual void set_objective_coefficient(const vector<SCIP_Real>& obj_coeff, const vector<SCIP_Real>& additional_coeff);

	 virtual void modify_pricer(list<Constraint*> constraint);
  
	// Launch Cplex solver and get back an optimal stable set 
	virtual  bool find_stableset(StableSet* stable, SCIP_Real& objvalue);

	 virtual void setupBranching(SCIP_ConsData data);
	 virtual void setoutBranching();
	 
	 


	


};

#endif
