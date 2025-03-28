

#include "MaxNodePricerProj.h"



MaxNodePricerProj::MaxNodePricerProj(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority, int nbNodes) : ObjPricerColoring(scip_,p_name,priority,MM){
		G=GG; 
		M=MM;
		A_cplex.create_MIP(G,nbNodes);
		a =new NoCutting();
	}


StableSet* MaxNodePricerProj::coloring_pricing(SCIP* scip, SCIP_RESULT* result){


	//cout<<"**************PRICER MAX NODE************"<<endl;




	nbConstraints =nbConstraints + M->ctn.size();

	int i;
	vector<SCIP_Real> lambda;
	lambda.resize(G->nb_nodes);
	vector<SCIP_Real>* u ;
	list<Constraint*>::iterator ctn;
	list<SCIP_ROW*>::iterator yy;
	for (i = 0; i < G->nb_nodes; i++) {  // on récupère les coûts duaux des rhs1
		lambda[i]= rounding(scip, SCIPgetDualsolLinear(scip, M->V_node_ineq[i]));
		ctn = M->ctn.begin();
		for(yy = M->additional_ineq.begin();yy != M->additional_ineq.end();yy ++){ // on récupère les coûts duaux des contraintes ajoutées

			u = (*ctn)->getLinearization();
			lambda[i]+= (*u)[i] *  rounding(scip,SCIProwGetDualsol(*yy));
			delete(u);
		
			ctn++;
		}

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
	vector<SCIP_Real> rho;
	A_cplex.set_objective_coefficient(lambda,rho);
	bool stable_found = A_cplex.find_stableset(stable,cplex_objvalue);



	//cout<<"PRICER MAX NODE FINISHED"<<endl;



	return stable;






}


void MaxNodePricerProj::setupBranching(SCIP_ConsData data){
	setoutBranching();
	A_cplex.setupBranching(data);

}

void MaxNodePricerProj::setoutBranching(){
	A_cplex.setoutBranching();

}


