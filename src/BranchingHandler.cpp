#include"BranchingHandler.h"

#define OUTPUT_HANDLER

//////////////////////////////////////////////
//////////////////////////////////////////////


BranchingHandler::BranchingHandler(SCIP* scip,  C_Graph *GG, list<ObjPricerColoring*> list_pricerr , list<Separator*> list_separatorr  , C_master_coloring* M_) : ObjConshdlr(
		scip,"BranchingHandler", "Handler For Branching Constraints",2000000, -2000000, -2000000,1, -1, 1, 0, FALSE, FALSE, FALSE,SCIP_PROPTIMING_BEFORELP,SCIP_PRESOLTIMING_FAST){
		G = GG;
		list_pricer = list_pricerr;
		M=M_;	
		list_separator=list_separatorr;

    }


void create_CteBranch(SCIP* scip, bool same, int v1, int v2,  SCIP_ConsData *father_data, SCIP_CONS** cons){


	#ifdef OUTPUT_HANDLER
		cout << " ------ CREATE A CONSTRAINT ASSOCIATED TO A NODE   ---------------  \n";
	#endif


	// initialise les donnees specifiques au noeud fils
	SCIP_ConsData* consdata = new SCIP_ConsData;
	SCIP_CONSHDLR* conshdlr = SCIPfindConshdlr(scip, "BranchingHandler");
    
	#ifdef OUTPUT_HANDLER
		if (conshdlr==NULL) cout<<"CONSTRAINT HANDLER NOT FOUND -> CHECK SCIP_DEBUG TO SEE ITS PARAMETERS"<<endl;
	#endif
	
	// cree la constrainte de branchement du noeud fils
	consdata->same = same;
	consdata->v1 = v1;
	consdata->v2=v2;
	consdata->father_data = father_data;


 
	SCIPcreateCons(scip, cons, "BranchingCste", conshdlr, consdata,
    							FALSE, //initial
    							FALSE, //separate
    							FALSE, //enforce
    							FALSE, //check
    							TRUE,  //propagate
    							TRUE,  //local
    							FALSE, //modifiable
    							FALSE, //dynamic
    							FALSE, //removable
		                                        TRUE); //stickinganode

    
    
	#ifdef OUTPUT_HANDLER
		cout << " ------ END CREATION  ---------------  \n";
	#endif
 
}


