
#ifndef __pricermaxnodeProj
#define __pricermaxnodeProj

#include "Pricer_Coloring.h"
#include "Alternance.h"
#include "BranchingHandler.h"
#include "Cplex_proj_algo.h"

class MaxNodePricerProj : public ObjPricerColoring{
	public:


	Cplex_pricing_algo A_cplex;


	MaxNodePricerProj(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority,int nbNodes);

	//virtual ~ExactPricer(){}

	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result);
	void setupBranching(SCIP_ConsData data);
	void setoutBranching();


   
};


#endif
