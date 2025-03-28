#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/* scip includes */
#include "objscip/objscip.h"
#include "objscip/objscipdefplugins.h"
#include "Graph.h"
#include "SCIP_master.h"
#include "ExactPricer.h"
#include "MaxNodePricer.h"
#include "GloutPricer.h"
#include "Alternance.h"
#include "Separator.h"
#include <string.h>
#include "ChvatalGomorySep.h"
#include "BranchingHandler.h"
#include "BranchingRule.h"
#include "ExactTriangleSep.h"
#include "ExactPricerProj.h"
#include "MaxNodePricerProj.h"
#include "Cplex_Noproj_algo.h"
//#include "doctest.h"
/* namespace usage */
using namespace std;
using namespace scip;

#define ADDOPT FALSE

#define NBMAXCONSITER 2000


//#define SCIP_DEBUG   // Si non commente, affiche le log de SCIP



// name option k(pricing alternance) doi

void writeinter(list<C_master_DOI*> l1,list<list<int>> l2,SCIP* scip);
bool isInVar(C_master_coloring M, StableSet* s);
int getOpt(char * filename);

int main(int argc, char** argv)
{
	char * name,*nameext; //, *nameextsol;
	int i;

	if(argc < 3){
		cerr<<"usage: "<<argv[0]<<" <DIMACS file name> <mode>"<<endl; 
		return 1;
	}

	srand(1);
  
	name= new char[40];
	nameext= new char[40];
	//nameextsol= new char[40];
	//doctest::Context(argc, argv).run(); 


	name=strcat(name,argv[1]);
	nameext=strcat(nameext,argv[1]);


	ifstream fic(nameext);
  
	C_Graph G;

	G.read_undirected_DIMACS(fic);

	//////////////
	//////  SCIP INITIALIZATION
	//////////////

	SCIP *scip=NULL;

	C_master_coloring Master;


	// initialize SCIP environment 
	SCIPcreate(&scip);
	srand(1);
	SCIPprintVersion(scip, NULL);
	//SCIPinfoMessage(scip, NULL, "\n");

	// include default plugins 
	SCIPincludeDefaultPlugins(scip);

	// set verbosity parameter 
	#ifdef SCIP_DEBUG
	SCIPsetIntParam(scip, "display/verblevel", 5);
	
	// SCIPsetBoolParam(scip, "display/lpinfo", TRUE); 
	#endif

	// create empty problem 
	SCIPcreateProb(scip, "COL", 0, 0, 0, 0, 0, 0, 0);

	
	
	//SCIP_CALL( SCIPsetBoolParam(scip, "pricing/delvars", TRUE));
	//SCIP_CALL( SCIPsetIntParam(scip,"lp/colagelimit",1));
	//SCIP_CALL( SCIPsetRealParam(scip,"separating/maxbounddist",1.0));


	////////////////////////
	//////  INEQUALITIES
	////////////////////////



	// Set covering constraints
	

	Master.V_node_ineq.resize(G.nb_nodes, (SCIP_CONS*) NULL);

	char con_name[255];
	for (i = 0; i < G.nb_nodes; i++)
	{
		SCIP_CONS* con = NULL;
		(void) SCIPsnprintf(con_name, 255, "C%d", i);
		SCIPcreateConsLinear( scip, &con, con_name, 0, NULL, NULL,
				    1.0,   // lhs 
				    SCIPinfinity(scip),   // rhs  SCIPinfinity(scip) if >=1
				    true,  // initial
				    true, // separate 
				    true,  // enforce 
				    true,  // check 
				    true,  // propagate 
				    false, // local 
				    true,  // modifiable 
				    false, // dynamic 
				    false, // removable 
				    false  /* stickingatnode */);
		SCIPaddCons(scip, con);
		Master.V_node_ineq[i] = con; 
	}

	/////////////////////
	// Add variables corresponding to the glout coloring
	////////////////////
	SCIP_VAR* sol;

	Master.L_var.clear();
	char var_name[255];
	list<StableSet*> firstColor;
	//if(argv[5][0] != '2'){
		
		
		G.getGloutColoring(firstColor);
		list<StableSet*>::iterator it;
		list<int>::iterator itt;
		
		i = 0;

		for (it = firstColor.begin() ;it != firstColor.end();it++){
		SCIPsnprintf(var_name, 255, "V_%d",i);
		i++;
		SCIPdebugMsg(scip, "new variable <%s>\n", var_name);


		C_master_var* var=new C_master_var;

		SCIPcreateVar(scip, &(var->ptr), var_name,
			  0,                     // lower bound
			  1e20,      // upper bound
			   1,                     // objective
			   SCIP_VARTYPE_INTEGER, // variable type
			   true, false, NULL, NULL, NULL, NULL, NULL);

		  
		// add new variable to the list of variables to price into LP (score: leave 1 here) 
		SCIPaddVar(scip, var->ptr);

		var->stable=(*it);

		// add coefficients into the set covering constraints 
//		cout<<"les co-2-plexes : "<<endl;
		for(itt = (*it)->L_nodes.begin(); itt != (*it)->L_nodes.end(); itt++){
//		cout<<*itt << " ";
		SCIPaddCoefLinear(scip, Master.V_node_ineq[*itt], var->ptr, 1.0);
		}

//		cout<<endl;



		Master.L_var.push_back(var);

		}
		
		if(ADDOPT){
		
			

			int res = getOpt(nameext);
			
			
			SCIPcreateVar(scip, &sol, "optvar",
			  0,                     // lower bound
			  1e20,      // upper bound
			   res,                     // objective
			   SCIP_VARTYPE_INTEGER, // variable type
			   true, false, NULL, NULL, NULL, NULL, NULL);
			   
			   SCIPaddVar(scip, sol);
			 for(int i = 0; i < G.nb_nodes ; i++){
			 	
			 	SCIPaddCoefLinear(scip, Master.V_node_ineq[i], sol, 1.0);
			 
			 }
			 

		  
	
	
	
		}
	
	
	


		
	if(atoi(argv[4]) ==0){
	
		list<pair<int,list<int>>*> DOI;
		
		if(atoi(argv[7]) == 1)
			DOI = G.getDualOptCouple();
		else if(atoi(argv[7]) == 2)
			DOI = G.getDualOptCouple2();
		else if(atoi(argv[7]) == 3 )
		    DOI = G.getDualOptCouple3();
		else if(atoi(argv[7]) == 4 )
		    DOI = G.getDualOptCouple4();
		else if(atoi(argv[7]) == 5 )
		    DOI = G.getDualOptCouple5();
		else if(atoi(argv[7]) == 6 )
		    DOI = G.getDualOptCouple6();
		else if(atoi(argv[7]) == 7 )
		{   cout<<"jquiql  a"<<endl;
		    DOI = G.getDualOptCouple7();
		    cout<<"pas fdini"<<endl;
		    }
		    
		    
		    
		
		else
		    exit(EXIT_FAILURE);
		
	
		if(DOI.size()==0){
			return 0;
		}	


		list<pair<int,list<int>>*>::iterator itDOI;
		list<int>::iterator DOIVertex;
		int kkk=0;
		
		for(itDOI= DOI.begin() ; itDOI != DOI.end(); itDOI++){
			C_master_DOI* var=new C_master_DOI;
	     
	     		char var_namee[255];
			SCIPsnprintf(var_namee, 255, "DOI_%d",kkk);
			SCIPdebugMsg(scip, "doi variable <%s>\n", var_name);
			SCIPcreateVar(scip, &(var->ptr), var_namee,
				   0,                     // lower bound
				   1e20,//G.getNeighborhoodIncluded((*itDOI)->first).size(),      // upper bound
				   0,                     // objective
				   SCIP_VARTYPE_INTEGER, // variable type
				   true, false, NULL, NULL, NULL, NULL, NULL);

			  kkk++;
			// add new variable to the list of variables to price into LP (score: leave 1 here) 

			cout<<"nouvelle doi " << (*itDOI)->first<<" ";
			var->v=(*itDOI)->first;
			var->neighborhood = (*itDOI)->second;

			SCIPaddCoefLinear(scip, Master.V_node_ineq[var->v], var->ptr, -1.0);

		
				for(DOIVertex = var->neighborhood.begin(); DOIVertex!=var->neighborhood.end(); DOIVertex++){
					SCIPaddCoefLinear(scip, Master.V_node_ineq[*DOIVertex], var->ptr, 1.0);
					cout<<*DOIVertex<<endl;
				}
						SCIPaddVar(scip, var->ptr);
			//cout<<endl;



			Master.DOI_var.push_back(var);


		}

		if(argv[5][0] =='6')
			return 0;
		
//		if(atoi(argv[4]) ==1){
//			ofstream res;
//		
//			res.open("../nb_doi.txt",std::ios_base::app);
//		

//			
//			res<<nameext<<";"<< DOI.size() << ";" << firstColor.size()<<endl; 
//			return 0;
//		}

	
	}
	
	
	ExactPricer* pricer_ptr;
	static const char* PRICER_NAME = "Pricer_Coloring";

	MaxNodePricer* pricerRacine;
	static const char* PRICER_NAME_RACINE = "pricer racine";

	GloutPricer* pricerGlout;
	static const char* PRICER_NAME_GLOUT = "pricer glouton";
	
	
	ExactPricerProj* pricer_ptr_proj;
	static const char* PRICER_NAME_proj = "Pricer_Coloring_proj";

	MaxNodePricerProj* pricerRacineProj;
	static const char* PRICER_NAME_RACINE_proj = "pricer racine_proj";

	Cplex_Noproj_algo AA;
	Cplex_Noproj_algo AB;
	pricer_ptr = new ExactPricer(scip, PRICER_NAME, &G, &Master,0);
	pricer_ptr->activate_recovery = false;

	    if(atoi(argv[8]) ==1){ // activate recovery
	        pricer_ptr->activate_recovery = true;
	    
	    }
	

	
	pricer_ptr_proj = new ExactPricerProj(scip, PRICER_NAME_proj, &G, &Master,0);
		list<ObjPricerColoring*> lPrice;
	if(argv[6][0] == 'b'){
	
		SCIPincludeObjPricer(scip, pricer_ptr, true);
		
		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME));
		lPrice.push_back(pricer_ptr);

		pricerRacine= new MaxNodePricer(scip, PRICER_NAME_RACINE, &G, &Master,10,1);
		SCIPincludeObjPricer(scip, pricerRacine, true);
	}
	else if(argv[6][0] =='d'){
		SCIPincludeObjPricer(scip, pricer_ptr_proj, true);
				cout<<"juskla c bon"<<endl;
		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_proj));
		lPrice.push_back(pricer_ptr_proj);

		pricerRacineProj= new MaxNodePricerProj(scip, PRICER_NAME_RACINE_proj, &G, &Master,10,1);
		SCIPincludeObjPricer(scip, pricerRacineProj, true);

	}
	else if(argv[6][0] =='f'){
		
		
		
		pricer_ptr_proj->A_cplex = AA;
		pricer_ptr_proj->A_cplex.create_MIP(&G,-1);
	
		SCIPincludeObjPricer(scip, pricer_ptr_proj, true);

		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_proj));
		lPrice.push_back(pricer_ptr_proj);

		pricerRacineProj= new MaxNodePricerProj(scip, PRICER_NAME_RACINE_proj, &G, &Master,10,1);
		pricerRacineProj->A_cplex = AB;
		pricerRacineProj->A_cplex.create_MIP(&G,1);
		SCIPincludeObjPricer(scip, pricerRacineProj, true);
		
	
	
	}

	
	
	ExactTriangleSep* sep;
	ExactTriangleSep* sep2 =new ExactTriangleSep( scip,&Master,"sep triangle", G);
	//ChvatalGomorySep* sep2 = new ChvatalGomorySep( scip,&Master,"chvatal",G.nb_nodes, -1, 0);
	list<Separator*> lSep;

	
	
	double addTime=0;
	
	
	
	if(argv[2][1] == 'b' && argv[2][0] ==  'b'){
		
		NoCutting* noAlt= new NoCutting();
		NullSeparator noSep(scip,"fake pricer",&Master);
		pricerGlout= new GloutPricer(scip, PRICER_NAME_GLOUT, &G, &Master,100,noAlt,&noSep);
		SCIPincludeObjPricer(scip, pricerGlout, true);
		
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_GLOUT));
		
		
		
		
		
		
	  	
	


	}
	else if( argv[2][1] == '3' && argv[2][0] ==  'a'){
		if(argv[6][0] == 'b'){
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE));
			lPrice.push_back(pricerRacine);
			
		}
		else{
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE_proj));
			lPrice.push_back(pricerRacineProj);
		
		}
		K_Pricing* alt = new K_Pricing(atoi(argv[3]), &Master);
		//NullSeparator* sepp = new NullSeparator(scip,"ntmlebug");
		sep = new ExactTriangleSep( scip,&Master,"sep trianglea", G);
		sep->init();
		//NoCutting* alt = new NoCutting();
		sep2->init();
		sep->typeOfCoeffDoi= atoi(argv[4]);
		lSep.push_back(sep2);
		lSep.push_back(sep);

	        //SCIP_CALL(SCIPincludeObjSepa(scip,sep2,FALSE));

		pricerGlout= new GloutPricer(scip, PRICER_NAME_GLOUT, &G, &Master,100,alt,sep);
		SCIPincludeObjPricer(scip, pricerGlout, true);
		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_GLOUT));
		


	}
	else if( argv[2][1] == '2' && argv[2][0] ==  'a'){
		if(argv[6][0] == 'b'){
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE));
			lPrice.push_back(pricerRacine);
			
		}
		else{
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE_proj));
			lPrice.push_back(pricerRacineProj);
		
		
		}
		Alpha_Pricing* alt = new Alpha_Pricing(0.8375, G.nb_nodes /4 , G.nb_nodes /10 , &Master);
		
		sep = new ExactTriangleSep( scip,&Master,"sep triangle", G);
		sep->init();
		//NoCutting* alt = new NoCutting();
		sep2->init();
		sep->typeOfCoeffDoi= atoi(argv[4]);
		lSep.push_back(sep2);
		lSep.push_back(sep);

	        SCIP_CALL(SCIPincludeObjSepa(scip,sep2,FALSE));

		pricerGlout= new GloutPricer(scip, PRICER_NAME_GLOUT, &G, &Master,100,alt,sep);
		SCIPincludeObjPricer(scip, pricerGlout, true);
		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_GLOUT));

		

	}
	else if(argv[2][1] == 'c' && argv[2][0] ==  'b'){
			if(argv[6][0] == 'b'){
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE));
			lPrice.push_back(pricerRacine);
			}
			else{
			SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_RACINE_proj));
			lPrice.push_back(pricerRacineProj);
		
		}
		NoCutting* noAlt= new NoCutting();
		
		NullSeparator noSep(scip,"fake pricer",&Master);
		
		pricerGlout= new GloutPricer(scip, PRICER_NAME_GLOUT, &G, &Master,100,noAlt,&noSep);
	
		SCIPincludeObjPricer(scip, pricerGlout, true);
		
		SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME_GLOUT));
		sep2->init();
		pricerGlout->sep = sep2;
		lSep.push_back(sep2);
		(pricerGlout)->sepapa = true;
	        //SCIP_CALL(SCIPincludeObjSepa(scip,sep2,FALSE));
	        

	}
	
	if(argv[5][0] == '0'){
		SCIP_CALL( SCIPsetLongintParam(scip, "limits/totalnodes", 1) );

		SCIP_CALL( SCIPsetRealParam(scip, "limits/time", 3600) );
	}
	else if(argv[5][0] =='1'){
		
		SCIP_CALL( SCIPsetLongintParam(scip, "limits/totalnodes", 1) );

		SCIP_CALL( SCIPsetRealParam(scip, "limits/time", 3600) );	
	
	}
	
	
	else{
		//SCIP_CALL( SCIPsetRealParam(scip,"limits/absgap",1));
		
		lPrice.push_back(pricerGlout);
		
		

		SCIP_CALL( SCIPsetRealParam(scip, "limits/time", 10800) );
		//SCIP_CALL( SCIPsetObjIntegral(scip) );
		BranchingHandler* branchHandler = new BranchingHandler(scip, &G, lPrice, lSep , &Master);
   		BranchingRule* branchRule = new BranchingRule(scip,&G, &Master);

		SCIPincludeObjConshdlr(scip, branchHandler, TRUE);
		SCIPincludeObjBranchrule(scip, branchRule, TRUE);
		
		
//		 if(argv[5][0] =='2'){
//		 	ifstream res2;
//		 	
//			res2.open("../inter.txt",std::ios_base::in);
//			
//			string line;
//			getline(res2, line);
//			addTime = stod(line);
//			int kk = 0;
//			char c;
//			

//			while (getline(res2,line)){
//									 	
//				


//				StableSet* s = new StableSet;
//				s->G = &G;
//				s->initialize(G.nb_nodes);
//				
//				string number;
//				char c = ' ';
//				stringstream input_stringstream(line);
//				while(getline(input_stringstream,number,' ')){

//					int yoyo  =stoi(number); 
//					s->add(yoyo);
//					//cout<<"juskla"<<endl;
//					
//					//SCIPaddCoefLinear(scip, Master.V_node_ineq[yoyo], var->ptr, 1.0);
//						
//				}
//				if(isInVar(Master, s)){
//				
//					free(s);
//				
//				}
//				else{
//					char var_name[255];
//					SCIPsnprintf(var_name, 255, "V_%d",Master.L_var.size());
//					SCIPdebugMsg(scip, "new variable <%s>\n", var_name);
//					C_master_var* var=new C_master_var;

//					SCIPcreateVar(scip, &(var->ptr), var_name,
//								   0,                     // lower bound
//								   1e+20,      // upper bound
//								   1,                     // objective
//						   		   SCIP_VARTYPE_INTEGER, // variable type
//								   true, false, NULL, NULL, NULL, NULL, NULL);

//						  
//					SCIPaddVar(scip, var->ptr);
//					Master.L_var.push_back(var);			
//					var->stable = s;
//					kk++;
//					
//					for(list<int>::iterator hfg = s->L_nodes.begin() ; hfg != s->L_nodes.end(); hfg ++){
//						SCIPaddCoefLinear(scip, Master.V_node_ineq[*hfg], var->ptr, 1.0);

//					
//					
//					} 
//					
// 	
//				
//				}

//				
//				
//			}
			

			
			
			 
	
//		}
	}

	//sep2->typeOfCoeffDoi= atoi(argv[4]);
	auto start = chrono::steady_clock::now();
	SCIPwriteOrigProblem(scip, "test.lp", "lp", FALSE);
    
	SCIPsolve(scip);
	

	auto end = chrono::steady_clock::now();
	
	list<list<int>> solution;
	list<C_master_var*>::iterator itersol;   
	list<int> s; 
	SCIP_SOL* solutionnn = SCIPgetBestSol(scip);
	for(itersol =  Master.L_var.begin() ; itersol != Master.L_var.end(); itersol++){
		if(SCIPgetSolVal(scip, solutionnn, (*itersol	)->ptr)>= 1-0.00001){
			cout<<"un co-2-plex"<<endl;
			s.clear(); //value of variable in primal CIP solution, or in current LP/pseudo solution

			StableSet S  = *((*itersol)->stable) ; 
			for(list<int>::iterator uu = S.L_nodes.begin() ; uu != S.L_nodes.end() ; uu ++){
				s.push_back( *uu);
			
			}
			solution.push_back(s);
		}	
	
	}
	if( ! G.isItCovering( solution) && Master.DOI_var.size() == 0 && SCIPgetSolVal(scip, solutionnn,sol)==0 ){
	
		throw std::invalid_argument("does not cover");
			        exit(EXIT_FAILURE);
	
	}
