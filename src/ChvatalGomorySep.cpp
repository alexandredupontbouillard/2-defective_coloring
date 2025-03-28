#include "ChvatalGomorySep.h"

#define DELTA 0.01
#include "ilconcert/ilosys.h"
#define eps 1e-5
#define SIZEPOOL 20


ChvatalGomorySep::ChvatalGomorySep(SCIP* scip,C_master_coloring* _master, const char* p_name,int nb_nodes_, int maxNode, int typeOfCoeffDoi_): Separator(scip,p_name,_master){

	//cout<<"constructor called"<<endl;
	M = _master;
	nbColumnAtLastIteration=0;
	nb_nodes= nb_nodes_;
	nbMaxNode = maxNode;
	typeOfCoeffDoi=typeOfCoeffDoi_;
	//cout<<"constructor finished"<<endl;

}


bool ChvatalGomorySep::separate(SCIP* scip,SCIP_RESULT* result){
	//cout<<"separator called"<<endl;
	*result =SCIP_DIDNOTFIND ;
	vector<SCIP_Real> coeff; // récupérer les coefficients des stables de la solution
	int nb_stables =M->L_var.size(); 
	coeff.resize(nb_stables);

	list<C_master_var*>::iterator it;  // on récupère les valeurs des variables et les stables en base 
	int i = 0;
	
	IloRangeArray CC(env);

	for(it = M->L_var.begin(); it != M->L_var.end(); it++){
		
		coeff[i]=SCIPgetSolVal(scip, NULL, (*it)->ptr);		

		if(i >= nbColumnAtLastIteration){   // we have to create new variables associated to new stables and their associated constraints
					
			f.add(IloNumVar(env, 0.0, 1.0- DELTA, ILOFLOAT));
			
	 		alpha.add(IloNumVar(env, 0.0, nb_nodes, ILOINT));

			ostringstream varname;
    			varname.str("");
				
    			varname<<"f_"<<i+1;
		        f[i].setName(varname.str().c_str());
			
    			varname.str("");
    			varname<<"alpha_"<<i+1;
		        alpha[i].setName(varname.str().c_str());
			
			IloExpr cst(env);
			
			cst+= f[i]-alpha[i];
			//cst+= -alpha[i];
			for(int j = 0 ; j < nb_nodes ; j++){
				if((*it)->stable->contains(j)){

					cst+=  u[j];
				}

			}
			CC.add(cst==0);
	

		}



		obj.setLinearCoef(alpha[i],- arrondiS(coeff[i])); // on met à jour la nouvelle fonction objectif


		i++;
	}
	/*
	if(typeOfCoeffDoi==0){
		
		list<C_master_DOI*>::iterator itDOI;
		int oo = 0;
		if(M->DOI_var.size()>0){
			for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end() ; itDOI ++){
				obj.setLinearCoef(alphaDOI[oo],- arrondiS(SCIPgetSolVal(scip, NULL, (*itDOI)->ptr)));
				oo++;

			}
		}

	}*/
	

	nbColumnAtLastIteration =M->L_var.size();
	model.add(CC);

	if ( !cplex.populate() ) {
    		
		cout<<"seems like model is infeasible"<<endl;
		return SCIP_OKAY;
  	}
	


	double objvalue=cplex.getObjValue();	
	ctn.clear();
	
	if(objvalue >=0.1  ){
	
		// fabriquer les contraintes
		
	
		int maxCt = cplex.IncumbentId;
		SCIP_Real maxDiff;
		SCIP_Real r;
		//cout<<"jusjka2"<<endl;
		for(int i = 0 ; i< cplex.getSolnPoolNsolns(); i++){
			vector<SCIP_Real>* v = new vector<SCIP_Real>(nb_nodes);
			for(int k = 0 ; k < nb_nodes ; k++){
				(*v)[k] = arrondis(cplex.getValue(u[k], i));
				

			}

			ctn.push_back(new ChvatalGomoryCst(v, nb_nodes));

		}
	
		return true;
	}
	//cout<<"separator finished"<<endl;
	return  false;


}


