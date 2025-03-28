#include "Graph.h"
#include<bits/stdc++.h>


#define epsilon 0.00001


/****************************  StableSet *****************************/

bool StableSet::contains(int n){

	return nodes[n]==1;

}

bool StableSet::add(int n){
	

	
	if(nodes[n] == 1 )return true;

	int neighbsize = 0;

	for(list<int>::iterator it = L_nodes.begin() ; it != L_nodes.end(); it ++ ){
		if(n == *it)
			return false;
		if(G->adjacency2[n][*it] == 1){
			for(list<int>::iterator itt = L_nodes.begin() ; itt != L_nodes.end(); itt ++ ){
				if((*itt != *it) && G->adjacency2[*it][*itt] ==1)	
					neighbsize++;
		
			}
			neighbsize++;
		}
	}
	if(neighbsize >=2)
		return false;
	

	 
	
	

		nodes[n]=1;
		L_nodes.push_back(n);

		return true;
	
	
	


}

void StableSet::print(){

	list<int>::iterator it;

	for(it = L_nodes.begin(); it != L_nodes.end(); it ++){
		//cout<<(*it)<<" ";


	}
	//cout<<endl;

}
	
void StableSet::initialize(int nb_nodes){
	L_nodes.clear();
	nodes = vector<int>(nb_nodes);
	for(int i = 0; i < nb_nodes; i++){
		nodes[i] = 0;
	
	}

}

int StableSet::size(){

	return L_nodes.size();
}


/****************************  C_link  *******************************/

int C_link::return_other_extrem(int v){
	return (v==v1?v2:v1);
}






/***************************  C_node  *****************************/

bool C_node::test_neighbour(int j){
  list<C_link*>::iterator it;
  for(it=L_adjLinks.begin() ; it !=L_adjLinks.end() ; it++){
    if((*it)->return_other_extrem(num) == j)
      return true;
  }
  return false;
}

bool C_node::test_successor(int j){
  list<C_link*>::iterator it;
  for(it=L_adjLinks.begin() ; it !=L_adjLinks.end() ; it++){
    if((*it)->return_other_extrem(num) == j)
      return true;
  }
  return false;
}

/**************************  C_Graph  ******************************/




void C_Graph::completeIntoMaxStable(StableSet* stable){  // fonction à appeler pour compléter le stable

	for(int i = 0 ; i < nb_nodes ; i++){
	
		stable->add(i);
	
	}


	
		



}

bool C_Graph::respectBranching(StableSet s){
	
	for(list<int>::iterator it = s.L_nodes.begin(); it != s.L_nodes.end(); it ++){
		for(int j = 0 ; j < nb_nodes ; j++){
			if(j != *it){
				if(adjacency[*it][j] == 2 || adjacency[*it][j] == 4){
					if(! s.contains(j)){
						return false;
					
					}
				
				
				}
				
				else if(adjacency[*it][j] == 3 || adjacency[*it][j] == 5){
					if( s.contains(j)){
						return false;
					
					}
				
				}
			}
		
		}
	
	
	}
	return true;



}



void C_Graph::modifyGraphInBranching(SCIP_ConsData* data){ 
	

	SCIP_ConsData* d = data;
	
	resetGraphBranching();
	while(d != NULL){
	
		if(d->same){
		
			if(adjacency2[d->v1][d->v2] == 1){
				adjacency[d->v1][d->v2]=4;
				adjacency[d->v2][d->v1]=4;
			}else{
				adjacency[d->v1][d->v2]= 2;
				adjacency[d->v2][d->v1]= 2;
			//propagateBranchingOverSameSame(d->v1,d->v2);
			}

		}
		else{
		
			if(adjacency2[d->v1][d->v2]  == 1){
				adjacency[d->v1][d->v2]=5;
				adjacency[d->v2][d->v1]=5;
				}
			else{
				adjacency[d->v1][d->v2]= 3;
				adjacency[d->v2][d->v1]=3;
				}
			//propagateBranchingOverSameDiffer(d->v1,d->v2);
		}
		d = d->father_data;
	

	}
	
//	vector<int> size;
//	size.resize(nb_nodes);
//	
//	for(int i = 0 ; i < nb_nodes ; i++){
//		for(int  j = 0 ; j < i ; j++){
//			if(adjacency2[i][j] ==4){
//				size[i]++;
//				size[j]++;
//			
//			
//			}
//		
//		}
//	
//	
//	}
//	for(int i = 0; i < nb_nodes; i++){
//		//cout<<" ceux qui sont avec : "<< i <<" :"<< endl;
//		for(list<int>::iterator it = V_nodes[i].same.begin() ; it != V_nodes[i].same.end(); it ++){
//			//cout<<*it<<" ";
//		
//		
//		}
//		//cout<<endl;
//		if(size[i] > 1 ){
//		
//			
//			//cout<<"YOYOYOYOYOYOYOYOY \n \n \n \n \n \n \n YOYOYOYOYOYOYOYOY \n \n \n \n \n \n \n \n"<<endl;
//		        throw std::invalid_argument("inconsistent branching");
//		        exit(EXIT_FAILURE);
//		
//		}
//	
//	}
}


void C_Graph::propagateBranchingOverSameDiffer(int v1, int v2 ){  

	list<int>::iterator it1;
	list<int>::iterator it2;
	if(adjacency[v1][v2] == 0){
		adjacency[v1][v2]=3;
		adjacency[v2][v1]=3;
	
	}
	else if(adjacency[v1][v2] ==1){
		adjacency[v1][v2]=5;
		adjacency[v2][v1]=5;
	
	}
	else{
		////cout<<"truc bizarre là \n \n \n "<<endl;
		return;
	}
	for(it1 = V_nodes[v2].same.begin(); it1 != V_nodes[v2].same.end(); it1 ++){  
		if(adjacency[v1][*it1] == 0){
			adjacency[v1][*it1]=3;
			adjacency[*it1][v1]=3;

		}
		else if(adjacency[v1][*it1] == 1){
			adjacency[v1][*it1]=5;
			adjacency[*it1][v1]=5;
		
		}
		


	}


	// add edge between v2 and all vertices merged with v1
	for(it1 = V_nodes[v1].same.begin(); it1 != V_nodes[v1].same.end(); it1 ++){

		if(adjacency[v2][*it1] == 0){		
			adjacency[v2][*it1]=3;
			adjacency[*it1][v2]=3;
		
		}
		else if(adjacency[v2][*it1] == 1){
			adjacency[v2][*it1]=5;
			adjacency[*it1][v2]=5;
		
		}

		for(it2 = V_nodes[v2].same.begin(); it2 != V_nodes[v2].same.end(); it2 ++){
		
			
			if(adjacency[*it1][*it2] == 0){
				adjacency[*it1][*it2] = 3;
				adjacency[*it2][*it1] = 3;
			}
			else if(adjacency[*it1][*it2] == 1){
				adjacency[*it1][*it2] = 5;
				adjacency[*it2][*it1] = 5;
			
			}


		}

	}






}







