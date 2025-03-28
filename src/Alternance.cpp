#include "Alternance.h"

#define epsilonAlternance 1e-4


///////////// Alpha_Pricing

bool Alpha_Pricing::ShouldWePrice(SCIP* scip){

	SCIP_Real ss = sumValue(scip,M);
 	
	

	if(lastobj == 0){

		nbPricingRound++;
		lastobj = ss;
		lastRound = true;
		return true;
	}
	else if((ss-lastobj)/lastobj >epsilonAlternance){
		k  =maxx(k*alpha,1);

	}
	else if(!lastRound){
		k  =maxx(k/alpha,1);

	}


	lastobj = ss;

	if(M->L_var.size() < setup){

		
		nbPricingRound++;
		lastRound =  true;
	}
	else if(nbPricingRound == setup ){

		nbPricingRound=0;
		lastRound =  false;
	}
	else if(nbPricingRound < k){

		nbPricingRound++;
		lastRound =  true;
	}
	else{
	
		nbPricingRound=0;
		lastRound =  false;
	}
	

	return lastRound;

	
	

}

void Alpha_Pricing::hadToPrice(){
	
	nbPricingRound++;
}

//////////// K_Pricing




bool K_Pricing::ShouldWePrice(SCIP* scip){
	//cout<<"price or cut question ?"<<endl;
	SCIP_Real ss = sumValue(scip,M);
  	bool shouldprice=true;
	
	//cout<<lastObj.size()<<endl;
	if(lastObj.size()<k){
		
		lastObj.push(ss);
	}
	else{

		SCIP_Real lV = lastObj.front() ;
		shouldprice = !(maxx(lV-ss,ss-lV) <= epsilonAlternance);
		lastObj.pop();
		lastObj.push(ss);
		//cout<<lV<<" "<<ss<< endl;
		//cout<<shouldprice<<endl;
		if(!shouldprice){

			//we are going to separate so we should empty the queue in order to not cut again next k round
			for(int t = 0; t < lastObj.size(); t++){
				lastObj.pop();
			}

		}
		

  	}
	
	//cout<<"answer"<<endl;
	return shouldprice;

}