//	else{	
//		cout<<"j'affichelesdoi"<<endl;
//		for(list<C_master_DOI*>::iterator dfg = Master.DOI_var.begin(); dfg != Master.DOI_var.end() ; dfg++){
//		
//			cout<<SCIPgetSolVal(scip, solutionnn, (*dfg	)->ptr) <<endl;
//		
//		}
//	
//	}
	
	ofstream res;
        string m1 = "../result";
        string m2 = argv[2];
        string m3 = "_";
        string m4 = argv[3];
        string m5 = argv[4];
        string m6=argv[5];
        string m7 = ".txt";
        string m8=argv[6];
        string m9 = argv[7];
        res.open(m1+ m2 + m3 + m4 + m3 +m5 + m3 +m6 +"_" + m8 + "_" + m9 +  m7,std::ios_base::app);
	
	if(argv[5][0] == '0' && argv[6][0] == 'b'){
	
	res<<nameext << ";" << SCIPgetLPObjval(scip) << ";" <<  SCIPgetNTotalNodes(scip) << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())   << ";" <<pricer_ptr->pricingCalled<<";" << pricerRacine->pricingCalled << ";" <<pricerGlout->pricingCalled << ";" << pricer_ptr->nbColumns << ";" << pricerRacine->nbColumns <<  ";" << pricerGlout->nbColumns<<";"<< pricer_ptr->pricingTime << ";" << pricerRacine->pricingTime << ";" << pricerGlout->pricingTime<<";" << pricerGlout->nbConstraints << ";"<<pricerGlout->separatingTime << ";" <<sep2->nbConstraints << ";" << sep2->sepaTime <<";"<< Master.DOI_var.size() << endl;}
	else if(argv[6][0] == 'c'){
	res<<nameext << ";" << SCIPgetLPObjval(scip) << ";" <<  SCIPgetNTotalNodes(scip) << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())   << ";" <<pricer_ptr_proj->pricingCalled<<";" << pricerRacineProj->pricingCalled << ";" <<pricerGlout->pricingCalled << ";" << pricer_ptr_proj->nbColumns << ";" << pricerRacineProj->nbColumns <<  ";" << pricerGlout->nbColumns<<";"<< pricer_ptr_proj->pricingTime << ";" << pricerRacineProj->pricingTime << ";" << pricerGlout->pricingTime<<";" << pricerGlout->nbConstraints << ";"<<pricerGlout->separatingTime << ";" <<sep2->nbConstraints << ";" << sep2->sepaTime <<";"<< Master.DOI_var.size() << endl;
	
	
	
	}
	else if(argv[5][0] =='1'){
		
		ofstream res1;
		res1.open("../inter.txt",std::ios_base::out);
		res1<<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())<<endl;
		list<C_master_var*>::iterator itVarClass;
		
		for(itVarClass= Master.L_var.begin() ; itVarClass != Master.L_var.end() ; itVarClass++ ){
			if(SCIPgetSolVal(scip, NULL, (*itVarClass)->ptr) != 0){
				for(list<int>::iterator itElem = (*itVarClass)->stable->L_nodes.begin();  itElem !=(*itVarClass)->stable->L_nodes.end() ; itElem++ ){
					res1<<*itElem<<" ";
				
				
				}
				res1<<endl;
			
			}
		
		}
		
		list<C_master_DOI*>::iterator itDOI;
		
		list<C_master_DOI*> again;
		list<list<int>> varvar;
		
		for(itDOI = Master.DOI_var.begin() ; itDOI != Master.DOI_var.end(); itDOI++){
			if(SCIPgetSolVal(scip, NULL, (*itDOI)->ptr) != 0){
				bool b = false;
				double sum = 0;
				for(itVarClass= Master.L_var.begin() ; itVarClass != Master.L_var.end() ; itVarClass++ ){
					if(SCIPgetSolVal(scip, NULL, (*itVarClass)->ptr) != 0 && (*itVarClass)->stable->contains((*itDOI)->v )){	
						sum=sum+SCIPgetSolVal(scip, NULL, (*itVarClass)->ptr);
						
						list<int> yo;
						for(list<int>::iterator koko = (*itDOI)->neighborhood.begin() ;koko != (*itDOI)->neighborhood.end() ; koko ++){
							res1<<*koko<<" ";
							yo.push_back(*koko);
						
						
						}  
						for(list<int>::iterator koko = (*itVarClass)->stable->L_nodes.begin() ;koko!= (*itVarClass)->stable->L_nodes.end() ; koko++) {
							if(*koko != (*itDOI)->v ){
								res1<<*koko<<" ";
								yo.push_back(*koko);
							
							}
						
						}
						res1<<endl;
						varvar.push_back(yo);
						if(sum >= SCIPgetSolVal(scip, NULL, (*itDOI)->ptr)){
							b = true;
							break;
							
						
						}
						
						
					}
				
				}
				if(!b){
					again.push_back(*itDOI);
				
				}
		
				
			}
		
		}
	
		res1.close();
		
		writeinter(again,varvar,scip);

	
	}