void C_Graph::propagateBranchingOverSameSame(int v1,int v2 ){
	list<int>::iterator it1;
	list<int>::iterator it2;
	
	if(adjacency[v1][v2]==0){
		adjacency[v1][v2]=2;
		adjacency[v2][v1]=2;
	}
	else if(adjacency[v1][v2] = 1){
		adjacency[v1][v2]=4;
		adjacency[v2][v1]=4;
	
	}
	else{
		//cout<<"truc bizarre \n \n \n"<<endl;
	
	}
	if(!(std::find(V_nodes[v2].same.begin(), V_nodes[v2].same.end(), v1) != V_nodes[v2].same.end())){
		V_nodes[v2].same.push_back(v1);
		
	}	
	if(!(std::find(V_nodes[v1].same.begin(), V_nodes[v1].same.end(), v2) != V_nodes[v1].same.end())){
		V_nodes[v1].same.push_back(v2);
	
	}
	for(it1 = V_nodes[v2].same.begin(); it1 != V_nodes[v2].same.end(); it1 ++){  
		if(adjacency[v1][*it1] == 1){
			adjacency[v1][*it1]=4;
			adjacency[*it1][v1]=4;
		
		}
		else if(adjacency[v1][*it1] == 0){
			adjacency[v1][*it1]=2;
			adjacency[*it1][v1]=2;
		
		}
		if(!(std::find(V_nodes[v1].same.begin(), V_nodes[v1].same.end(), *it1) != V_nodes[v1].same.end())){
		
			if(v1 != *it1){
				V_nodes[v1].same.push_back(*it1);
			}
		}
		if(!(std::find(V_nodes[*it1].same.begin(), V_nodes[*it1].same.end(), v1) != V_nodes[*it1].same.end())){
			if(v1 != *it1){
			V_nodes[*it1].same.push_back(v1);
			}
		}
	}
	for(it1 = V_nodes[v1].same.begin(); it1 != V_nodes[v1].same.end(); it1 ++){
		if(adjacency[v2][*it1] == 1){
			adjacency[v2][*it1]=4;
			adjacency[*it1][v2]=4;
		
		}
		else if(adjacency[v2][*it1] == 0){
			adjacency[v2][*it1]=2;
			adjacency[*it1][v2]=2;
		
		}
		if(!(std::find(V_nodes[v2].same.begin(), V_nodes[v2].same.end(), *it1) != V_nodes[v2].same.end())){
			if(v2 != *it1){
				V_nodes[v2].same.push_back(*it1);
			}
		}
		if(!(std::find(V_nodes[*it1].same.begin(), V_nodes[*it1].same.end(), v2) != V_nodes[*it1].same.end())){
			if(v2 != *it1){
				V_nodes[*it1].same.push_back(v2);
			}
		}
		for(it2 = V_nodes[v2].same.begin(); it2 != V_nodes[v2].same.end(); it2 ++){
			if(*it1 != *it2){
				if(adjacency[*it1][*it2] == 0){
					adjacency[*it1][*it2] = 2;
					adjacency[*it2][*it1] = 2;
				}
				else if(adjacency[*it1][*it2] == 1){
					adjacency[*it1][*it2] = 4;
					adjacency[*it2][*it1] = 4;
				}
				if(!(std::find(V_nodes[*it1].same.begin(), V_nodes[*it1].same.end(), *it2) != V_nodes[*it1].same.end())){
					if(*it1 != *it2){
						V_nodes[*it1].same.push_back(*it2);
					}
				}
				if(!(std::find(V_nodes[*it2].same.begin(), V_nodes[*it2].same.end(), *it1) != V_nodes[*it2].same.end())){
					if(*it2 != *it1){
						V_nodes[*it2].same.push_back(*it1);
					}
				}
			}
		}
	}
	for(int i = 0 ; i < nb_nodes ; i++){
		if(adjacency[v1][i] == 3 || adjacency[v1][i] == 5){
		
			if(adjacency[v2][i] == 1){
				adjacency[v2][i] = 5 ;
				adjacency[i][v2] = 5;
			}
			else if(adjacency[v2][i] == 0){
				adjacency[v2][i] = 3 ;
				adjacency[i][v2] = 3 ; 
			}
			for(it2 = V_nodes[v2].same.begin(); it2 != V_nodes[v2].same.end(); it2 ++){
				if(adjacency[*it2][i] == 1 ){
					adjacency[*it2][i] = 5;
					adjacency[i][*it2] = 5;	
				}
				else if(adjacency[*it2][i] ==0){
					adjacency[*it2][i] = 3;
					adjacency[i][*it2] = 3;	
				}
			}
		}
		if(adjacency[v2][i] == 3 || adjacency[v2][i] == 5)  {
			if(adjacency[v1][i] == 1){
				adjacency[v1][i] = 5 ;
				adjacency[i][v1] = 5;
			}
			else if(adjacency[v1][i] == 0){
				adjacency[v1][i] = 3 ;
				adjacency[i][v1] = 3 ; 
			}
			
			
			for(it2 = V_nodes[v1].same.begin(); it2 != V_nodes[v1].same.end(); it2 ++){
				if(adjacency[*it2][i] == 1 ){
					adjacency[*it2][i] = 5;
					adjacency[i][*it2] = 5;	
				}
				else if(adjacency[*it2][i] ==0){
					adjacency[*it2][i] = 3;
					adjacency[i][*it2] = 3;	
				
				
				}
			}	
		}
	}
}

void C_Graph::resetGraphBranching(){
//	for(int i = 0; i < nb_nodes ; i++){
//		V_nodes[i].same.clear();

//		for(int j = 0 ; j<i ; j++){
//			
//			if(adjacency[i][j] ==2){
//				adjacency[i][j]=0;
//				adjacency[j][i]=0;
//			
//			}
//			else if(adjacency[i][j] == 3){
//			
//				adjacency[i][j]=0;
//				adjacency[j][i]=0;
//			
//			}
//				
//			else if(adjacency[i][j] == 4){  // reset the adjacency matrix
//				adjacency[i][j]=1;
//				adjacency[j][i]=1;
//			}
//			else if(adjacency[i][j] == 5){
//				adjacency[i][j]=1;
//				adjacency[j][i]=1; 
//			
//			}

//		} 

//	}
	for(int i = 0 ; i< nb_nodes; i++){
		for( int j = 0 ; j<nb_nodes ; j++){
			adjacency[i][j] =adjacency2[i][j];
			

	
			
			
		
		
		}
	
	}
}



