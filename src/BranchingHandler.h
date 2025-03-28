#ifndef BranchingHandlerHl
#define BranchingHandlerHl

#include "Graph.h"
#include "Pricer_Coloring.h"
#include "Separator.h"
#include "SCIP_master.h"
#include <list>
#include "scip/scip.h"
#include "scip/cons_linear.h"
#include "objscip/objscip.h"
using namespace std;


class ObjPricerColoring;

// Current data of a node in the search tree


void create_CteBranch(SCIP* scip, bool same, int v1, int v2,  SCIP_ConsData *father_data, SCIP_CONS** cons);


class BranchingHandler : public scip::ObjConshdlr {

 public :
  
  C_Graph *G;
  list<ObjPricerColoring*> list_pricer;
  list<Separator*> list_separator;
  C_master_coloring* M;
  
 BranchingHandler(SCIP* scip,  C_Graph *GG, list<ObjPricerColoring*> list_pricerr , list<Separator*> list_separatorr  , C_master_coloring* M_);

  
  
     //*  Activation d'un noeud (appelée à chaque fois qu'on rentre dans un noeud)
   
    virtual SCIP_RETCODE scip_active(
	SCIP * 	scip,
	SCIP_CONSHDLR * conshdlr,
	SCIP_CONS * cons 
        );

   
     //* Désactivation d'un noeud (appelée à chaque fois qu'on quitte un noeud
     //* sauf si on le quitte pour aller dans un noeud fils)
   
    virtual SCIP_RETCODE scip_deactive(
	SCIP * 	scip,
	SCIP_CONSHDLR * 	conshdlr,
	SCIP_CONS * 	cons 
	);



      //////////////////////////:
      //////////////////////////
      
     //transforms constraint data into data belonging to the transformed problem 
    virtual SCIP_RETCODE scip_trans(
	SCIP*              scip,             
	SCIP_CONSHDLR*     conshdlr,          
	SCIP_CONS*         sourcecons,        
	SCIP_CONS**        targetcons         
	);


    virtual SCIP_RETCODE scip_check(
	SCIP*              scip,               // SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS**        conss,              // array of constraints to process 
	int                nconss,             // number of constraints to process 
	SCIP_SOL*          sol,                // the solution to check feasibility for 
	SCIP_Bool          checkintegrality,   // has integrality to be checked? 
	SCIP_Bool          checklprows,        // have current LP rows to be checked? 
	SCIP_Bool          printreason,        // should the reason for the violation be printed? 
	SCIP_Bool          completely,         // should all violations be checked? 
	SCIP_RESULT*       result              // pointer to store the result of the feasibility checking call 
	);
	
    virtual SCIP_RETCODE scip_enfolp(
	SCIP*              scip,               //SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS**        conss,              // array of constraints to process 
	int                nconss,             // number of constraints to process 
	int                nusefulconss,       // number of useful (non-obsolete) constraints to process 
	SCIP_Bool          solinfeasible,      // was the solution already declared infeasible by a constraint handler? 
	SCIP_RESULT*       result              // pointer to store the result of the enforcing call 
	);
	
    virtual SCIP_RETCODE scip_enfops(
	SCIP*              scip,               // SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS**        conss,              // array of constraints to process 
	int                nconss,             // number of constraints to process 
	int                nusefulconss,       // number of useful (non-obsolete) constraints to process 
	SCIP_Bool          solinfeasible,      // was the solution already declared infeasible by a constraint handler? 
	SCIP_Bool          objinfeasible,      // is the solution infeasible anyway due to violating lower objective bound? 
	SCIP_RESULT*       result              // pointer to store the result of the enforcing call 
	);
    
    virtual SCIP_RETCODE scip_lock(
	SCIP*              scip,               // SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS*         cons,               // the constraint that should lock rounding of its variables, or NULL if the
	SCIP_LOCKTYPE      in,					//   constraint handler does not need constraints 
	int                nlockspos,          // no. of times, the roundings should be locked for the constraint 
	int                nlocksneg           // no. of times, the roundings should be locked for the constraint's negation 
	);
    
    virtual SCIP_RETCODE scip_sepalp(
	SCIP*              scip,               // SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS**        conss,              // array of constraints to process 
	int                nconss,             // number of constraints to process 
	int                nusefulconss,       // number of useful (non-obsolete) constraints to process 
	SCIP_RESULT*       result              // pointer to store the result of the separation call 
	);
    
    virtual SCIP_RETCODE scip_sepasol(
	SCIP*              scip,               // SCIP data structure 
	SCIP_CONSHDLR*     conshdlr,           // the constraint handler itself 
	SCIP_CONS**        conss,              // array of constraints to process 
	int                nconss,             // number of constraints to process 
	int                nusefulconss,       // number of useful (non-obsolete) constraints to process 
	SCIP_SOL*          sol,                // primal solution that should be separated 
	SCIP_RESULT*       result              // pointer to store the result of the separation call 
	);
};

#endif
