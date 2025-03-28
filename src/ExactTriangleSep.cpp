
#include "ExactTriangleSep.h"
#define epsilonn 1e-5

#define SIZEPOOL1 5

ExactTriangleSep::ExactTriangleSep(SCIP* scip_,C_master_coloring* _master,const char* p_name, C_Graph G_):  Separator(scip,p_name,_master){
	
	M = _master;

	G = G_;
	scip=scip_;
	
	}
bool ExactTriangleSep::separate(SCIP* scip,SCIP_RESULT* result){


	*result =SCIP_DIDNOTFIND ;
	
	int nb_stables =M->L_var.size(); 

	list<C_master_var*>::iterator it;  // on récupère les valeurs des variables et les stables en base 

	
	ctn.clear();
	
	for(list<vector<int>*>::iterator itTriangles = triangles.begin() ; itTriangles != triangles.end(); itTriangles++){
		SCIP_Real sum =0;
		for(it = M->L_var.begin(); it != M->L_var.end(); it++){
			
			if((*it)->stable->contains((**itTriangles)[0]) || (*it)->stable->contains((**itTriangles)[1])|| (*it)->stable->contains((**itTriangles)[2])){
				sum+= arrondiS(SCIPgetSolVal(scip, NULL, (*it)->ptr));
			
			}



		
		}
		list<C_master_DOI*>::iterator it2;
		for(it2 = M->DOI_var.begin(); it2 != M->DOI_var.end() ; it2++){
			int a = 0;
			int b=0;
			int coeff;
				if((*it2)->v == (**itTriangles)[0] || (*it2)->v == (**itTriangles)[1] || (*it2)->v == (**itTriangles)[2]){
			
					a=1;
			
				}
			for(list<int>::iterator nei = (*it2)->neighborhood.begin() ; nei != (*it2)->neighborhood.end() ; nei++){
				if(*nei == (**itTriangles)[0] || *nei == (**itTriangles)[1] || *nei == (**itTriangles)[2]){
					b=1;
				
				}
			if(a==0){
				coeff = 0;
			}
			else if(b == 0){
				coeff = 0;
	
			}
			else{
				coeff=-1;
			}
			sum=sum+= 0 * arrondiS(SCIPgetSolVal(scip, NULL, (*it2)->ptr));
			
			
			}
			
	
			
		
		
		}
		
		if(sum < 2- epsilonn){
			//cout<<"violée"<<(**itTriangles)[0] << " "<<(**itTriangles)[1] << " "<<(**itTriangles)[2] <<endl;
			triangleCst* t = new triangleCst((**itTriangles)[0],(**itTriangles)[1],(**itTriangles)[2], G.nb_nodes);
			//cout<<"created"<<endl;
			ctn.push_back(t);
			
		}
		else{
			//cout<<"non violée"<<endl;
		
		}
		if(ctn.size()>SIZEPOOL1){
			break;
			return true;
		}
	}
	
		
	bool cvcv= ctn.size()>0;

	return cvcv;


}
void ExactTriangleSep::init(){
//	triangles.clear();
//    vector<int>* r = new vector<int>(0);
//    r->push_back(4);
//    r->push_back(5);
//    r->push_back(6);
//    triangles.push_back(r);
//    return;
	for(int i = 0 ; i < G.nb_nodes ; i++){
		for(int j = 0 ; j < i ; j++){
			for(int h = 0 ; h < j ; h++){
				//if((G.adjacency[i][j] == 1 && G.adjacency[i][h]==1 && G.adjacency[j][h]==0) || (G.adjacency[i][j] == 1 && G.adjacency[i][h]==0 && G.adjacency[j][h]==1) || (G.adjacency[i][j] == 0 && G.adjacency[i][h]==1 && G.adjacency[j][h]==1)){
				if(G.adjacency[i][j] + G.adjacency[i][h] + G.adjacency[j][h]>= 2){
					vector<int>* r = new vector<int>(0);
					r->push_back(i);
					r->push_back(j);
					r->push_back( h);
					
					triangles.push_back(r);
				
				}
			
			
			}
		
		
		}
	
	
	
	}
	//cout<<"init done : "<< triangles.size() <<endl;


}

void ExactTriangleSep::setupBranching(SCIP_ConsData data){



}
void ExactTriangleSep::setoutBranching(){



}