StableSet* C_Graph::findMaxStableSet(vector<pair<int,float>> weight){

	StableSet* result = new StableSet;
	result->G = this;
	result->initialize(nb_nodes);

	
	////cout<<"glout has been launched "<<endl;


	sort(weight.begin(), weight.end(), [](pair<int,float> a, pair<int,float> b) -> bool { return a.second > b.second;});
	int vertex;
	while(weight.size()>0){
		vertex= weight[0].first;
		
		StableSet* copy = new StableSet;
        copy->G = this;
        copy->initialize(nb_nodes);

        for(list<int>::iterator iter =  result->L_nodes.begin() ; iter != result->L_nodes.end(); iter ++){
            copy->add(*iter);
        }
		
		if( copy->add(vertex)){
		    bool b = true;
		    weight.erase(weight.begin());
		    for(vector<pair<int,float>>::iterator it = weight.begin(); it != weight.end() ; it++ ){
			    if( adjacency[vertex][(*it).first] == 3 ||adjacency[vertex][(*it).first] ==5){
				    weight.erase(it);
				    it--;

			    }
		    }
		    
		    
	        
	        for(vector<pair<int,float>>::iterator it = weight.begin(); it != weight.end() ; it++ ){
			    if( adjacency[vertex][(*it).first] == 2 ||adjacency[vertex][(*it).first] ==4){	        
	                if(! copy->add((*it).first)){
	                    b = false;
	                }
	            }
	                
	        }
	        
	        
	        if(b){
	            free(result);
	            result = copy;
	        }
	        else{
	            free(copy);
	        }
	        

		
		}
		
		else{
		    free(copy);
		    weight.erase(weight.begin());
		}

	}
	
    
    
	return result;




}






void C_Graph::getGloutColoring(list<StableSet*>& coloration){
	
	StableSet* stablemax;

	vector<int> sommets = vector<int>(nb_nodes);
	

	list<int>::iterator it;

	for(int i = 0 ; i < nb_nodes; i ++){
		stablemax= new StableSet;
		stablemax->G = this;



		stablemax->initialize(nb_nodes);
		





		for(int j = 0 ; j < nb_nodes ; j ++){   // on cherche le premier sommet non couvert de la liste de sommets
			if(sommets[j]==0){
				stablemax->add(j);
				j = nb_nodes;

			}


		}

		if(stablemax->size()>0){
			completeIntoMaxStable(stablemax);   // on complète le wingleton en stable max
			// on met à jour la liste de sommets 
			for(it = stablemax->L_nodes.begin() ; it != stablemax->L_nodes.end() ; it ++){
				sommets[*it] = 1;


			}
			coloration.push_back(stablemax);
		}
		else{
			i = nb_nodes;
		}
		
		
		


	}




}

int C_Graph::edgeNumber(int v1 , int v2){

	for(int i = 0 ; i < nb_links ; i++){
	
		if((V_links[i]->v1 == v1 && V_links[i]->v2 == v2) || (V_links[i]->v2 == v1 && V_links[i]->v1 == v2)){
			return i ;}
	
	
	}


	return -1;
}

list<int> C_Graph::getMaximalClique(int v1, int v2){
	list<int> result;
	result.push_back(v1);
	result.push_back(v2);
	list<int>::iterator it;
	for( int i = 0; i < nb_nodes; i++){
	
		bool b = true;
		for( it=result.begin() ; it != result.end() ; it ++ ){
			if(*it == i)
				b=false;
			if(adjacency[i][*it] == 0)
				b = false;
		
		
		}
		if(b)
			result.push_back(*it);
	
	}
	return result;

}

C_Graph  C_Graph::getComplementary(){
	int i ,j , nb;
	C_Graph g;
	C_link* a;


	g.nb_nodes = nb_nodes;
	g.nb_links = nb_nodes*(nb_nodes-1) /2 - nb_links;
	g.adjacency.resize(nb_nodes);
	for( i = 0 ; i < nb_nodes; i++){

		g.adjacency[i]= vector<int>(nb_nodes,1);

    	}
	
	g.V_nodes.resize(nb_nodes);
        g.V_links.resize(g.nb_links);

        for (i=0;i<nb_nodes;i++){
        	g.V_nodes[i].num = i;
        	g.V_nodes[i].L_adjLinks.clear();
        	g.V_nodes[i].weight=0;
		g.adjacency[i][i] = 0;
        }
	

	for(i = 0 ; i < nb_links ; i++){
		g.adjacency[V_links[i]->v1][V_links[i]->v2] = 0;
		g.adjacency[V_links[i]->v2][V_links[i]->v1] = 0;

	}
	
	nb = 0;
	
	for(i = 0; i < nb_nodes ; i ++ ) {

		for(j = 0 ; j <i ; j ++){
			if(g.adjacency[i][j]==1){
				a = new C_link;
				a->v1 = min(i,j);
				a->v2 = max(i,j);
				a->weight=0;
				a->num=nb;
				g.V_links[nb] = a;
				nb++;
			}

		}

	}
	
	return g;


    
	
}



// A FINIR



