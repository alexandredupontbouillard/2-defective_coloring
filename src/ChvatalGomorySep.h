#ifndef __ChvatalSep
#define __ChvatalSep


#include "objscip/objscip.h"
#include "scip/scip.h"
#include "objscip/objsepa.h"
#include "scip/cons_linear.h"
//#include "scip_exception.hpp"
#include <ilcplex/ilocplex.h>
#include <vector>
#include <chrono>
#include <list>
#include <algorithm>

#include "Graph.h"
#include "SCIP_master.h"
#include "Constraint.h"
#include <algorithm>
#include "technique.h"
#include "Separator.h"
#include "BranchingHandler.h"
class C_master_coloring;

class ChvatalGomorySep : public Separator{

	public : 
	IloEnv   env;
  	IloModel model;
  	IloObjective obj;
  	IloCplex cplex;
      
  	IloNumVarArray u;
	IloNumVarArray big_u;
	IloNumVarArray f;
	IloNumVarArray alpha;
	IloNumVar alpha_0;
	IloNumVar f_0;

	IloNumVarArray fDOI;
	IloNumVarArray alphaDOI;

	int nbMaxNode;
	IloConstraint precision;
	int called;
	int nbColumnAtLastIteration;
	int nb_nodes;
	int typeOfCoeffDoi;

	ChvatalGomorySep(SCIP* scip,C_master_coloring* _master,const char* p_name, int nb_nodes_, int maxNode, int typeOfCoeffDoi);
	virtual bool separate(SCIP* scip,SCIP_RESULT* result); 
	virtual void init();

        virtual void setupBranching(SCIP_ConsData data);
	virtual void setoutBranching();

	virtual ~ChvatalGomorySep(){}



};


#endif