//	else if(argv[5][0] =='2'){
//		res<<nameext << ";" <<SCIPgetSolOrigObj(scip,NULL)<< ";"<< SCIPgetSolOrigObj(scip,solutionnn)<<";" <<  SCIPgetNTotalNodes(scip) << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())  + addTime << ";" <<pricer_ptr->pricingCalled<<";" << pricerRacine->pricingCalled << ";" <<pricerGlout->pricingCalled << ";" << pricer_ptr->nbColumns << ";" << pricerRacine->nbColumns <<  ";" << pricerGlout->nbColumns<<";"<< pricer_ptr->pricingTime << ";" << pricerRacine->pricingTime << ";" << pricerGlout->pricingTime<<";" << pricerGlout->nbConstraints << ";"<<pricerGlout->separatingTime << ";" <<sep2->nbConstraints << ";" << sep2->sepaTime  <<";"<< Master.DOI_var.size() << endl;
//	
//	}
	else{
		if(argv[6][0] == 'b'){
		res<<nameext << ";" << SCIPgetLPObjval(scip) << ";"<< SCIPgetPrimalbound(scip)<<";" <<  SCIPgetNTotalNodes(scip) << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())   << ";" <<pricer_ptr->pricingCalled<<";" << pricerRacine->pricingCalled << ";" <<pricerGlout->pricingCalled << ";" << pricer_ptr->nbColumns << ";" << pricerRacine->nbColumns <<  ";" << pricerGlout->nbColumns<<";"<< pricer_ptr->pricingTime << ";" << pricerRacine->pricingTime << ";" << pricerGlout->pricingTime<<";" << pricerGlout->nbConstraints << ";"<<pricerGlout->separatingTime << ";" <<sep2->nbConstraints << ";" << sep2->sepaTime  <<";"<< Master.DOI_var.size() << endl;}
		else{
			res<<nameext << ";" << SCIPgetLPObjval(scip) << ";"<< SCIPgetPrimalbound(scip)<<";" <<  SCIPgetNTotalNodes(scip) << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count())   << ";" <<pricer_ptr_proj->pricingCalled<<";" << pricerRacineProj->pricingCalled << ";" <<pricerGlout->pricingCalled << ";" << pricer_ptr_proj->nbColumns << ";" << pricerRacineProj->nbColumns <<  ";" << pricerGlout->nbColumns<<";"<< pricer_ptr_proj->pricingTime << ";" << pricerRacineProj->pricingTime << ";" << pricerGlout->pricingTime<<";" << pricerGlout->nbConstraints << ";"<<pricerGlout->separatingTime << ";" <<sep2->nbConstraints << ";" << sep2->sepaTime  <<";"<< Master.DOI_var.size() << endl;
		
		
		}
		
		
	
	
	//SCIPprintBestSol(scip, NULL, FALSE);
	}
	res.close();
	
	
	//res<< nameext << ";" << SCIPgetLPObjval(scip)  << ";" << Master.L_var.size()<< ";"  <<((double)chrono::duration_cast<chrono::milliseconds>(end - start).count()) /1000  << ";" << pricer_ptr->nbCalled << ";" << pricer_glout->nbCalled<< ";" <<p1->nbCalled <<";"<< p100->nbCalled<< ";" <<p10000->nbCalled  << ";" << pricer_ptr->nb_column_generated << ";" << pricer_glout->nb_column_generated << ";" <<  p1->nb_column_generated << ";" <<  p100->nb_column_generated << ";" <<  p10000->nb_column_generated << ";" << pricer_ptr->pricingTime/1000 << ";" <<  pricer_glout->pricingTime/1000 << ";" <<  p1->pricingTime/1000 << ";" <<  p100->pricingTime/1000 << ";" <<  p10000->pricingTime/1000 << isSolEntier(scip,&Master)<< "\n";


		
	return 0;
}


