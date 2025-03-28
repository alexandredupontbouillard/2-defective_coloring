
#ifndef separator
#define separator

#include "objscip/objscip.h"
#include "scip/scip.h"
#include "objscip/objsepa.h"
#include "scip/cons_linear.h"
//#include "scip_exception.hpp"
#include <ilcplex/ilocplex.h>
#include "Constraint.h"
#include "Graph.h"
#include <chrono>
#include "SCIP_master.h"
class C_master_coloring;

class Constraint;
struct SCIP_ConsData;

class Separator : public scip::ObjSepa{
	public:
	

	C_master_coloring* M;
	list<Constraint*> ctn;
	int nbConstraints = 0;
	float sepaTime=0;
	int sepaCalled = 0;
	bool shouldSepa=true;
	int nbCtGen;
	int typeOfCoeffDoi=4;


	Separator(SCIP* scip,const char* p_name, C_master_coloring* M_): ObjSepa (scip, p_name, p_name, 1500,1 , 1, FALSE, FALSE){M = M_;}
	//Destructor
	virtual ~Separator(){
	}

	virtual bool separate(SCIP* scip,SCIP_RESULT* result){shouldSepa = false; return SCIP_OKAY;} //should fill ctn
	virtual void init(){return;}

	virtual void setupBranching(SCIP_ConsData data)=0;
	virtual void setoutBranching()=0;

	int addConstraints(SCIP* scip, SCIP_RESULT* result);
	int addConstraints(SCIP* scip,SCIP_SEPA* sepa, SCIP_RESULT* result);	

	SCIP_RETCODE scip_execlp(SCIP*, SCIP_SEPA*, SCIP_RESULT*, unsigned int);
	SCIP_RETCODE scip_execsol(SCIP*, SCIP_SEPA*, SCIP_RESULT*, unsigned int);
	SCIP_RETCODE scip_init(SCIP*,SCIP_SEPA*);
	
};	


class NullSeparator : public Separator{
	public : 
	
	NullSeparator(SCIP* scip,const char* p_name, C_master_coloring* M);
	
	virtual bool separate(SCIP* scip,SCIP_RESULT* result);
	virtual void init();

	virtual void setupBranching(SCIP_ConsData data);
	virtual void setoutBranching();

	virtual ~NullSeparator(){
	}




};



#endif
