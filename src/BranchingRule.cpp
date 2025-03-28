#include"BranchingRule.h"
#include"BranchingHandler.h"
//#define OUTPUT_BRANCHRULE
//#define DEBUG
#define eps 1e-6

using namespace std;


bool isPossibleToBranchOn(C_Graph* G,int v1, int v2){
	//return true;
	if(G->adjacency[v1][v2] == 0 || G->adjacency[v1][v2] ==1){
		return true;
		int adjv1 = 0;
		int k1  =0;
		for(list<int>::iterator it = G->V_nodes[v1].same.begin();it != G->V_nodes[v1].same.end(); it++){
			int k = 0;
			if(G->adjacency[v1][*it] ==4)
				adjv1++;
			
			if(G->adjacency[v2][*it] != 0 && G->adjacency[v2][*it] !=1 )
				return false;
			if(G->adjacency[v2][*it] == 1)
				k1++;
			
			for(list<int>::iterator itt = G->V_nodes[v1].same.begin();itt != G->V_nodes[v1].same.end(); itt++){
				if(*it != *itt && G->adjacency[*itt][*it]  == 4){
					k ++;
				
				}
					
			
			}
			if(k + G->adjacency[v2][*it] > 1 )
				return false;
		}
		if(k1+ G->adjacency[v1][v2] >1 ||adjv1+ G->adjacency[v1][v2] >1)
			return false;
		
		
		k1  =0;
		adjv1 = 0;
		for(list<int>::iterator it = G->V_nodes[v2].same.begin();it != G->V_nodes[v2].same.end(); it++){
		
			if(G->adjacency[v2][*it] ==4)
				adjv1++;
			
			if(G->adjacency[v1][*it] != 0 && G->adjacency[v1][*it] !=1 )
				return false;
				
			if(G->adjacency[v1][*it] == 1)
				k1++;
			int k = 0;
			for(list<int>::iterator itt = G->V_nodes[v2].same.begin();itt != G->V_nodes[v2].same.end(); itt++){
				if(*it != *itt && G->adjacency[*itt][*it]  == 4){
					k ++;
				
				}
					
			
			}
			if(k + G->adjacency[v2][*it] > 1 )
				return false;
		}
		if(k1+ G->adjacency[v1][v2] >1 ||adjv1+G->adjacency[v1][v2] >1)
			return false;
		
		
	
	
		return true;
	
	}

	return false;
	

	/*else if(G->adjacency[v1][v2] ==1){
		for(list<int>::iterator i= G->V_nodes[v1].same.begin() ; i != G->V_nodes[v1].same.end() ; i++){
			if(G->adjacency[*i][v2]==1 || G->adjacency[*i][v2]==5)
				return false;
	
	
		}
		for(list<int>::iterator i= G->V_nodes[v2].same.begin() ; i != G->V_nodes[v2].same.end() ; i++){
			if(G->adjacency[*i][v1]==1 || G->adjacency[*i][v1]==3)
				return false;
	
	
		}
	
		
	}
	else if(G->adjacency[v1][v2] == 2  || G->adjacency[v1][v2] ==3 || G->adjacency[v1][v2] ==4 || G->adjacency[v1][v2] ==5){
		return false;
	
	
	}*/
	
}

