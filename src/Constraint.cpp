
#include "Constraint.h"
#include "Cplex_Noproj_algo.h"



int nbCstChavtal = 0;
int nbCstOddCycle = 0;
int nbTriangle = 0;
//////////// Constraints definition ///////////////////



IloNumVar* ChvatalGomoryCst::modifyPricer(Cplex_pricing_algo* pricer){
	IloNumVar* vc =new IloNumVar(pricer->env, 0.0, rhs, ILOINT);
	ostringstream varname;	   
	varname.str("");
	

    	varname<<"ChvatalGomory_"<<nbCstChavtal;
	nbCstChavtal++;
	vc->setName(varname.str().c_str());
	IloRangeArray CC(pricer->env);


	 //ofstream res;
   	 //res.open("constraints.txt",std::ios_base::app);

	IloExpr* cst = new IloExpr(pricer->env);
	(*cst)+= (*vc);
	for (int i = 0; i <   nb_nodes ;i++){

      		
      		(*cst)+=  -pricer->x[i] * (*u)[i]; 
      	//	res<<(*u)[i]<<" ";	
      	
  	}
	
	CC.add((*cst)<=1-0.0001);
	//res<<"\n"<<"";
  	pricer->model.add(CC);
	
	//res.close();
	return vc;

	


}

int ChvatalGomoryCst::coeffOfStable(StableSet* s){
	SCIP_Real result = 0;
	list<int>::iterator it;
	for(it = s->L_nodes.begin(); it != s->L_nodes.end(); it ++){
		result+=  (*u)[(*it)];
		

	}
	return arrondiSup(result);
}


ChvatalGomoryCst::ChvatalGomoryCst(vector<SCIP_Real>* u_,int nb_nodes_){   //give to this constructor, vectors with weigth with less or equal than two decimals
	
		u = u_;
		nb_nodes = nb_nodes_;
		SCIP_Real sum=0;
		for(int i = 0 ; i < u->size(); i ++){
			sum+= (*u)[i];

		}
		rhs = arrondiSup(sum);
}

int ChvatalGomoryCst::coeffDoi(int w, list<int> T, int typeOfCoeff){

	if(typeOfCoeff == 0){
		double sumT=0;
		list<int>::iterator it;
		for(it = T.begin() ; it !=  T.end(); it++){
			sumT+= (*u)[(*it)];

		}
		return floor(sumT)-ceil((*u)[w]);
	}
	else{
		return 0;

	}

}



triangleCst::triangleCst(int u_,int v_, int w_, int nb_nodes_){
	u = u_;
	v = v_;
	w = w_;
	nb_nodes = nb_nodes_;
	tt.push_back(u_);
	tt.push_back(v_);
	tt.push_back(w_);
	rhs = 2;
}


IloNumVar* triangleCst::modifyPricer(Cplex_Noproj_algo* pricer){

	Cplex_pricing_algo* pp = dynamic_cast<Cplex_pricing_algo*> (pricer);

	IloNumVar* vc =new IloNumVar(pp->env, 0.0, 1, ILOINT);
	ostringstream varname;	   
	varname.str("");


    	varname<<"Triangle"<<nbTriangle;
	nbTriangle++;
	vc->setName(varname.str().c_str());
	IloRangeArray CC(pp->env);


	 //ofstream res;
   	 //res.open("constraints.txt",std::ios_base::app);

	IloExpr* cst = new IloExpr(pp->env);
				 


	(*cst)+= (*vc)- pp->x[u] -  pp->x[v]   -pp->x[w]   ;
	for(int i = 0 ; i < pp->G->nb_links ; i++){
		if(pricer->G->V_links[i]->v1 == u || pp->G->V_links[i]->v1 == v || pp->G->V_links[i]->v1 == w ||pp->G->V_links[i]->v2 == u || pp->G->V_links[i]->v2 == v || pp->G->V_links[i]->v2 == w){
			(*cst)-=pricer->y[i];
			
		}
	
	}


	
	CC.add((*cst)<= 0);

  	pp->model.add(CC);

	return vc;

}

IloNumVar* triangleCst::modifyPricer(Cplex_pricing_algo* pricer){

	IloNumVar* vc =new IloNumVar(pricer->env, 0.0, 1, ILOINT);
	ostringstream varname;	   
	varname.str("");


    	varname<<"Triangle"<<nbTriangle;
	nbTriangle++;
	vc->setName(varname.str().c_str());
	IloRangeArray CC(pricer->env);


	 //ofstream res;
   	 //res.open("constraints.txt",std::ios_base::app);

	IloExpr* cst = new IloExpr(pricer->env);
				 


	(*cst)+= (*vc)- pricer->x[u] -  pricer->x[v]   -pricer->x[w]   ;
	


	
	CC.add((*cst)<= 0);

  	pricer->model.add(CC);

	return vc;

}
int triangleCst::coeffOfStable(StableSet* s){
	if( s->contains(u) || s->contains(w) || s->contains(v)){
	
		return 1;
	}
	else{
		return 0 ;
	}

}
vector<SCIP_Real>* triangleCst::getLinearization(){

	vector<SCIP_Real>* result = new vector<SCIP_Real> (nb_nodes);
	
	
	(*result)[u]= 0.5;
	(*result)[v]=0.5;
	(*result)[w] = 0.5;
	return result;

}
int triangleCst::coeffDoi(int ww, list<int> T, int typeOfCoeff){
	return 0;
	int a=0;
	int b;
	

	
	if(ww ==u || ww == v || ww == w ){
		a=1;
	
	}
	else{
		a = 0;
	
	}
	b = 0;
	for(list<int>::iterator it  = T.begin(); it != T.end(); it ++){
		if( *it == u || *it == v || *it ==w){
			b = 1;
		
		}
	
	}
	return b-a;
	if(a==0){
		return 0;	
	}
	if(b==1){
		return 0;
	}
	else{
		return -1;
	}
	return b -a;
}




