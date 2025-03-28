
#ifndef __pricerglout
#define __pricerglout

#include "Pricer_Coloring.h"
#include "Alternance.h"
#include "Separator.h"
#include "BranchingHandler.h"


class GloutPricer : public ObjPricerColoring{
	public:





	GloutPricer(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority,Alternance* alt,Separator* sep_);

	//virtual ~ExactPricer(){}

	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result);
	void setupBranching(SCIP_ConsData data);
	void setoutBranching();


   
};


#endif