SCIP_RETCODE BranchingRule::scip_execlp(SCIP* scip, SCIP_BRANCHRULE* branchrule, SCIP_Bool allowaddcons, SCIP_RESULT* result) {

	#ifdef OUTPUT_BRANCHRULE
		cout << " --------------------- Branching Rule EXECLP ---------------  \n";
		cout << "Nombre de noeuds actuel : " << SCIPgetNNodes(scip) << std::endl;
	#endif

		//cout<<"brancher execlp"<< ceil(SCIPgetLPObjval(scip)) << ";" << SCIPgetPrimalbound(scip)<<endl;
		SCIP_ConsData *consdata;
		SCIP_NODE* node = SCIPgetCurrentNode(scip);
		
		if (node->conssetchg!=NULL) { 
			consdata=SCIPconsGetData(node->conssetchg->addedconss[0]);
			#ifdef OUTPUT_BRANCHRULE
			cout<<"Consdata non null"<<endl;
			#endif
		}
		else {
			consdata=NULL;
			#ifdef OUTPUT_BRANCHRULE
			cout<<"Consdata null"<<endl;
			#endif

		}
		

		int v1;
		int v2;
		double min = 10000;
		list<C_master_var*>::iterator it;
		list<C_master_DOI*>::iterator itDOI;
	//	
	//		for(int i = 0 ; i < G->nb_nodes; i++){
	//		for(int j = 0; j <i ; j++){
	//			cout<<G->adjacency[i][j]<<" ";
	//		}
	//		cout<<endl;
	//	
	//	}
	//	
	//	
	//	cout<<"je commence a faire afficher le calcul des sommes"<<endl;
		G->resetGraphBranching();
		G->modifyGraphInBranching(consdata);
		int avantdoi;
		for(int i = 0 ; i < G->nb_nodes ; i++){
			for(int j = 0 ; j < i ; j++){

				//if(isPossibleToBranchOn(G,j,i)){
					double sum = 0;
					for(it = M->L_var.begin(); it != M->L_var.end() ; it ++){
						
						if((*it)->stable->contains(i) && (*it)->stable->contains(j)){
							sum+= SCIPgetVarSol(scip,  (*it)->ptr);	   				
						} 			
					}
					avantdoi = sum;
					for(itDOI = M->DOI_var.begin(); itDOI != M->DOI_var.end() ; itDOI ++){ 
					
						
						if( (*itDOI)->containsBranch(i,G) && (*itDOI)->containsBranch(j,G)  ){
							sum+= SCIPgetVarSol(scip,  (*itDOI)->ptr);
						
						}
						
					
					}
					//cout<<sum<< " ";
					if(sum >0 && abs(0.5,sum) < abs(0.5,min)  ){
						if(isPossibleToBranchOn(G,j,i)){
							v1 = j;
							v2 = i;
							min = sum;
						}
						else{
							cout<<"ici truc bizarre : "<< G->adjacency[i][j] <<" sum = "<< sum << " avant doi = "<< avantdoi <<endl;
							/*if(G->adjacency[i][j] == 2 || G->adjacency[i][j] ==4){
								for(it = M->L_var.begin(); it != M->L_var.end() ; it ++){
									
									if(((*it)->stable->contains(i) && !(*it)->stable->contains(j)) || ((*it)->stable->contains(j) && !(*it)->stable->contains(i)) && SCIPgetVarSol(scip,  (*it)->ptr) != 0){
										cout<<"problem var co-2-plex"<<endl;			
									} 			
								}
							
							
							}*/
							
							
						}
					}
					
					
				//}
				
			}

		}
		
		//G->resetGraphBranching();
		if (min !=10000){
			//cout<<"\n jsuis passé là \n"<<endl;

			cout<<"branching "<<v1<<" "<<v2<<endl;
			SCIP_NODE *newnode;		
			SCIP_CONS *newcons;
			
			SCIP_NODE *newnode1;		
			SCIP_CONS *newcons1;


			// first node
			SCIPcreateChild(scip, &newnode, 1000.0, SCIPgetLocalTransEstimate(scip));
			create_CteBranch(scip, true, v1, v2,  consdata, &newcons);
			
			
			SCIPaddConsNode(scip, newnode, newcons, NULL);
			SCIPreleaseCons(scip, &newcons);

			// second node
			SCIPcreateChild(scip, &newnode1, 1000.0, SCIPgetLocalTransEstimate(scip));
			create_CteBranch(scip, false, v1, v2,  consdata, &newcons1);
			
			
			
			
			SCIPaddConsNode(scip, newnode1, newcons1, NULL);
			SCIPreleaseCons(scip, &newcons1);
		
			*result = SCIP_BRANCHED;
			
			
		}
		else{
			
			exit(EXIT_FAILURE);
			return SCIP_OKAY;	
			
			for(list<C_master_DOI*>::iterator ittt = M->DOI_var.begin();ittt != M->DOI_var.end();ittt++){


				if(SCIPgetVarSol(scip,(*ittt)->ptr)-round(SCIPgetVarSol(scip,(*ittt)->ptr)) !=0){
					//ofstream res;
					//res.open("erreur.txt",std::ios_base::app);
					//res<<"\n \n \n niqué \n \n \n "<<endl;
					//res.close();
					throw std::invalid_argument("un branchement a déconné");
					exit(EXIT_FAILURE);
					
				}
			}
				throw std::invalid_argument("un branchement a déconné");
			exit(EXIT_FAILURE);
			return SCIP_OKAY;


			

			//cout<<"Every variable is integer!!!!!! \n \n \n"<<endl;
			//*result = SCIP_CUTOFF;
			for(it = M->L_var.begin(); it != M->L_var.end() ; it ++){
						
				if(SCIPgetVarSol(scip,  (*it)->ptr)> 0){
					cout<<SCIPgetVarSol(scip,  (*it)->ptr)<<" : ";	   				
					
				}
				
			}
			cout<<endl;
		}
	
	
	#ifdef DEBUG
		cout << "\n*****END OF Branching Rule EXECLP ****\n";
		cout << "****************************************\n";
	#endif

	return SCIP_OKAY;

}
