
#ifndef __pricermaxnode
#define __pricermaxnode

#include "Pricer_Coloring.h"
#include "Alternance.h"
#include "BranchingHandler.h"

class MaxNodePricer : public ObjPricerColoring{
	public:


	Cplex_pricing_algo A_cplex;


	MaxNodePricer(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority,int nbNodes);

	//virtual ~ExactPricer(){}

	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result);
	void setupBranching(SCIP_ConsData data);
	void setoutBranching();


   
};


#endif