void C_Graph::findViolatedCycleIq(vector<StableSet*>* stables, vector<float>& cost, list<list<int>*> &result, int rhs=3){ // détecte de manière gloutonne si une inégalité de cycle a été détectée
	


	int pred1,pred2;
	list<int>* cycle;
	pair<float,list<int>> res;
	vector<int>  ss;
	vector<int> sommet_interdit;
	int v1,v2;
	float violation;
	pair<float,int> newSommet;
	bool oncontinue;
	
	for(int i = 0 ; i < nb_links ; i ++){

		
		cycle=new list<int>();
		sommet_interdit = vector<int>(nb_nodes,0);
		violation = 0;	
		v1 = V_links[i]->v1;
		v2 = V_links[i]->v2;
		
		cycle->push_back(v1);						//on ajoute au cycle le premier sommet de l'arête
		ss = vector<int> (stables->size(), 0);
		
		cycle->push_back(v2);						//on ajoute au cycle le second sommet de l'arête
		pred1 = v2;
		pred2 = v1;
		
		computeCostOfVertice( stables ,   &ss, cost, v1,&res);	//on calcul le coût de chaque sommet et on met à jour l'ensemble des stables
					
		majSS(&res.second, &ss);
		
		violation = violation + res.first;

	        computeCostOfVertice( stables ,   &ss, cost, v2, &res );
	       
		majSS(&res.second, &ss);

		
		violation = violation + res.first;
		sommet_interdit[v1]=1;
		sommet_interdit[v2]=1;
		newSommet= ajouteSommetAuCycle( cycle, stables ,  &ss, cost ,  v1, v2, rhs-violation,&sommet_interdit);
		oncontinue = false;
		violation = violation + newSommet.first; 

		if(newSommet.first !=-1){		
			sommet_interdit[newSommet.second]=1;
			oncontinue = adjacency[v1][newSommet.second] == 0 || adjacency[v2][newSommet.second] == 0 ;
			if(cycle->front()  == v1){
				pred2 = v2;
				v2 = cycle->back();
				

			}
			else{
				pred1 = v1;
				v1 = cycle->front();
			}
		}
		
	
		while(oncontinue){
			
			newSommet= ajouteSommetAuCycle( cycle, stables ,  &ss, cost ,  v1, v2, rhs-violation,&sommet_interdit);// on ajoute un sommet dans le cycle
			
			if(newSommet.second != -1 ){			
				sommet_interdit[newSommet.second]=1;									// on met à jour la liste de sommets interdits
				violation = violation + newSommet.first;
											// on met à jour la violation du cycle partiel
				if(cycle->front() ==newSommet.second ){									// on met à jour les extrémités du cycle
					pred1=v1;
					v1 = newSommet.second;
				}	
				else{
					pred2=v2;
					v2= newSommet.second;
				}
				
				
				
				newSommet= ajouteSommetAuCycle( cycle, stables ,  &ss, cost ,  v1, v2, rhs-violation,&sommet_interdit);// on ajoute un sommet dans le cycle
				if(newSommet.second!=-1){					
					sommet_interdit[newSommet.second]=1;									// on met à jour la liste de sommets interdits
					violation = violation + newSommet.first;								// on met à jour la violation du cycle partiel
					if(cycle->front() ==newSommet.second ){									// on met à jour les extrémités du cycle
						pred1=v1;
						v1 = newSommet.second;
					}	
					else{
						pred2=v2;
						v2= newSommet.second;
						
					}
			
					if(!  verifyChord(cycle,pred1,pred2,v1,v2)  ){
						
						if(adjacency[v1][v2] == 1){
							
							if(!cycleIn(result,cycle)){

								result.push_back(cycle);
							}
                                                        else
                                                                free(cycle);
							oncontinue = false;
						}
					}else{
						oncontinue=false;
						free(cycle);

					}

				}
				else{
				oncontinue = false;
				free(cycle);
				}
			
				
	 		}
			else{
				oncontinue  = false;
				free(cycle);

			}
			
			


		}
	}


	
	

	



}

bool C_Graph::verifyChord(list<int>* cycle,int pred1,int pred2,int v1, int v2){ //renvoit true si le cycle partiel contient une corde passant par ses extrémités
	////cout<<pred1<<" "<<pred2 << " "<<v1 << " "<<v2<<endl;

	list<int>::iterator itt;
	long unsigned int c= 1;
	for(itt = next(cycle->begin(),1); c<cycle->size(); itt++){
		if(adjacency[cycle->front()][(*itt)] == 1 && pred1!= (*itt) && v2 != (*itt) ){
		
			return true;

		}
		c++;

		
	}
	c =1 ;
	list<int>::reverse_iterator it;
	for(it = next(cycle->rbegin(),1); c < cycle->size(); it++){
		c++;
		if(adjacency[cycle->back()][(*it)] == 1 && pred2!= (*itt) && (*itt) != v1 ){
			
			return true;

		}

		
	}
	return false;


}









pair<float,int> C_Graph::ajouteSommetAuCycle(list<int>* cycle,vector<StableSet*>* stables , vector<int>* ss,vector<float>& cost ,  int v1, int v2, float placedispo,vector<int>* sommet_interdit){ //retourne le coût du sommet ajouté
	 list <C_link*>::iterator it;
	 
	list<int> mmm; 
	
	pair<float,list<int>> min = make_pair(100,mmm);

	
	pair<float,list<int>> m;
	int verticeMin2,k;
	int verticeMin1 = 0;
	 for( it= V_nodes[v1].L_adjLinks.begin() ; it != V_nodes[v1].L_adjLinks.end(); it++ ){
		
		k = (*it)->return_other_extrem(v1);

		if((*sommet_interdit)[k] != 1){
			
			 computeCostOfVertice(stables,ss,cost,k,&m);
			if(m.first < min.first){
				verticeMin1 = k;
				min.first = m.first;
				min.second = m.second;

			}
		}

	}

	verticeMin2 = -1;
	for( it= V_nodes[v2].L_adjLinks.begin() ; it != V_nodes[v2].L_adjLinks.end(); it++ ){
		
		k = (*it)->return_other_extrem(v2);

		if((*sommet_interdit)[k]!=1){		
			 computeCostOfVertice(stables,ss,cost,k,&m);
			
			if(m.first < min.first){
				verticeMin2 = k;
				min.first = m.first;
				min.second = m.second;

			}
		}

	}

	if(min.first < placedispo-epsilon){  // le sommet peut être ajouté

		if(verticeMin2 == -1){ // le sommet à ajouter est stocké dans verticeMin1, le sommet doit donc être ajouté à gauche dans la liste

			cycle->push_front(verticeMin1);
			
			verticeMin2= verticeMin1;
			
			
			

		}
		else{ // le sommet à ajouter est stocké dans verticeMin2, le sommet doit donc être ajouté à droite dans la liste.
			
			cycle->push_back(verticeMin2);
			
			

		}


	
		majSS(&min.second, ss);

		return make_pair(min.first,verticeMin2);


	}else{ // le sommet ne peut pas être ajouté, donc on renvoit stop
		return make_pair(-1,-1);

	}




	


}


// FINIT

bool cycleIn(list<list<int>*>& l1, list<int>* l2){  // return true si la liste l1 contient liste l2 

	list<list<int>*>::iterator it ;
	for(it = l1.begin() ; it != l1.end() ; it ++){
		if(compareCycle((*it),l2)) return true;

	}
	return false;
}




bool compareCycle(list<int>* l1, list<int>* l2){  // return true si les deux listes contiennent les mêmes sommets
	list<int>::iterator it1;
	list<int>::iterator it2;
	bool b;
	for(it1 = l1->begin() ; it1 != l1->end() ; it1++){
		b = false;
		for(it2 = l2->begin(); it2 != l2->end() ; it2++){
			if( (*it1) == (*it2) ) b = true;


		}

		if(!b) return false;
	

	}


	return true;
}

int in(list<int> cycle, int vertice){

	int position = 0;
	list<int>::iterator it;

	for(it = cycle.begin() ; it != cycle.end() ; it++){
		if((*it) == vertice) {

			return position;
		}
		position=position+1;
	}	
	return -1;


}

void majSS(list<int>* indices, vector<int>* ss){

	list<int>::iterator it;
	for(it= indices->begin(); it != indices->end(); it ++){
		(*ss)[(*it)]=1;

	}

}




void computeCostOfVertice(vector<StableSet*>* stables , vector<int>*  ss,vector<float>& cost, int v1, pair<float,list<int>>*  m){
	float sum = 0;
	list<int> ll;
	m->second.clear();
	
	for(long unsigned int i = 0 ; i < stables->size(); i ++){
		
		if((*ss)[i]==0 && (*stables)[i]->contains(v1)){
			sum = sum + cost[i];
			
			m->second.push_back(i);
		}



	}
		
	m->first = sum;
	


}