//////////////////////////////////////////////
//////////////////////////////////////////////
SCIP_RETCODE BranchingHandler::scip_active(SCIP * scip, SCIP_CONSHDLR * conshdlr, SCIP_CONS * cons) {

	#ifdef OUTPUT_HANDLER
	    cout << " --------------------- Active handler ---------------  \n";
	#endif
 
    SCIP_ConsData *consdata = SCIPconsGetData(cons);

	


 	list<ObjPricerColoring*>::iterator pricerIt;
	for(pricerIt = list_pricer.begin();pricerIt !=  list_pricer.end(); pricerIt ++){

		if(consdata != NULL)
		(*pricerIt)->setupBranching(*consdata);

		
	}
//	for(int i = 0 ; i < G->nb_nodes; i++){
//		for(int j = 0; j <i ; j++){
//			cout<<G->adjacency[i][j]<<" ";
//		}
//		cout<<endl;
//	
//	}

	
	list<Separator*>::iterator sepaIt;
	for(sepaIt = list_separator.begin();sepaIt !=  list_separator.end(); sepaIt ++){

		(*sepaIt)->setupBranching(*consdata);

	}

	
	list<C_master_var*>::iterator varIt;   
	list<C_master_DOI*>::iterator itDOI;
	G->resetGraphBranching();
	G->modifyGraphInBranching(consdata);
	
	/*for(int i = 0 ; i < G->nb_nodes ; i++){
		for(int j = 0 ; j < i ; j++){
			if(G->adjacency[i][j] == 3 ||G->adjacency[i][j] == 5){
			
				for(varIt = M->L_var.begin(); varIt != M->L_var.end(); varIt ++){
			
					if( (*varIt)->stable->contains(i) && (*varIt)->stable->contains(j)){
						
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*varIt)->ptr , 0.0);
					

					}

				}
				for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end(); itDOI++){
					if( (*itDOI) ->containsBranch(i,G) && !(*itDOI) ->containsBranch(j,G)){
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
					
					}
					if( (*itDOI) ->containsBranch(j,G) && !(*itDOI) ->containsBranch(i,G)){
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
					
					}
					if((*itDOI) ->v == i || ((*itDOI)->v == j)){
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
			
					}
				 }
			
			
			
			}
			else if(G->adjacency[i][j] == 2 ||G->adjacency[i][j] == 4){
				for(varIt = M->L_var.begin(); varIt != M->L_var.end(); varIt ++){
			
					if( ((*varIt)->stable->contains(i) && !(*varIt)->stable->contains(j)) ||( !(*varIt)->stable->contains(i) && (*varIt)->stable->contains(j))){
						
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*varIt)->ptr , 0.0);
					

					}

				}
				for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end(); itDOI++){
		
					if( (*itDOI) ->containsBranch(i,G) && (*itDOI) ->containsBranch(j,G)){
						SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
					}				
					
				
				}
			
			
			}
		
		
		}
	
	
	}
	G->resetGraphBranching();*/
	while(consdata!= NULL ){
		if (consdata->same){
		
			
			
			for(varIt = M->L_var.begin(); varIt != M->L_var.end(); varIt ++){
				
				if( ((*varIt)->stable->contains(consdata->v1) && (!(*varIt)->stable->contains(consdata->v2))) ||( (!(*varIt)->stable->contains(consdata->v1)) && (*varIt)->stable->contains(consdata->v2))){
					
					SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*varIt)->ptr , 0.0);
				

				}

			}
			
			for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end(); itDOI++){
				if( (*itDOI)->containsBranch(consdata->v1,G) && (!(*itDOI)->containsBranch(consdata->v2,G))     ){
					SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
				
				}
				if( (*itDOI)->containsBranch(consdata->v2,G) && (!(*itDOI) ->containsBranch(consdata->v1,G))    ){
					SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
				
				}
	//			if( ((*itDOI) ->containsBranch(consdata->v2,G) || (*itDOI) ->containsBranch(consdata->v1,G))  &&  ((*itDOI) ->v == consdata->v1 || (*itDOI) ->v == consdata->v2)){
	//				SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
	//			
	//			}
	//			if((*itDOI) ->v == consdata->v1 || ((*itDOI)->v == consdata->v2)){
	//				SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
	//			
	//			}
				
				
			
			}
					
			
		}
		else{
			for(varIt = M->L_var.begin(); varIt != M->L_var.end(); varIt ++){
				if( (*varIt)->stable->contains(consdata->v1) && (*varIt)->stable->contains(consdata->v2) ){
					SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*varIt)->ptr , 0.0);

				}

			}
					list<C_master_DOI*>::iterator itDOI;
			for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end(); itDOI++){
			
				if( (*itDOI) ->containsBranch(consdata->v1,G) && (*itDOI) ->containsBranch(consdata->v2,G)){
					SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
				}
			
				
	//			if( (*itDOI) ->containsBranch(consdata->v2,G) && !(*itDOI) ->containsBranch(consdata->v1,G)){
	//				SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
	//			
	//			}
	//			
	//			if( (*itDOI) ->containsBranch(consdata->v1,G) && !(*itDOI) ->containsBranch(consdata->v2,G)){
	//				SCIPchgVarUbNode(scip, SCIPgetCurrentNode(scip), (*itDOI)->ptr , 0.0);
	//			
	//			}
				
				
			
			}
			
			

		}
		consdata = consdata->father_data;
	}

    
          // changes upper bound of variable in the given node; if possible, adjust bound
          //  to integral value; doesn't store any inference information in the bound change,
          //such that in conflict analysis, this change is treated like a branching decision


    
//    #ifdef OUTPUT_HANDLER
//      cout<<"Var ";
//      for (it=consdata->var->L_nodes.begin();it!=consdata->var->L_nodes.end();it++)
//	cout<<*it<<" ";
//      cout<<" set to "<<consdata->same<<endl;
//    #endif




#ifdef OUTPUT_HANDLER
    cout << " --------------------- Fin Active handler ---------------  \n";
#endif


    return SCIP_OKAY;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
SCIP_RETCODE BranchingHandler::scip_deactive(SCIP* scip, SCIP_CONSHDLR* conshdlr, SCIP_CONS*cons){
#ifdef OUTPUT_HANDLER
	cout << " --------------------- Desactive handler ---------------  \n";
#endif

	assert(conshdlr != NULL);
	assert(cons != NULL);

	
	list<ObjPricerColoring*>::iterator pricerIt;
/*
	for(pricerIt = list_pricer.begin();pricerIt !=  list_pricer.end(); pricerIt ++){
		
		(*pricerIt)->setoutBranching();

	}
	

	list<Separator*>::iterator sepaIt;
	for(sepaIt = list_separator.begin();sepaIt !=  list_separator.end(); sepaIt ++){

		(*sepaIt)->setoutBranching();

	}*/

	return SCIP_OKAY;
    }