void C_master_coloring::printSolution(  SCIP * scip, SCIP_SOL *sol){
	list<C_master_var*>::iterator itv;
	list<int>::iterator iti;  
	//int nbcol = 0;

	cout<<"Affichage de la solution par nous"<<endl;
	for(itv = this->L_var.begin(); itv!=this->L_var.end(); itv++){
		(*itv)->printSolution( scip, sol);

	}

	cout<<"Affichage de la solution par SCIP"<<endl;
	SCIPprintBestSol(scip, NULL, FALSE);


}

void C_master_var::printSolution( SCIP * scip , SCIP_SOL *sol){


	if(SCIPgetSolVal (scip, sol, ptr) > SCIPepsilon(scip)){
		stable->print();

	}

	

}

bool isInVar(C_master_coloring M, StableSet* s){
	
	
	for(list<C_master_var*>::iterator it = M.L_var.begin(); it != M.L_var.end(); it++){
		bool b=true;
		StableSet* ss = (**it).stable;
		
		
		for(int i = 0; i < ss->nodes.size(); i++){
			b = b && ss->nodes[i] == s->nodes[i];
		
		
		}
		if(b)
			return true;
		
	
	
	
	}
	
	return false;


}

int getOpt(char * filename){
	
	ifstream fic("../optimums.txt");
	int ii;
	int result;
	char name[5000];
	
	while ( fic.good() ) {
		
	
		fic >> name;
		fic >> ii;
		if(strcmp(name, filename) == 0){
		
			result = ii;
		}
		
	}          
	
	return result;
	

	return 1e20;
}