void C_Graph::deleteVertex(int i){
	
	vector<vector<int>> adj(nb_nodes-1) ;
	vector<C_node> V_node(nb_nodes-1) ;
	
	for(int j = 0; j < nb_nodes-1 ; j++){
		adj[j] = vector<int>(nb_nodes-1);
		V_node[j].num = j ;
		V_node[j].L_adjLinks.clear();
      		V_node[j].weight=1;
		
	}


	int deltaI = V_nodes[i].L_adjLinks.size(); 
	//vector<C_node> V_node(nb_nodes-1);
	vector<C_link*> newV_links(nb_links-deltaI);


	int v1;
	int v2;

	int k =0;
	for(int l = 0; l < nb_links ; l++){

		if(V_links[l]->v1 != i && V_links[l]->v2 != i){

			if(V_links[l]->v1 > i ){
				v1=  V_links[l]->v1-1;

			}
			else{
				v1=  V_links[l]->v1;

			}

			if(V_links[l]->v2 > i ){
				v2=  V_links[l]->v2-1;

			}
			else{
				v2=  V_links[l]->v2;

			}
			newV_links[k] = new  C_link;
			newV_links[k]->v1 = v1;
			newV_links[k]->v2 = v2;
			newV_links[k]->weight = 0;
			
			adj[v1][v2] = 1;
			adj[v2][v1] = 1;
			V_node[v1].L_adjLinks.push_back(newV_links[k]);
			V_node[v2].L_adjLinks.push_back(newV_links[k]);
			k++;

		}

	}

	adjacency = adj;  //

	V_nodes = V_node;

	V_links = newV_links;
	nb_nodes = V_nodes.size();
	nb_links = newV_links.size();

	



}

void C_Graph::cleanGraph(){
	int j = 0;
	//removing vertex of degree 0
	////cout<<nb_nodes<<endl;
	bool asupprimer;
	for(int i = 0 ; i < nb_nodes ; i++){
		for(int j = i+1 ; j < nb_nodes ; j ++){
			 asupprimer=true;
			 
			for(int k = 0 ; k < nb_nodes ; k++){
				if(V_nodes[i].test_neighbour(k)){
					if(!V_nodes[j].test_neighbour(k)){

						asupprimer=false;
					}

				}

			}


			if(asupprimer){
				////cout<<i<< " " << j <<endl;
				deleteVertex(i);
				i = nb_nodes;
				j = nb_nodes;
				break;
			}



		}

	}
	if(asupprimer){
		
		cleanGraph();
		
	}


}




void C_Graph::read_undirected_DIMACS(istream & fic){
  if (!fic){
    //cout<<"File Error"<<endl;
  }
  else{
    int k,i,j;
    string m1,m2;
    list<C_link>::iterator it;
    C_link *a;

    fic>>m1;
    fic>>m2;
		
    // Jump the file description and go to the data
    while (((m1!="p")&&(m2!="edge"))||((m1!="p")&&(m2!="col"))){
      m1=m2;
      fic>>m2;
    }

    directed=false;
    
    fic>>nb_nodes;
    fic>>nb_links;
		
	this->adjacency.resize(nb_nodes);	
	this->adjacency2.resize(nb_nodes);
    //adjacency = vector<vector<int>>(nb_nodes);
    
    int cb_added=0;
    for( i = 0 ; i < nb_nodes; i++){

		//adjacency[i]= vector<int>(nb_nodes);
		this->adjacency[i].resize(nb_nodes);
		this->adjacency2[i].resize(nb_nodes);
    }
    for( i = 0 ; i < nb_nodes; i++){

		for( j = 0 ; j < nb_nodes; j++){

		//adjacency[i]= vector<int>(nb_nodes);
		this->adjacency[i][j]=0;
		this->adjacency2[i][j]=0;
    }
    }
	

    V_nodes.resize(nb_nodes);
    V_links.resize(nb_links);

    for (i=0;i<nb_nodes;i++){
      V_nodes[i].num = i;
      V_nodes[i].L_adjLinks.clear();
      V_nodes[i].weight=1;
    }
    //cout<<"liste des aretes"<<endl;
    for (k=0;k<nb_links;k++){
    

	      fic>>m1;
	      fic>>i;
	      fic>>j;
     if( this->adjacency[i-1][j-1]!=1 &&   this->adjacency[j-1][i-1]!=1){
	      this->adjacency[i-1][j-1]=1;
	      this->adjacency[j-1][i-1]=1;
	      this->adjacency2[i-1][j-1]=1;
	      this->adjacency2[j-1][i-1]=1;
	      a=new C_link;
	      a->num=cb_added;
	      a->v1=min(i-1,j-1);
	      a->v2=max(i-1,j-1);
	      ////cout<<i<< " "<< j <<endl;
	      a->weight=0;
	      V_nodes[i-1].L_adjLinks.push_back(a);
	      V_nodes[j-1].L_adjLinks.push_back(a);
	      V_links[cb_added] = a;
	      cb_added++;
      }
    }
    nb_links = cb_added;
	
  }
  
	/*for(int k = 1 ; k < nb_nodes ; k++){
		
		bool b = true;
		for(int m = 0 ; m <nb_nodes ; m++){
			if(adjacency[k][m] == 1)
				b=false;

		}
		if(b){
			adjacency[k][0]=true;
			adjacency[0][k]=true;
			 C_link * a=new C_link;
			 a->v1 = 0;
			a->v2=k;
			 a->weight=0;
			V_nodes[0].L_adjLinks.push_back(a);
      			V_nodes[k].L_adjLinks.push_back(a);
      			V_links.push_back( a);
			nb_links++;
		}


	}*/
	//cleanGraph();
	
}




void C_Graph::write_dot_G(string InstanceName){
  list<C_link>::iterator it;
  int i,k;

  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G.dot";

  ofstream fic(FileName.str().c_str());

  if (!directed) {
  
  fic<<"graph G {"<<endl;

  for(i=0 ; i<nb_nodes ; i++)
      fic<<"  "<<V_nodes[i].num<<"[shape = octagon]"<<endl;

  for(k=0 ; k<nb_links ; k++)
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
  
  fic<<"}"<<endl;

  }
  else{


  }

  

  fic.close();
  ostringstream commande; 
  commande.str("");
  commande<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G.pdf "<< FileName.str().c_str()<<endl;
  //cout<<commande.str().c_str();
  if(system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}
  return;
}