//////////////////////////////////////////////
/** transforms constraint data into data belonging to the transformed problem */
    SCIP_RETCODE BranchingHandler::scip_trans(
	SCIP*              scip,               //**< SCIP data structure *
	SCIP_CONSHDLR*     conshdlr,           //**< the constraint handler itself *
	SCIP_CONS*         sourcecons,         //**< source constraint to transform *
	SCIP_CONS**        targetcons          //**< pointer to store created target constraint *
	) {

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Trans handler ---------------  \n";
#endif
	
   SCIP_CONSDATA* sourcedata;
   SCIP_CONSDATA* targetdata;

   sourcedata = SCIPconsGetData(sourcecons);
   targetdata = NULL;

   targetdata= new SCIP_CONSDATA;
   //targetdata->var = sourcedata->var;
   targetdata->father_data = sourcedata->father_data;
   targetdata->v1 = sourcedata->v1;
   targetdata->v2 = sourcedata->v2;
   targetdata->same = sourcedata->same;
   SCIPcreateCons(scip, targetcons, SCIPconsGetName(sourcecons), conshdlr, targetdata,
				  SCIPconsIsInitial(sourcecons), SCIPconsIsSeparated(sourcecons), SCIPconsIsEnforced(sourcecons),
				  SCIPconsIsChecked(sourcecons), SCIPconsIsPropagated(sourcecons),
				  SCIPconsIsLocal(sourcecons), SCIPconsIsModifiable(sourcecons), 
				  SCIPconsIsDynamic(sourcecons), SCIPconsIsRemovable(sourcecons), SCIPconsIsStickingAtNode(sourcecons));


    
	return SCIP_OKAY;
    }


/////////////////////////////////////////////
    SCIP_RETCODE BranchingHandler::scip_check(
	SCIP*              scip,               /**< SCIP data structure */
	SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
	SCIP_CONS**        conss,              /**< array of constraints to process */
	int                nconss,             /**< number of constraints to process */
	SCIP_SOL*          sol,                /**< the solution to check feasibility for */
	SCIP_Bool          checkintegrality,   /**< has integrality to be checked? */
	SCIP_Bool          checklprows,        /**< have current LP rows to be checked? */
	SCIP_Bool          printreason,        /**< should the reason for the violation be printed? */
	SCIP_Bool          completely,         /**< should all violations be checked? */
	SCIP_RESULT*       result) {

#ifdef OUTPUT_HANDLER
	//std::cout << " --------------------- Check handler ---------------  \n";
#endif


	*result = SCIP_FEASIBLE;
	return SCIP_OKAY;

    }
	
    SCIP_RETCODE BranchingHandler::scip_enfolp(
	SCIP*              scip,               /**< SCIP data structure */
	SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
	SCIP_CONS**        conss,              /**< array of constraints to process */
	int                nconss,             /**< number of constraints to process */
	int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
	SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
	SCIP_RESULT*       result) {

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Enfolp handler ---------------  \n";
#endif


	*result = SCIP_FEASIBLE;
	return SCIP_OKAY;
    }
	
    SCIP_RETCODE BranchingHandler::scip_enfops(
	SCIP*              scip,               /**< SCIP data structure */
	SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
	SCIP_CONS**        conss,              /**< array of constraints to process */
	int                nconss,             /**< number of constraints to process */
	int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
	SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
	SCIP_Bool          objinfeasible,      /**< is the solution infeasible anyway due to violating lower objective bound? */
	SCIP_RESULT*       result) {

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Enfops handler ---------------  \n";
#endif


	*result = SCIP_FEASIBLE;
	return SCIP_OKAY;
    }

    SCIP_RETCODE BranchingHandler::scip_lock(
	SCIP*              scip,               /**< SCIP data structure */
	SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
	SCIP_CONS*         cons,               /**< the constraint that should lock rounding of its variables, or NULL if the*/
	SCIP_LOCKTYPE	   in,				/*   constraint handler does not need constraints */
	int                nlockspos,          /**< no. of times, the roundings should be locked for the constraint */
	int                nlocksneg) {

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Lock handler ---------------  \n";
#endif


	return SCIP_OKAY;
    }
		
    SCIP_RETCODE BranchingHandler::scip_sepalp(
	SCIP*              scip,               /**< SCIP data structure */
	SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
	SCIP_CONS**        conss,              /**< array of constraints to process */
	int                nconss,             /**< number of constraints to process */
	int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
	SCIP_RESULT*       result) {

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Sepalp handler ---------------  \n";
#endif

	if(ceil(SCIPgetLPObjval(scip)) < SCIPgetPrimalbound(scip)){
		*result = SCIP_DIDNOTFIND;
	
	}
	else{
		*result = SCIP_DIDNOTFIND;
		//*result = SCIP_CUTOFF;
	}
	
	return SCIP_OKAY;
    }

    SCIP_RETCODE BranchingHandler::scip_sepasol(SCIP* scip, SCIP_CONSHDLR* conshdlr, SCIP_CONS** conss, 
						int nconss, int nusefulconss, SCIP_SOL* sol, SCIP_RESULT* result){

#ifdef OUTPUT_HANDLER
	std::cout << " --------------------- Sepasol handler ---------------  \n";
#endif

	*result = SCIP_CUTOFF;
	return SCIP_OKAY;
    }
