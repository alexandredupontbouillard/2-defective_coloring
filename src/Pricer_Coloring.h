#ifndef __SCIP_PRICER_COLORING_
#define __SCIP_PRICER_COLORING_

#include "objscip/objscip.h"
#include "scip/pub_var.h"

#include <vector>
#include <list>
#include <chrono>

#include "Graph.h"
#include "SCIP_master.h"
#include "Alternance.h"
#include "Separator.h"
#include "BranchingHandler.h"
using namespace std;
using namespace scip;


class Alternance;
class Separator;

/** pricer class */
class ObjPricerColoring : public ObjPricer{
	public:

	char* NAME;
	C_Graph *G;
	C_master_coloring *M;
	Alternance* a;
	Separator* sep;

	double pricingTime=0;
	double separatingTime=0;
  	int nbColumns=0;
	int nbConstraints=0;
	int pricingCalled = 0;
	int sepaCalled = 0;
    bool sepapa = false;

	bool separate(SCIP* scip, SCIP_RESULT* result);
	bool isSSnegativeRC(SCIP* scip, StableSet* stable);

	void addColumn(SCIP* scip,StableSet* s);
   /** Destructs the pricer object. */
   
    

	ObjPricerColoring(SCIP* scip, const char* p_name,int priority, C_master_coloring *M);

	virtual SCIP_DECL_PRICERINIT(scip_init);
	virtual SCIP_DECL_PRICERREDCOST(scip_redcost);

   	virtual ~ObjPricerColoring(){}
	virtual StableSet* coloring_pricing(SCIP* scip, SCIP_RESULT* result){return NULL;}
	virtual void setupBranching(SCIP_ConsData data)=0;
	virtual void setoutBranching()=0;
	


   
};

#endif