void C_Graph::write_dot_G_stableset(string InstanceName, vector<int>& stable){
  int i,k;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_stable.dot";

  ofstream fic(FileName.str().c_str());
  
  fic<<"graph G {"<<endl;
  
  for(i=0 ; i<nb_nodes ; i++){
    if (stable[i]>1-epsilon)
      fic<<"  "<<V_nodes[i].num<<"[shape = octagon]"<<endl;
    else
      if (stable[i]<epsilon)
	fic<<"  "<<V_nodes[i].num<<"[shape = octagon, color=white]"<<endl;
  }
  
  for(k=0 ; k<nb_links ; k++){
    if ((stable[V_links[k]->v1]>1-epsilon)&&(stable[V_links[k]->v2]>1-epsilon))
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
    else
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\" [color=white];"<<endl;
  }
  
  fic<<"}"<<endl;
		
  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G_stable.pdf "<< FileName.str().c_str()<<endl;
  //cout<<commande.str().c_str();
  if (system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}


}





void C_Graph::write_dot_G_color(string InstanceName, vector<int>& coloring){
  int i,k;
  ostringstream FileName; 
  FileName.str("");
  FileName <<InstanceName.c_str() << "_G_color.dot";

  vector <string> colors;
  colors.push_back("green");
  colors.push_back("blue");
  colors.push_back("red");
  colors.push_back("cyan");
  colors.push_back("yellow");
  colors.push_back("magenta");
  colors.push_back("darkorchid");
  colors.push_back("darkorange");
  colors.push_back("deeppink");
  colors.push_back("forestgreen3");
  colors.push_back("indigo");
  colors.push_back("midnightblue");
  colors.push_back("violetred");

  int chi=0;
  for (i=0;i<nb_nodes;i++)
    if (chi<coloring[i]) chi=coloring[i];

  
  if(chi >= (int)colors.size()){
    //cout<<"We only have 13 colors and this solutions needs "<<chi<<" colors... some nodes will have wrong colors!"<<endl;
  }
  
  ofstream fic(FileName.str().c_str());
  fic<<"graph G {"<<endl;
  
  for(i=0 ; i<nb_nodes ; i++){
    fic<<"  "<<V_nodes[i].num<<"[shape = octagon, style = filled , fillcolor = "<<colors[(coloring[V_nodes[i].num]) % colors.size()]<<" ]"<<endl;
  }
  
  for(k=0 ; k<nb_links ; k++){
      fic<<"  \""<<V_links[k]->v1<<"\"--\""<<V_links[k]->v2<<"\";"<<endl;
  }
  
  fic<<"}"<<endl;
		
  fic.close();

  ostringstream commande; 
  commande.str("");
  commande<<"dot -Tpdf -o "<<InstanceName.c_str() << "_G_color.pdf "<< FileName.str().c_str()<<endl;
  //cout<<commande.str().c_str();
  if (system(commande.str().c_str())){cout<<"PDF generated successfully"<<endl;}
  

}


list<pair<int,list<int>>*> C_Graph::getDualOptCouple(){
	list<list<int>> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){

		o = getMaxStableOfNeighborhoodIncluded(i);
		////cout<<"finished"<<endl;
		if(o.size() != 0){
			for(list<list<int>>::iterator it= o.begin() ; it != o.end() ; it ++){
			
				pair<int,list<int>>* blabla =new pair<int,list<int>>(i,*it); 

				result.push_back(blabla);
			
			}
			//result.push_back(new pair<int,list<int>>(i,o.front()));
		}

		
	}

	return result;
	


}

list<pair<int,list<int>>*> C_Graph::getDualOptCouple2(){
	list<list<int>> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){

		o = getMaxStableOfNeighborhoodIncluded2(i);
		////cout<<"finished"<<endl;
		if(o.size() != 0){
			for(list<list<int>>::iterator it= o.begin() ; it != o.end() ; it ++){
			
				pair<int,list<int>>* blabla =new pair<int,list<int>>(i,*it); 

				result.push_back(blabla);
			
			}
			//result.push_back(new pair<int,list<int>>(i,o.front()));
		}

		
	}

	return result;
	


}

list<pair<int,list<int>>*> C_Graph::getDualOptCouple3(){
	list<list<int>> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){
		////cout<<"i "<< i<<endl;
		o = getMaxStableOfNeighborhoodIncluded(i);
		////cout<<"finished"<<endl;
		if(o.size() != 0){
			for(list<list<int>>::iterator it= o.begin() ; it != o.end() ; it ++){
			
				pair<int,list<int>>* blabla =new pair<int,list<int>>(i,*it); 

				result.push_back(blabla);
			
			}
		}


		
	}
	result.sort([](pair<int,list<int>>*  pair1, pair<int,list<int>>* pair2)
        {
           
            return pair1->second.size() < pair2->second.size();
        });
	list<pair<int,list<int>>*> result1;
	int k = result.size()/2;
	int compteur =0;
	
	for(list<pair<int,list<int>>*>::iterator it = result.begin() ; (it != result.end()&&compteur < k ); it ++){
		result1.push_back(*it);
		compteur++;
	
	}

	
	
	return result1;
	


}

list<pair<int,list<int>>*> C_Graph::getDualOptCouple4(){
    list<int> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){
		////cout<<"i "<< i<<endl;
		o = getNeighborhoodIncluded(i);
		if(o.size() != 0){
			for(list<int>::iterator it= o.begin() ; it != o.end() ; it ++){
			    if(*it != i){
			        list<int> rr;
			        rr.push_back(*it);
				    pair<int,list<int>>* blabla =new pair<int,list<int>>(i,rr); 
				    result.push_back(blabla);
				}
				for(list<int>::iterator itt= o.begin() ; itt != o.end() ; itt ++){
				    if(*itt != *it){
				        bool b = true;
				        for(int j = 0; j<nb_nodes ; j++ ){
				            if(adjacency[*it][j] ==1 &&  adjacency[*itt][j] ==1 && j!= i){
				                b = false;
				            
				            }
				        
				        
				        }
				        if(b){
				            list<int> rr;
				            rr.push_back(*it);
				            rr.push_back(*itt);
				            pair<int,list<int>>* blabla =new pair<int,list<int>>(i,rr);
				            result.push_back(blabla); 
				        
				        }
				        
				    }
				}			
			
			}
		}
		
		
		
	}
	return result;

}

bool C_Graph::isCo2plex(list<int> s){

	int cbquivapas = 0;
	
	for(list<int>::iterator it = s.begin() ; it != s.end() ; it ++){
		int n = 0;
		
		for(list<int>::iterator it2 = s.begin() ; it2 != s.end() ; it2++){
			if(*it != *it2 && adjacency2[*it][*it2] ==1){
				n = n+1;
			
			}
			
			
		
		}
		if(n > 1 ){
			cbquivapas++;
		
		}
	
	
	}
	if(cbquivapas>= 1 ){
	
		//cout<<"YOYOYOYOYOYOYOYOY \n \n \n \n \n \n \n YOYOYOYOYOYOYOYOY \n \n \n \n \n \n \n \n"<< cbquivapas<<endl;
			        throw std::invalid_argument("c pas un co-2-plex");
			        exit(EXIT_FAILURE);
			return false;
	
	}


	return true;

}

