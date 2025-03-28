
#include "Pricer_Coloring.h"
#include <utility>
SCIP_DECL_PRICERINIT(ObjPricerColoring::scip_init)
{
  int i;
  //cout<<"abstract pricer init"<<endl;
  for (i = 0; i < G->nb_nodes; i++){
    SCIPgetTransformedCons(scip, M->V_node_ineq[i], &(M->V_node_ineq)[i]);
  }
   
   return SCIP_OKAY;
}


ObjPricerColoring::ObjPricerColoring(SCIP* scip, const char* p_name,int priority,C_master_coloring *M_):ObjPricer(scip, p_name, "Finds stable set with negative reduced cost.", priority, TRUE){
	//cout<<"classic pricer started"<<endl;
	M = M_;
	nbConstraints=0;
	NAME = (char *) p_name;

}


SCIP_DECL_PRICERREDCOST(ObjPricerColoring::scip_redcost)
{
	//cout<<"pricing init "<<endl;
	
	
	//cout<<"yo la night c moi "<<endl;
	//cout<< SCIPgetLPObjval(scip)<<endl;

	SCIPdebugMsg(scip, "call scip_redcost ...\n");

	*result = SCIP_SUCCESS;

	if(a->ShouldWePrice(scip)){

		auto start = chrono::steady_clock::now();
		int i = nbColumns;

		addColumn(scip,coloring_pricing(scip,result));
		auto end = chrono::steady_clock::now();
		pricingTime+= chrono::duration_cast<chrono::microseconds>(end-start).count();
		pricingCalled++;
		//for(list<C_master_DOI*>::iterator DOI = M->DOI_var.begin(); DOI != M->DOI_var.end(); DOI++){
		//	cout<<"DOI : " << SCIPgetSolVal(scip, NULL, (*DOI)->ptr) <<endl;
		
		//}

	}
	else{    

		if(sep != NULL){ //verify that there is a separator
			//cout<<"SEPARATING"<<endl;

			auto startsep =chrono::steady_clock::now();
			
			if(separate(scip,result)){

				auto endsep =chrono::steady_clock::now();
				separatingTime+=chrono::duration_cast<chrono::milliseconds> (endsep - startsep).count();
					nbConstraints+=sep->addConstraints(scip,result);
			
			}
			else{	
			//	cout<<"did not fin any constraints"<<endl;
				auto endsep =chrono::steady_clock::now();
				separatingTime+= chrono::duration_cast<chrono::milliseconds>(endsep - startsep).count();
				auto start = chrono::steady_clock::now();
				addColumn(scip,coloring_pricing(scip,result));
				auto end = chrono::steady_clock::now();
				a->hadToPrice();
				pricingTime+= chrono::duration_cast<chrono::microseconds>(end-start).count();
				pricingCalled++;
			}
			

		}
		


	}
	
  

   	//cout<<"pricing stop "<<endl;
	 return SCIP_OKAY;
}


bool ObjPricerColoring::separate(SCIP* scip, SCIP_RESULT* result){
	return sep->separate(scip,result);

}


bool ObjPricerColoring::isSSnegativeRC(SCIP* scip, StableSet* stable){

	list<int>::const_iterator it;
	SCIP_Real sum = 0;
	for (it = stable->L_nodes.begin(); it != stable->L_nodes.end(); it++){
		sum+= rounding(scip, SCIPgetDualsolLinear(scip, M->V_node_ineq[(*it)]));
	

	}

	list<Constraint*>::iterator itctn;
	list<SCIP_ROW*>::iterator rowIterator = M->additional_ineq.begin();

	for(itctn  = M->ctn.begin(); itctn != M->ctn.end() ; itctn ++){
		
		

		sum+= (*itctn)->coeffOfStable(stable) * rounding(scip, SCIProwGetDualsol( (*rowIterator)));
		rowIterator++;
	
	} 

	return 1-sum 	<-1e-9;


}

void ObjPricerColoring::addColumn(SCIP* scip,StableSet* s){
	if(s != NULL){
		if(isSSnegativeRC(scip,s) && G->isCo2plex(s->L_nodes) ){
		
			if(! G->respectBranching( *s)){
				return;
			
			}

			nbColumns++;
			char var_name[255];
			SCIPsnprintf(var_name, 255, "V_%d",M->L_var.size());
			SCIPdebugMsg(scip, "new variable <%s>\n", var_name);

		    
			C_master_var *var = new C_master_var;
			SCIPcreateVar(scip, &(var->ptr), var_name,
				  0.0,                     // lower bound
				  1e20,//G->nb_nodes,      // upper bound
				  1.0,                     // objective
				  SCIP_VARTYPE_INTEGER, // variable type
				  true, true, NULL, NULL, NULL, NULL, NULL);

			/* add new variable to the list of variables to price into LP (score: leave 1 here) */
			list<int>::iterator it;
			for (it = s->L_nodes.begin(); it != s->L_nodes.end(); it++){
				SCIPaddCoefLinear(scip, M->V_node_ineq[*it], var->ptr, 1.0);
				

			}
			SCIPaddPricedVar(scip, var->ptr, 1.0);
			/* add coefficient into the set covering constraints */
			var->stable=s;
			

			
			/*
			list<int>::iterator itt;
			for (it = s->L_nodes.begin(); it != s->L_nodes.end(); it++){
				int sizen = 0;
				for (itt = s->L_nodes.begin(); itt != s->L_nodes.end(); itt++){
					if(G->adjacency[*it][*itt] == 1){
						sizen++;
					
					}
				

				}
				if(sizen>=2){
					cout<<"error"<< typeid(*this).name()<<endl;
				
				}
				

			}
			*/
			
			


			list<Constraint*>::iterator itctn;
			list<SCIP_ROW*>::iterator rowIterator = M->additional_ineq.begin();
			for(itctn  = M->ctn.begin(); itctn != M->ctn.end() ; itctn ++){


				SCIPaddVarToRow(scip, (*rowIterator), var->ptr,(*itctn)->coeffOfStable(s) );
				rowIterator++;

			} 

			M->L_var.push_back(var);
		}
		else{
			delete(s);
		
		}

			//cout<<endl;
			/*
			cout<<"ses coefficients dans les contraintes additionelles"<<endl;
			list<Constraint*>::iterator itctn;
			list<SCIP_ROW*>::iterator rowIterator = M->additional_ineq.begin();
			for(itctn  = M->ctn.begin(); itctn != M->ctn.end() ; itctn ++){

				cout<<(*itctn)->coeffOfStable(s)<<" " << SCIProwGetDualsol( (*rowIterator))<< " //";
				rowIterator++;

			} */
			
	}

			
}
	





