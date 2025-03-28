
#ifndef __pricerexact
#define __pricerexact

#include <cstdlib>
#include "Pricer_Coloring.h"
#include "Alternance.h"
#include "BranchingHandler.h"
#include "objscip/objscip.h"
#include "scip/scip.h"
#include "objscip/objsepa.h"
#include "scip/cons_linear.h"
#include <ilcplex/ilocplex.h>
#include "Constraint.h"
#include "Graph.h"
#include <chrono>
#include "SCIP_master.h"
class ExactPricer : public ObjPricerColoring{
	public:

    bool activate_recovery;
	Cplex_pricing_algo A_cplex;

    
	void recovery(SCIP* scip);
	ExactPricer(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority);

	//virtual ~ExactPricer(){}

	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result);
	void setupBranching(SCIP_ConsData data);
	void setoutBranching();


   
};


#endif
