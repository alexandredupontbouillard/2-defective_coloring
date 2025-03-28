#ifndef _ExactTriangleSep
#define _ExactTriangleSep


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

class ExactTriangleSep : public Separator{

	public : 
	C_Graph G;
	C_master_coloring* M;
	list<vector<int>*> triangles;
        SCIP* scip;

	ExactTriangleSep( SCIP* scip_, C_master_coloring* _master,const char* p_name, C_Graph G);
	virtual bool separate(SCIP* scip,SCIP_RESULT* result); 
	virtual void init();

        virtual void setupBranching(SCIP_ConsData data);
	virtual void setoutBranching();

	virtual ~ExactTriangleSep(){};



};


















#endif