bool C_Graph::isItCovering(list<list<int>> sol){
	////cout<<"size of the covering"<< sol.size()<<endl;
	vector<bool> oo;
	oo.resize(nb_nodes);
	for(int i = 0 ; i < nb_nodes ; i ++){
		oo[i]=false;
	
	}
	
	list<list<int>>::iterator it;
	for(it = sol.begin() ; it != sol.end() ; it ++){
		if(! isCo2plex(*it)){
			throw std::invalid_argument("c pas un co-2-plex dans le test covering ");
			        exit(EXIT_FAILURE);
		
		}
		for(list<int>::iterator itt = (*it).begin() ; itt != (*it).end() ; itt++){
			oo[*itt] = true;
		
		}
	
	
	}
	
	for(int i = 0 ; i < nb_nodes ; i++){
		if( ! oo[i])
			return false;
	
	}
	return true;
}


list<list<int>> C_Graph::getMaxStableOfNeighborhoodIncluded2(int k ){

	list<int> neigh_k = getNeighborhoodIncluded(k);
	
	////cout<<"for vertex "<< k << endl;
	
	
	
	list<int> result;
	list<int> remaining;
	list<list<int>> result_full;
//	for(list<int>::iterator it = neigh_k.begin() ; it != neigh_k.end() ; it++){
//		result.push_back(*it);
//		result_full.push_back(result);
//		result.clear();
//	

//	}
//	return result_full;

	bool b;
	if(neigh_k.size() == 0){

		return result_full;
	}
	else{
	
		list<int>::iterator it;
		
		list<int> toDelete;
		
		for(list<int>::iterator lmlm = neigh_k.begin() ; lmlm != neigh_k.end() ; lmlm++){
			result.push_back(*lmlm);
			result_full.push_back(result);
			result.clear();
		
		
		}
		
		
		
		return result_full;
	}

}


list<list<int>> C_Graph::getMaxStableOfNeighborhoodIncluded(int k){

	list<int> neigh_k = getNeighborhoodIncluded(k);
	
	////cout<<"for vertex "<< k << endl;
	
	
	
	list<int> result;
	list<int> remaining;
	list<list<int>> result_full;
//	for(list<int>::iterator it = neigh_k.begin() ; it != neigh_k.end() ; it++){
//		result.push_back(*it);
//		result_full.push_back(result);
//		result.clear();
//	

//	}
//	return result_full;

	bool b;
	if(neigh_k.size() == 0){

		return result_full;
	}
	else{
	
		list<int>::iterator it;
		
		list<int> toDelete;
		
		for(list<int>::iterator lmlm = neigh_k.begin() ; lmlm != neigh_k.end() ; lmlm++){
			result.push_back(*lmlm);
			result_full.push_back(result);
			result.clear();
		
		
		}
		
			
			
			
		
		while(neigh_k.size() != 0){
			
			int v = -1;

			int size_n=0;

			
			for(it = neigh_k.begin() ; it != neigh_k.end() ; it ++){
				if( V_nodes[(*it)].L_adjLinks.size() > size_n){
					size_n = V_nodes[(*it)].L_adjLinks.size();
									
					v = (*it);
				}


			}
			neigh_k.remove(v);


//			v = neigh_k.front();
//			neigh_k.pop_front();
//			
			
			
			result.push_back(v);
			
			
			////cout<<"adding : "<< v<<endl;
			
			
			if(neigh_k.size()>0){
				for(it = neigh_k.begin() ; it != neigh_k.end() ; it ++ ){
					bool b = true;
					int it2;
					int it3;

					for(it2 = 0;it2 < nb_nodes; it2++ ){
						if(adjacency[v][it2] == 1 && it2 != v){
							if(!b)
								break;
							////cout<<"une fois"<<endl;
							for(it3 = 0;it3< nb_nodes; it3++){
								if(adjacency[*it][it3]==1){
								////cout<<"deux fois "<<endl;
									if(  it2==it3 && it2!=k && b){
										b = false;
										////cout<<"adding remaining o : "<< *it<<endl;
										remaining.push_back(*it);
										it = neigh_k.erase(it);
										
										
										it--;
										
										
										////cout<<"juskla"<<endl;
										break;
									}
								
							
								}
						      }
						}
					
					
					}
								//					//cout<<"juskla1"<<endl;
				
					
					if(adjacency[v][*it] == 1 && b ){
						remaining.push_back(*it);
						it = neigh_k.erase(it);
						////cout<<"adding remaining : "<< *it<<endl;
						it--;
						


					}
					

				}
			}
			if(neigh_k.size()==0){

				neigh_k = remaining;
				remaining.clear();
				
				if(result.size()>=2)
					result_full.push_back(result);
				result.clear();
			
			}
				
			

		}


	}
	
	return result_full;
	


}

list<list<int>> C_Graph::getMaxStableOfNeighborhoodIncludedNONADJ(int k){

	list<int> neigh_k = getNeighborhoodIncludedNONADJ(k);
	
	////cout<<"for vertex "<< k << endl;
	
	
	
	list<int> result;
	list<int> remaining;
	list<list<int>> result_full;
//	for(list<int>::iterator it = neigh_k.begin() ; it != neigh_k.end() ; it++){
//		result.push_back(*it);
//		result_full.push_back(result);
//		result.clear();
//	

//	}
//	return result_full;

	bool b;
	if(neigh_k.size() == 0){

		return result_full;
	}
	else{
	
		list<int>::iterator it;
		
		list<int> toDelete;
		
		for(list<int>::iterator lmlm = neigh_k.begin() ; lmlm != neigh_k.end() ; lmlm++){
			result.push_back(*lmlm);
			result_full.push_back(result);
			result.clear();
		
		
		}
		
			
			
			
		
		while(neigh_k.size() != 0){
			cout<<neigh_k.size()<<endl;
			int v = neigh_k.front();

			int size_n=0;

		    
			neigh_k.remove(v);


//			v = neigh_k.front();
//			neigh_k.pop_front();
//			
			
			
			result.push_back(v);
			
			
			////cout<<"adding : "<< v<<endl;
			
			
			if(neigh_k.size()>0){
				for(it = neigh_k.begin() ; it != neigh_k.end() ; it ++ ){
					bool b = true;
					int it2;
					int it3;

					for(it2 = 0;it2 < nb_nodes; it2++ ){
						if(adjacency[v][it2] == 1 && it2 != v){
							if(!b)
								break;
							////cout<<"une fois"<<endl;
							for(it3 = 0;it3< nb_nodes; it3++){
								if(adjacency[*it][it3]==1){
								////cout<<"deux fois "<<endl;
									if(  it2==it3 && it2!=k && b){
										b = false;
										////cout<<"adding remaining o : "<< *it<<endl;
										remaining.push_back(*it);
										it = neigh_k.erase(it);
										
										
										it--;
										
										
										////cout<<"juskla"<<endl;
										break;
									}
								
							
								}
						      }
						}
					
					
					}
								//					//cout<<"juskla1"<<endl;
				
					
					if(adjacency[v][*it] == 1 && b ){
						remaining.push_back(*it);
						it = neigh_k.erase(it);
						////cout<<"adding remaining : "<< *it<<endl;
						it--;
						


					}
					

				}
			}
			if(neigh_k.size()==0){

				neigh_k = remaining;
				remaining.clear();
				
				if(result.size()>=2)
					result_full.push_back(result);
				result.clear();
			
			}
				
			

		}


	}
	
	return result_full;
	


}