void ChvatalGomorySep::init(){
	//cout<<"**************************** INIT CPLEX *******************"<<endl;

	model= IloModel(env);




	u = IloNumVarArray(env, nb_nodes, 0.0, IloInfinity , ILOFLOAT);
	alpha_0 = IloNumVar(env, 0.0, nb_nodes , ILOINT);
	alpha_0.setName("alpha_0");
	f_0= IloNumVar(env, 0.0, 1.0-DELTA, ILOFLOAT);
	f_0.setName("f_0");
	f = IloNumVarArray(env,0);
	alpha =IloNumVarArray(env,0);
	IloNumVarArray x_w;
  	IloNumVarArray x_T;

	IloRangeArray CC(env);
	
	if(typeOfCoeffDoi ==0){
		alphaDOI = IloNumVarArray(env, M->DOI_var.size(), 0.0 , nb_nodes-DELTA ,ILOFLOAT); // ajouter les bonnes contraintes
		x_w = IloNumVarArray(env, M->DOI_var.size(), 0.0 , 1.0 ,ILOINT);
		x_T = IloNumVarArray(env, M->DOI_var.size(), 0.0 , nb_nodes-DELTA ,ILOINT);
		int d = 0;
		list<C_master_DOI*>::iterator itDOI;
		for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end() ; itDOI++){
			ostringstream varname;
			varname.str("");
			varname<<"alphaDOI_"<<d+1;
			alphaDOI[d].setName(varname.str().c_str());

			varname.str("");
			varname<<"x_w_"<<d+1;
			x_w[d].setName(varname.str().c_str());

			varname.str("");
			varname<<"x_T_"<<d+1;
			x_T[d].setName(varname.str().c_str());


			IloExpr cst1(env);
			IloExpr cst2(env);
			IloExpr cst3(env);
			IloExpr cst4(env);
			IloExpr cst5(env);	
			list<int>::iterator itStable;
			for(itStable = (*itDOI)->neighborhood.begin() ; itStable != (*itDOI)->neighborhood.end() ; itStable++){

				cst3+= -u[(*itStable)];
				cst4+=  u[(*itStable)];

			}
			cst1+= x_w[d] - u[(*itDOI)->v];
			cst2+= u[(*itDOI)->v] -x_w[d];
			cst3+= x_T[d]; 
			cst4+= -x_T[d];
			CC.add(cst1<=1-DELTA);
			CC.add(cst2<=0);
		 	CC.add(cst3<= 0);
			CC.add(cst4<=1-DELTA);
			
			cst5+= alphaDOI[d]+ x_w[d] -x_T[d];
			CC.add(cst5 == 0);
			d++;


		}
		


	}
	else if(typeOfCoeffDoi<3){
		list<C_master_DOI*>::iterator itDOI;
		for(itDOI = M->DOI_var.begin() ; itDOI != M->DOI_var.end() ; itDOI++){
			IloExpr cst(env);
			list<int>::iterator itStable;
			for(itStable = (*itDOI)->neighborhood.begin() ; itStable != (*itDOI)->neighborhood.end() ; itStable++){
				
			
					cst+=  u[(*itStable)];
				

			}
			cst-= - u[(*itDOI)->v];
			if(typeOfCoeffDoi == 1 ){

				CC.add(cst==0);
			}
			else{
				CC.add(cst <= 0);
			}


		}

	}
	

	



	IloExpr cst(env);
	for (int  i =0; i <nb_nodes;i++){
		cst+= u[i];
		ostringstream varname;

		varname.str("");
		varname<<"u_"<<i;
		u[i].setName(varname.str().c_str());
	}
	cst+= f_0 - alpha_0;
	CC.add(cst==0);

	model.add(CC);


	obj = IloAdd(model, IloMaximize(env, 0.0));
	for(int i = 0; i < nb_nodes ; i++){
		obj.setLinearCoef(u[i],1e-4); 
	}



	obj.setLinearCoef(alpha_0,1);


	cplex = IloCplex(model);

	if(nbMaxNode != -1){
		cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, nbMaxNode);
	}

	cplex.setParam(IloCplex::Param::MIP::Pool::Capacity, SIZEPOOL);
	cplex.setParam(IloCplex::Param::MIP::Pool::Replace,1 );  // permet de demander des solutions différentes
	cplex.setOut(env.getNullStream());
	cplex.setWarning(env.getNullStream());

	 //cout<<"**************************** FIN INIT CPLEX *********************"<<endl;





}



void ChvatalGomorySep::setupBranching(SCIP_ConsData data){

//TODO

}
void ChvatalGomorySep::setoutBranching(){


//TODO

}
