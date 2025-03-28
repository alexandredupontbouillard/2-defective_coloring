

#include "GloutPricer.h"



GloutPricer::GloutPricer(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority,Alternance* alt,Separator* sep_) : ObjPricerColoring(scip_,p_name,priority,MM){
		G=GG; 
		M=MM;
		a =alt;
		sep = sep_;
		//cout<<"constructor pricer glout"<<endl; 
	}


StableSet* GloutPricer::coloring_pricing(SCIP* scip, SCIP_RESULT* result){


	//cout<<"**************PRICER GLOUT************"<<endl;



	//cout<<"PRICING GLOUT"<<endl;

	int i;
	vector<pair<int,float>> lambda;
	lambda.resize(G->nb_nodes);
	list<SCIP_ROW*>::iterator yy;
	
	list<Constraint*>::iterator ctn;
	vector<SCIP_Real>* u ;
	//cout<<"les coeffs de mes sommets : ";
	for (i = 0; i < G->nb_nodes; i++)  { // on récupère les coûts duaux des rhs1
		lambda[i]= make_pair (i,rounding(scip, SCIPgetDualsolLinear(scip, M->V_node_ineq[i])));
		//cout<<rounding(scip, SCIPgetDualsolLinear(scip, M->V_node_ineq[i]))<<" ";
		//cout<<"juskla"<<endl;
		ctn = M->ctn.begin();
	
		for(yy = M->additional_ineq.begin();yy != M->additional_ineq.end();yy ++){
			u = (*ctn)->getLinearization();
			lambda[i].second +=  (*u)[i] * rounding(scip,SCIProwGetDualsol(*yy));

			ctn++;
			delete(u);
		}
		//cout<<lambda[i].second<<" \\ ";
	}
	//cout<<endl;
	
	#ifdef OUTPUT_PRICER
	cout<<"dual solution"<<endl;
	for (i = 0; i < G->nb_nodes; i++){
		//cout<<"Cte_"<<i<<"="<<lambda[i]<<" ";
	}
	cout<<endl;
	#endif

	  



	StableSet* stable = G->findMaxStableSet(lambda);
	
    if(! isSSnegativeRC(scip, stable) && sepapa){
    
        //cout<<"juskla"<<endl;
        separate(scip, result);
        //cout<<"bas alors ? "<<endl;
    }


	//cout<<"**************PRICER GLOUT FINISHED************"<<endl;




	return stable;






}

void GloutPricer::setupBranching(SCIP_ConsData data){
setoutBranching();
	//cout<<"setup branching for glout pricer"<<endl;
	G->modifyGraphInBranching(&data);

}
void GloutPricer::setoutBranching(){
	//cout<<"setour branching for glout pricer"<<endl;
	G->resetGraphBranching();

}