list<pair<int,list<int>>*> C_Graph::getDualOptCouple5(){ // each (w,{t}), w adjacent to t or not but only add the vertex of largest neighborhood
  
    list<int> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){
		////cout<<"i "<< i<<endl;
		o = getNeighborhoodIncludedNONADJ(i);
		if(o.size() != 0){
		    int v = o.front();
			for(list<int>::iterator it= o.begin() ; it != o.end() ; it ++){
			    if(V_nodes[v].L_adjLinks.size() < V_nodes[*it].L_adjLinks.size())   
			        v = *it;
			
			
		    }
		    list<int> l;
		    l.push_back(v);
		    pair<int,list<int>>* p = new pair<int,list<int>>(i,l);
		    result.push_back(p);
		}
		
		
		
	}
	return result;
  
  
}
list<pair<int,list<int>>*> C_Graph::getDualOptCouple6(){ // each (w,{t}), w adjacent to t

    list<int> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){
		////cout<<"i "<< i<<endl;
		o = getNeighborhoodIncludedNONADJ(i);
		if(o.size() != 0){
			for(list<int>::iterator it= o.begin() ; it != o.end() ; it ++){
			    list<int> l;
		        l.push_back(*it);
		        pair<int,list<int>>* p = new pair<int,list<int>>(i,l);
		        result.push_back(p);
			
			
		    }
		    
		}
		
		
		
	}
	return result;




}

list<pair<int,list<int>>*> C_Graph::getDualOptCouple7(){ // each (w,T) with w not necessarily complete to T

    list<list<int>> o;
	list<pair<int,list<int>>*> result;
	for(int i = 0 ; i < nb_nodes ; i++){

		o = getMaxStableOfNeighborhoodIncludedNONADJ(i);
		cout<<"finished"<<endl;
		if(o.size() != 0){
			for(list<list<int>>::iterator it= o.begin() ; it != o.end() ; it ++){
			
				pair<int,list<int>>* blabla =new pair<int,list<int>>(i,*it); 

				result.push_back(blabla);
			
			}
			//result.push_back(new pair<int,list<int>>(i,o.front()));
		}

		
	}

	return result;


}


list<int> C_Graph::getNeighborhoodIncludedNONADJ(int k){
    list<C_link*>::iterator it;
	list<C_link*>::iterator itt;
	int v;
	int u ;
	bool b;
	list<int> result;
	//cout<<"niehborhood of vertex "<< k<<endl;
	for(int j = 0; j < nb_nodes; j++){

		if(j!=k){
	
			
			b = true;

			for(int i = 0; i < nb_nodes ; i++){
				if(adjacency[i][j] == 1 && adjacency[i][k]==0 && k!=i && j!=i)
					b=false;
			
			}


	//		for(itt = V_nodes[v].L_adjLinks.begin() ; itt != V_nodes[v].L_adjLinks.end() ; itt++){
	//			u = (*itt)->return_other_extrem(v);
	//			if(k != u){
	//				b = b && adjacency[k][u];
	//				
	//			}
	//		


	//		}

			if(b){
				////cout<<j<<endl;
				result.push_back(j);
				
			}
		}


	}
	return result;



}






list<int> C_Graph::getNeighborhoodIncluded(int k){
	list<C_link*>::iterator it;
	list<C_link*>::iterator itt;
	int v;
	int u ;
	bool b;
	list<int> result;
	////cout<<"niehborhood of vertex "<< k<<endl;
	for(int j = 0; j < nb_nodes; j++){

		if(adjacency[j][k] ==1 && j!=k){
	
			
			b = true;

			for(int i = 0; i < nb_nodes ; i++){
				if(adjacency[i][j] == 1 && adjacency[i][k]==0 && k!=i && j!=i)
					b=false;
			
			}


	//		for(itt = V_nodes[v].L_adjLinks.begin() ; itt != V_nodes[v].L_adjLinks.end() ; itt++){
	//			u = (*itt)->return_other_extrem(v);
	//			if(k != u){
	//				b = b && adjacency[k][u];
	//				
	//			}
	//		


	//		}

			if(b){
				////cout<<j<<endl;
				result.push_back(j);
				
			}
		}


	}
	return result;



}


/*
#ifndef DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

bool contain(list<int> l, int x){
	list<int>::iterator it;
	
	for(it = l.begin() ; it != l.end() ; it ++){
		if(x == *it)
			return true;
	
	
	}
	return false;
}


bool containn(list<list<int>> l , list<int> l1){
	list<list<int>>::iterator it;
	bool b = false;
	for(it = l.begin() ; it != l.end() ; it ++){
		bool k = true;
		
		if( (*it).size() == l1.size()){
			for(list<int>::iterator itt = l1.begin() ; itt != l1.end() ; itt++){
				if(! contain(*it, *itt)){
					k = false;
				}
			
			}
		
		}
		else{
			k = false;
		
		}
		
		b= b || k;
		
	
	
	}
	
	return b;


}


TEST_CASE("testing DOI generator") {
    
    
    	ifstream fic("testdoc.col");
	C_Graph G;
    	G.read_undirected_DIMACS(fic);
    	
    	 
    	 
    	 list<list<int>> ll = G.getMaxStableOfNeighborhoodIncluded(2);
    	 //for(list<list<int>>::iterator it = ll.begin() ; it != ll.end() ; it ++){
    	 
    	 
    	 //} 
    	 
    	 
    	 list<int> tt;
    	 tt.push_back(0);
    	 CHECK(ll.size() == 1);
    	 CHECK(containn(ll,tt));
    	 
    	 l = G.getNeighborhoodIncluded(3);
    	 CHECK(contain(l,5));
    	 CHECK(contain(l,2));
    	 CHECK(contain(l,1));
    	 CHECK(contain(l,0));
    	 ll = G.getMaxStableOfNeighborhoodIncluded(3);
    	 CHECK(ll.size()>= 3 );
   	ll = G.getMaxStableOfNeighborhoodIncluded(6);
   	CHECK(ll.size() == 0);
   	
    	 
    	
    	
    	fic.close();
    	
//    
}
#endif
*/
