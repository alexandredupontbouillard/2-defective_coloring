#include "Cplex_proj_algo.h"



using namespace std;

#define eps 1e-5
//#define CPLEX_LOG  //Si non commente, affiche des logs

//#define WRITEPL






void Cplex_proj_algo::create_MIP(C_Graph *GG,int maxNodes){

  
	#ifdef CPLEX_LOG
	cout<<"**************************** INIT CPLEX *********************"<<endl;
	nbIt =0;
	#endif
	nbBranchingDecision=0;
	G=GG;
	nb_additional_variable = 0;
	model= IloModel(env);

	
	////////////////////////
	//////  VAR
	////////////////////////

	int i,k;

	x = IloNumVarArray(env, G->nb_nodes, 0.0, 1.0, ILOINT);
	cout<<G->nb_nodes<<endl;
	additional = IloNumVarArray(env, 0, 0.0, 1.0, ILOINT);
	for(i = 0; i < G->nb_nodes; i++) {
		ostringstream varname;
		//IloNumVar v(env, 0.0, 1.0, ILOINT);
		varname.str("");
		varname<<"x_"<<i;
		x[i].setName(varname.str().c_str());


	}
	
	y = IloNumVarArray(env, G->nb_links, 0.0, 1.0, ILOINT);

	for(i = 0; i < G->nb_links; i++) {
		ostringstream varname;
		//IloNumVar v(env, 0.0, 1.0, ILOINT);
		varname.str("");
		varname<<"y_"<<i;
		y[i].setName(varname.str().c_str());


	}
	



	//////////////
	//////  EDGE INEQUALITIES
	//////////////

	IloRangeArray CC(env);
	int nbcst=0;

	// Cst x_i + x_j \le 1 for every edges ij in E
	int k2;
	for (k=0;k<G->nb_links;k++){
		IloExpr cst(env);
		
		
		cst+= x[G->V_links[k]->v1] + x[G->V_links[k]->v2] - y[k];
	
		CC.add(cst<=1);
		ostringstream cstname;
		cstname.str("");
		cstname<<"c_edge"<<k<<"_";
		CC[nbcst].setName(cstname.str().c_str());
		nbcst++;
	}
	

	for (k=0;k<G->nb_nodes;k++){
		IloExpr cst(env);
		for(list<C_link*>::iterator bc = G->V_nodes[k].L_adjLinks.begin();bc != G->V_nodes[k].L_adjLinks.end() ; bc ++){
			cst+=y[(*bc)->num];
		
		
		}

		
		
		cst+= -x[k];
	
		CC.add(cst<=0);
		ostringstream cstname;
		cstname.str("");
		cstname<<"c_vertex"<<k<<"_";
		CC[nbcst].setName(cstname.str().c_str());
		nbcst++;
	}



//	if(true){
	//	IloExpr cst(env);
	//	for(i = 0; i < G->nb_nodes ; i ++){
	//		cst+= x[i];


	//	}
//		
//		CC.add(cst>=1+1e-9);
//		
//	}

	model.add(CC);


	//////////////
	////// OBJ
	//////////////
	

	// Initialization without any value
	//
	obj = IloAdd(model, IloMaximize(env, 0.0));


	//////////////
	////// CPLEX 
	//////////////




	cplex = IloCplex(model);
	cplex.setParam(IloCplex::Param::Simplex::Limits::UpperObj, -1e+20 );
		if(maxNodes != -1){
		
	 
		
		cplex.setParam(IloCplex::Param::MIP::Limits::Nodes,maxNodes);

		}

	#ifndef CPLEX_LOG
	cplex.setOut(env.getNullStream());
	cplex.setWarning(env.getNullStream());
	#endif
	branchingCt = *(new IloConstraintArray(env));

	#ifdef CPLEX_LOG
	cout<<"**************************** FIN INIT CPLEX *********************"<<endl;
	#endif
	



}


void Cplex_proj_algo::set_objective_coefficient(const vector<SCIP_Real>& obj_coeff, const vector<SCIP_Real>& coeff_additional){
	int i;
 
 	
 	
 	
/*	for (i=0;i<G->nb_nodes;i++){
		obj.setLinearCoef(x[i],abs(obj_coeff[i]));

	}

	for(i = 0 ; i < coeff_additional.size(); i ++){

		obj.setLinearCoef(additional[i],abs(coeff_additional[i]));


	}
	
*/
	IloExpr cst(env);
	for (i=0;i<G->nb_nodes;i++){
		cst+=x[i] * abs(obj_coeff[i]);
		//obj.setLinearCoef(x[i],abs(obj_coeff[i]));

	}

	for(i = 0 ; i < coeff_additional.size(); i ++){
		cst+=additional[i] *abs(coeff_additional[i]);
		//obj.setLinearCoef(additional[i],abs(coeff_additional[i]));
		
		


	}
	bound = cst>=1+1e-5;


	
	
}	
	