void writeinter(list<C_master_DOI*> l1,list<list<int>> l2,SCIP* scip){
	cout<<l1.size()<<endl;
	if(l1.size() == 0){
		
		return ;
	}
	else{

		ofstream res1;
		res1.open("../inter.txt",std::ios_base::app);
		list<C_master_DOI*> again;
		again.clear();
		list<list<int>> ll;

		for(list<C_master_DOI*>::iterator itDOI = l1.begin() ;itDOI!= l1.end(); itDOI++ ){
			bool b = false;
			for(list<list<int>>::iterator itVarClass= l2.begin() ; itVarClass != l2.end() ; itVarClass++ ){
				list<int> yo;
				yo.clear();
				bool bb = false;
				for(list<int>::iterator it = (*itVarClass).begin(); it != (*itVarClass).end() ; it++){
				
					if(*it == (*itDOI)->v){
						bb = true;
						b=true;
						for(list<int>::iterator koko = (*itDOI)->neighborhood.begin() ;koko != (*itDOI)->neighborhood.end() ; koko ++){
							
							yo.push_back(*koko);
						
						
						
						}  
					
					
					
					}
					else{
						yo.push_back(*it);
					}
				
				
				}
				if(bb){
					for(list<int>::iterator it =  yo.begin() ; it != yo.end(); it++){
						res1<<*it<<" ";
					
					
					}
					ll.push_back(yo);
					res1<<endl;
					break;
				
				
				}
				
		
			}
			if(!b){
			
				again.push_back(*itDOI);
			
			
			}
			
			
		
		
		}
		
		
		res1.close();
		
		if(again.size() != 0){
		
		writeinter(again, ll, scip);
		}
		
		
	}



}


