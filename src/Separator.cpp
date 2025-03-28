#include "Separator.h"


SCIP_RETCODE Separator::scip_execlp(SCIP* scip, SCIP_SEPA* sepa, SCIP_RESULT* result, unsigned int i){
	*result = SCIP_DIDNOTFIND;
	sepaCalled++;
	//cout<<"start separator"<<endl;
	auto start = chrono::steady_clock::now();

	if(separate(scip,result)){
		nbConstraints += addConstraints(scip,sepa,result);

		*result=SCIP_SEPARATED;
	}
	//cout<<"end separator"<<endl;
	auto end = chrono::steady_clock::now();
	sepaTime += chrono::duration_cast<chrono::milliseconds>(end - start).count();

	return SCIP_OKAY;
}
SCIP_RETCODE Separator::scip_execsol(SCIP* scip, SCIP_SEPA* sepa, SCIP_RESULT* result, unsigned int i){
	*result = SCIP_DIDNOTFIND;
	sepaCalled++;
	//cout<<"start separator"<<endl;
	auto start = chrono::steady_clock::now();
	if(separate(scip,result)){
		nbConstraints += addConstraints(scip,sepa,result);

		*result=SCIP_SEPARATED;
	}
	//cout<<"end separator"<<endl;

	auto end = chrono::steady_clock::now();
	sepaTime += chrono::duration_cast<chrono::milliseconds>(end - start).count();

	return SCIP_OKAY;
}
SCIP_RETCODE Separator::scip_init(SCIP* scip,SCIP_SEPA* sepa){
	//cout<<"init separator"<<endl;
	init();

	return SCIP_OKAY;
}


int Separator::addConstraints(SCIP* scip, SCIP_RESULT* result){


	int res = 0;
	list<Constraint*>::iterator it;
	list<C_master_DOI*>::iterator itDoi;

	for(it = ctn.begin(); it != ctn.end(); it ++){
		
		SCIP_ROW* row;
    		SCIP_CALL( SCIPcreateEmptyRow(scip, &row, "ChvatalGomory",(*it)->rhs, SCIPinfinity(scip) , FALSE, TRUE, TRUE) );
		SCIP_CALL( SCIPcacheRowExtensions(scip, row) );
		list<C_master_var*>::iterator ite;
		for(ite = M->L_var.begin(); ite != M->L_var.end(); ite++){  
		
			SCIP_CALL( SCIPaddVarToRow(scip, row,  (*ite)->ptr, (*it)->coeffOfStable((*ite)->stable)) ); 

		}
		
		if( M->DOI_var.size()>0){
			for(itDoi = M->DOI_var.begin() ; itDoi != M->DOI_var.end() ; itDoi ++){
				
				SCIP_CALL( SCIPaddVarToRow(scip, row,  (*itDoi)->ptr, (*it)->coeffDoi((*itDoi)->v, (*itDoi)->neighborhood, typeOfCoeffDoi)) );



			}
		}

		SCIP_CALL( SCIPflushRowExtensions(scip, row) );	
		if( SCIPisCutEfficacious(scip, NULL, row) ) //|| objvalue >= 0.5)
		{

			
			
			unsigned int b;
			if( SCIPaddCut(scip, NULL, row, TRUE,&b) ){
				//cout<<"yo"<<endl;
				res++;
				*result = SCIP_SEPARATED;
				M->additional_ineq.push_back(row);
				M->ctn.push_back(*it);
				nbCtGen++;
				
			}else{
				delete(*it);
				delete(row);
			}
		}
		
		
	}


	
	return res;


}

int Separator::addConstraints(SCIP* scip,SCIP_SEPA* sepa, SCIP_RESULT* result){


	int  nbCt= 0;
	list<Constraint*>::iterator it;
	list<C_master_DOI*>::iterator itDoi;


	for(it = ctn.begin(); it != ctn.end(); it ++){

			SCIP_ROW* row;
	    		SCIP_CALL( SCIPcreateEmptyRowSepa(scip, &row, sepa,"ct",(*it)->rhs, SCIPinfinity(scip) , FALSE, TRUE, TRUE) );
			SCIP_CALL( SCIPcacheRowExtensions(scip, row) );
			list<C_master_var*>::iterator ite;


			for( ite=M->L_var.begin(); ite != M->L_var.end(); ite++){
				SCIP_CALL( SCIPaddVarToRow(scip, row, (*ite)->ptr, (*it)->coeffOfStable((*ite)->stable)) ); 

			}  
			
			
			if(M->DOI_var.size()>0){
				for(itDoi = M->DOI_var.begin() ; itDoi != M->DOI_var.end() ; itDoi ++){
					
					SCIP_CALL( SCIPaddVarToRow(scip, row,  (*itDoi)->ptr, (*it)->coeffDoi((*itDoi)->v, (*itDoi)->neighborhood, typeOfCoeffDoi)) );



				}

			}
		SCIP_CALL( SCIPflushRowExtensions(scip, row) );
				
		if( SCIPisCutEfficacious(scip, NULL, row) ) //|| objvalue >= 0.5)
		{
			
			
			
			unsigned int b;
			if( SCIPaddCut(scip, NULL, row, TRUE,&b) ){
				
				nbCt++;
				*result = SCIP_SEPARATED;
				M->additional_ineq.push_back(row);
				M->ctn.push_back(*it);

//				
			}
			else{
				delete(*it);
				delete(row);
			}
		}
//			
		
	}
	

	
	return nbCt;
}



////////////////// NULL SEPARATOR

/*NullSeparator::NullSeparator(){

	return ;
}*/

NullSeparator::NullSeparator(SCIP* scip,const char* p_name,C_master_coloring *M): Separator(scip,p_name,M){}	


bool NullSeparator::separate(SCIP* scip,SCIP_RESULT* result){ return false;}
void NullSeparator::init(){return ;}

void NullSeparator::setupBranching(SCIP_ConsData data){return ;}
void NullSeparator::setoutBranching(){return ;}
