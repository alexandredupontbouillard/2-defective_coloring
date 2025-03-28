
#ifndef __pricerexactproj
#define __pricerexactproj

#include "Pricer_Coloring.h"
#include "Alternance.h"
#include "BranchingHandler.h"
#include "Cplex_proj_algo.h"

class ExactPricerProj : public ObjPricerColoring{
	public:


	Cplex_pricing_algo A_cplex;


	ExactPricerProj(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority);

	//virtual ~ExactPricer(){}

	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result);
	void setupBranching(SCIP_ConsData data);
	void setoutBranching();


   
};


#endif
