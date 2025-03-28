

#include "ExactPricerProj.h"



ExactPricerProj::ExactPricerProj(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority) : ObjPricerColoring(scip_,p_name,priority,MM){
		//cout<<"constructor exact pricer called"<<endl;
		G=GG; 
		M=MM;
		
		A_cplex.create_MIP(G,-1);
		
		a =new NoCutting();
		//cout<<"constructor exact pricer finished"<<endl;
	}


StableSet* ExactPricerProj::coloring_pricing(SCIP* scip, SCIP_RESULT* result){


	cout<<"**************PRICER EXACT************"<<endl;




	nbConstraints =nbConstraints + M->ctn.size();

	int i;
	vector<SCIP_Real> lambda;
	lambda.resize(G->nb_nodes);
	vector<SCIP_Real> rho;
	rho.resize(M->additional_ineq.size()	);

	for (i = 0; i < G->nb_nodes; i++)   // on récupère les coûts duaux des rhs1
		//lambda[i]= rounding(scip, SCIPgetDualsolLinear(scip, M->V_node_ineq[i]));
		lambda[i]=SCIPgetDualsolLinear(scip, M->V_node_ineq[i]);



	list<SCIP_ROW*>::iterator yy;
	i=0;
	for(yy = M->additional_ineq.begin();yy != M->additional_ineq.end();yy ++){ // on récupère les coûts duaux des contraintes ajoutées
		//rho[i] = rounding(scip,SCIProwGetDualsol(*yy));
		rho[i] = SCIProwGetDualsol(*yy);
		i++;	
	}




	#ifdef OUTPUT_PRICER
	cout<<"dual solution"<<endl;
	for (i = 0; i < G->nb_nodes; i++){
	cout<<"Cte_"<<i<<"="<<lambda[i]<<" ";
	}
	cout<<endl;
	#endif

	  



	StableSet* stable = new StableSet();
	stable->initialize(G->nb_nodes);
	SCIP_Real* cplex_objvalue =(SCIP_Real*) malloc(sizeof(SCIP_Real));

		cout<<"bloublou"<<endl;
	A_cplex.modify_pricer( M->ctn);



	A_cplex.set_objective_coefficient(lambda,rho);
	bool stable_found = A_cplex.find_stableset(stable,cplex_objvalue);




	cout<<"PRICER EXACT FINISHED"<<endl;


	return stable;






}


void ExactPricerProj::setupBranching(SCIP_ConsData data){ //TODO
	setoutBranching();
	A_cplex.setupBranching(data);
	}
void ExactPricerProj::setoutBranching(){ //TODO

	A_cplex.setoutBranching();



}

