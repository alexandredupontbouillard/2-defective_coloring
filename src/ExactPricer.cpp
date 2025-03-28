

#include "ExactPricer.h"
#include <tuple>


ExactPricer::ExactPricer(SCIP* scip_, const char* p_name, C_Graph* GG, C_master_coloring * MM,int priority) : ObjPricerColoring(scip_,p_name,priority,MM){
		//cout<<"constructor exact pricer called"<<endl;
		G=GG; 
		M=MM;
		A_cplex.create_MIP(G,-1);
		a =new NoCutting();
		//cout<<"constructor exact pricer finished"<<endl;
	}


StableSet* ExactPricer::coloring_pricing(SCIP* scip, SCIP_RESULT* result){


	//cout<<"**************PRICER EXACT************"<<endl;




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
	//cout<<"juskla"<<endl;


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


	A_cplex.modify_pricer( M->ctn);



	A_cplex.set_objective_coefficient(lambda,rho);
	bool stable_found = A_cplex.find_stableset(stable,cplex_objvalue);
	
	
	
	

	if(! isSSnegativeRC(scip,stable))
        recovery(scip);




    free(cplex_objvalue);

	return stable;






}

void ExactPricer::setupBranching(SCIP_ConsData data){ 
	setoutBranching();
	A_cplex.setupBranching(data);
	}
void ExactPricer::setoutBranching(){ 
	A_cplex.setoutBranching();
}

bool intersectionEmpty(StableSet* S, list<int> b){
    
    for(list<int>::iterator it = b.begin(); it != b.end() ; it++){
        if( S->contains(*it)){
            return false;
        }
    }
    
    return true;
}


void ExactPricer::recovery(SCIP* scip){
    //recovery desactivated
    if(!activate_recovery)
        return;
    
    if(M->DOI_var.size() > 0){
        vector<pair<float,StableSet*>> solCo2Plex;
        vector<float> coverage(G->nb_nodes);
        
       
        vector<float> coverageTriangle(M->ctn.size());
        
        
        
        for(list<C_master_var*>::iterator it = M->L_var.begin() ; it != M->L_var.end() ; it++){
            float b = SCIPgetSolVal(scip, NULL, (*it)->ptr);
            StableSet* S= (*it)->stable;
            pair<float,StableSet*> p;
            p.first = b;
            p.second = S;
            solCo2Plex.push_back(p);
            for(list<int>::iterator itt = S->L_nodes.begin() ; itt != S->L_nodes.end() ; itt++){
                coverage[*itt]+= b;
            
            }
            
            int currentT = 0;
            for(list<Constraint*>::iterator itTriangle = M->ctn.begin(); itTriangle != M->ctn.end() ; itTriangle++){
                coverageTriangle[currentT]+= b * (*itTriangle)->coeffOfStable(S);
                currentT+=1;
            }  
        }
        
        
        
        list<tuple<int,list<int>,float,SCIP_VAR*>> solDOI;
        for(list<C_master_DOI*>::iterator it = M->DOI_var.begin() ; it != M->DOI_var.end() ; it++ ){
            int w = (*it)->v;
            list<int> T = (*it)->neighborhood;
            float value = SCIPgetSolVal(scip, NULL, (*it)->ptr);
            if(value >0 ){
                tuple<int,list<int>,float,SCIP_VAR*> elem;
                get<0>(elem) = w;
                get<1>(elem) = T;
                get<2>(elem) = value;
                get<3>(elem) = (*it)->ptr;
                solDOI.push_back(elem);
            }
            coverage[w]-= value;
            for(list<int>::iterator itt = T.begin() ; itt!= T.end() ; itt++){
                coverage[*itt]+= value;
            
            }
            int currentT = 0;
            for(list<Constraint*>::iterator itTriangle = M->ctn.begin(); itTriangle != M->ctn.end() ; itTriangle++){
                coverageTriangle[currentT]+= value * (*itTriangle)->coeffDoi(w,T,sep->typeOfCoeffDoi);
                currentT+=1;
            }  
            
        }
        if(solDOI.size() == 0){

            return;
        }

        while(solDOI.size() != 0){
            
            tuple<int,list<int>,float,SCIP_VAR*> pi = solDOI.front();
            float doiv = get<2>(pi);
            bool b = false;
            for(int i = 0; i< solCo2Plex.size() ; i++){

                if(solCo2Plex[i].second->contains(get<0>(pi)) && intersectionEmpty(solCo2Plex[i].second, get<1>(pi))  &&  solCo2Plex[i].first != 0){
                   b = true;
                   float Co2v= solCo2Plex[i].first;           
                   float min;
                   if(Co2v < doiv)
                        min = Co2v;
                    else{
                        min = doiv;
                        solDOI.pop_front();
                        

                    }
                    
                    
                    
                   solCo2Plex[i].first-= min;
                   get<2>(pi) -= min;
                   coverage[get<0>(pi)]-= min;
                   
                   int currentT = 0;
                   for(list<Constraint*>::iterator itTriangle = M->ctn.begin(); itTriangle != M->ctn.end() ; itTriangle++){
                        coverageTriangle[currentT]-= min * (*itTriangle)->coeffDoi(get<0>(pi),get<1>(pi),sep->typeOfCoeffDoi);
                        coverageTriangle[currentT]+= min * (*itTriangle)->coeffOfStable(solCo2Plex[i].second);
                        if(coverageTriangle[currentT] <2){
                            SCIP_ROW* row;
                             SCIPcreateEmptyRow(scip, &row, "bound",0, 0 , FALSE, TRUE, TRUE) ;
                             SCIPcacheRowExtensions(scip, row) ;
                             SCIPaddVarToRow(scip, row,  get<3>(pi), 1 );
                            unsigned int b;
                            SCIPaddCut(scip, NULL, row, TRUE,&b);
                            return;
                        
                        
                        } 
                        currentT+=1;
                   }  

                   for(list<int>::iterator k = solCo2Plex[i].second->L_nodes.begin() ; k !=solCo2Plex[i].second->L_nodes.end() ; k++){
                        coverage[*k]+= min;
                   
                   }
                   
                   for(int j = 0; j<G->nb_nodes ; j++){
                    if(coverage[j] <1){
                        SCIP_ROW* row;
                         SCIPcreateEmptyRow(scip, &row, "bound",0, 0 , FALSE, TRUE, TRUE) ;
                         SCIPcacheRowExtensions(scip, row) ;
                         SCIPaddVarToRow(scip, row,  get<3>(pi), 1 );
                        unsigned int b;
                        SCIPaddCut(scip, NULL, row, TRUE,&b);
                        return;
                    }
                   }
                   
                    if(min == doiv)
                        break;
                }
                    
            

            }
            
            if(!b){
            
                        SCIP_ROW* row;
                        SCIPcreateEmptyRow(scip, &row, "bound",0, 0 , FALSE, TRUE, TRUE) ;
                        SCIPcacheRowExtensions(scip, row) ;
                        SCIPaddVarToRow(scip, row,  get<3>(pi), 1 );
                        unsigned int b;
                        SCIPaddCut(scip, NULL, row, TRUE,&b);
                        return;
                        
            }
        
        }
        
        
        
        
    

    }

}









