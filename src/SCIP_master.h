#ifndef __SCIP_MASTER_H__
#define __SCIP_MASTER_H__


#include <vector>
#include <list>
#include "Graph.h"
#include "Constraint.h"

/* scip includes */
#include "objscip/objscip.h"
#include "objscip/objscipdefplugins.h"

#include "Data.h"

using namespace std;
using namespace scip;


class Constraint;




class C_master_var{
	public:
  
	// Keep a pointer on every variables of the Master program
	SCIP_VAR * ptr;

	// Keep the list of nodes of the stable set corresponding to the variable
	StableSet* stable;
	void printSolution( SCIP * scip , SCIP_SOL *sol);
    
};


class C_master_DOI{
 	public:
	SCIP_VAR* ptr;
	int v;
	list<int> neighborhood;

	bool containsBranch(int i , C_Graph* G){
	
		list<int>::iterator it;
		for(it = neighborhood.begin() ; it!= neighborhood.end() ; it ++){
			if(*it == i){
				return true;
			}
			
		
		}
		/*
		for(int j = 0; j < G->nb_nodes; j ++){
				if(G->adjacency[i][j] == 2 || G->adjacency[i][j] == 4){
					if(contains(j)){
						return true;
					
					}
				
				}
			
			
		}
		*/

		return false;
		


	};
	bool contains(int i ){
		list<int>::iterator it;
		for(it = neighborhood.begin() ; it!= neighborhood.end() ; it ++){
			if(*it == i){
				return true;
			}
		
		}
	
		return false;	
	};

};


class C_master_coloring{
	public:


	// Keep a pointer on every constraint of the Master program
	vector<SCIP_CONS*> V_node_ineq;


	// Keep informations on every variables associated to vertices of the Master program
	list<C_master_var*> L_var;

	// Keep informations on every variables associated to DOI of the Master program
	list<C_master_DOI*> DOI_var;


	//Keep a pointer on every additional constraint of the Master program
	list<SCIP_ROW*> additional_ineq;

  
	list<Constraint*> ctn;

  

	Data _data;

   
	int numberTighIq(){
		int result = 0;
		list<SCIP_ROW*>::iterator it;
		for(it = additional_ineq.begin(); it != additional_ineq.end(); it ++){

			if(SCIProwGetDualsol(*it)>0){
				result++;
			}

		} 
		return result;
		

	}
	
  	void printSolution( SCIP * scip , SCIP_SOL *sol);

};


#endif