void Cplex_proj_algo::modify_pricer(list<Constraint*> ct){
		list<Constraint*>::iterator it;
		IloNumVar* ll;
		int i = 0;

		for(it = ct.begin() ; it !=ct.end(); it ++){

			if(i >= nb_additional_variable){
						cout<<"une iter é"<<endl;
				
				ll = 	(*it)->modifyPricer(this);
				nb_additional_variable ++;

										cout<<"ouééé"<<endl;

				additional.add(*ll);
			}
			i++;	
		
		}


}


// return true if a stable set have been found by Cplex
	bool  Cplex_proj_algo::find_stableset(StableSet* stable, SCIP_Real& objvalue) {
	list<int>::const_iterator it;
	int i;
	stable->G = G;
	#ifdef CPLEX_LOG
	cout<<endl<<" ************************* LAUCH PRICER with CPLEX"<<endl<<endl;
	#endif

	  
	#ifdef WRITEPL
	ostringstream filename;
	filename.str("");
	filename<<"../debug/pricing"<<nbIt<<".lp"<<"";
	nbIt++;


	cplex.exportModel(filename.str().c_str());
	#endif

	model.add(bound);

	if ( !cplex.solve() ) {
		//cout<<"status = "<< cplex.getStatus()<<endl;
		cout<<"ici "<<endl;
		model.remove(bound);
		//env.error() << "Failed to optimize Pricer with Cplex" << endl;
		
		//exit(1);
		return false;
	}
	else if (cplex.getStatus()==CPX_STAT_INFEASIBLE){
		
		#ifdef CPLEX_LOG
		cout<<"NO SOLUTION BECAUSE INFEASIBILITY ---- OUCH Cela semble ne devoir jamais se produire----"<<endl;
		cout<<endl<<" ************************* END PRICER with CPLEX"<<endl<<endl;
		#endif
				model.remove(bound);
		return false;
	}
	else{


		objvalue=cplex.getObjValue();
		for(i=0;i<G->nb_nodes;i++){
			if (cplex.getValue(x[i])>1-eps){
				stable->add(i);
			}
			else{

			//	cout<<0<<" ";
			}

		}
		model.remove(bound);

		//G->completeIntoMaxStable(stable);


	


		#ifdef CPLEX_LOG
		cout<<"Produced column : ";
		for (it=stable->L_nodes.begin();it!=stable->L_nodes.end();it++)
			cout<<*it<<" ";
		cout<<endl;
		cout<<"Objective value : "<<objvalue<<endl;
		cout<<endl<<" ************************* END PRICER with CPLEX"<<endl<<endl;     
		#endif

		return true;

	}


}

void Cplex_proj_algo::setupBranching(SCIP_ConsData data){

	SCIP_ConsData* d = &data;
	/*
	for(int i = 0 ; i < G->nb_nodes ; i++){
		for(int j = 0 ; j< i ; j++){
			if(G->adjacency[i][j] ==3 || G->adjacency[i][j] == 5){
				nbBranchingDecision++;
				IloExpr Ct1(env);
				IloConstraint cons;
				Ct1+= x[i] + x[j];
				cons = Ct1 <= 1;
				branchingCt.add( cons);
			
			}
			else if(G->adjacency[i][j] ==2 || G->adjacency[i][j] == 4){
			
				nbBranchingDecision++;
				IloExpr Ct1(env);
				IloConstraint cons;
			
				Ct1+= 	x[i] - x[j];
				cons = (Ct1 == 0);
				branchingCt.add( cons);
			}
					
	
		
		}
	
	}*/
	
	while(d != NULL){

		nbBranchingDecision++;
		IloExpr Ct1(env);
		IloConstraint cons;
		
		if(d->same){
			Ct1+= 	x[d->v1] - x[d->v2];
			cons = (Ct1 == 0);
			
		}
		else{

			Ct1+= x[d->v1] + x[d->v2];
			cons = Ct1 <= 1;


		}
		
		branchingCt.add( cons);
		d = d->father_data;

	}
	model.add(branchingCt);

}

void Cplex_proj_algo::setoutBranching(){
	//cout<<"setout branching cplex model"<<endl;
	for(int i = 0 ; i < nbBranchingDecision ; i++){
		
		model.remove(branchingCt[i]);
	}

	branchingCt.endElements();
	nbBranchingDecision =0;
}



