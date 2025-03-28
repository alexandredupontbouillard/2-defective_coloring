#ifndef __ALTERNANCE
#define __ALTERNANCE



#include "SCIP_master.h"
#include <queue>
#include "technique.h"
#include "objscip/objscip.h"
#include "scip/scip.h"


using namespace std;
using namespace scip;

class C_master_coloring;
class Alternance{ //abstract class with a method to decide pricing or cutting

	public : 
	C_master_coloring* M;
	virtual bool ShouldWePrice(SCIP* scip) = 0;
	virtual void hadToPrice()=0;
	virtual ~Alternance(){}
};






class NoCutting : public Alternance{


	public : 


	NoCutting(){}
	virtual bool ShouldWePrice(SCIP* scip){
		return true;
	}
	virtual void hadToPrice(){return ;}
	virtual ~NoCutting(){}



};

class Alpha_Pricing : public Alternance{

	public :
	float alpha;
	int setup;
	int k;
	int nbPricingRound;
	SCIP_Real lastobj;
	bool lastRound;

	Alpha_Pricing(float alpha_, int setup_, int k_, C_master_coloring* M_){
		M=M_;
		alpha=alpha_;
		setup=setup_;
		k = k_;
		nbPricingRound = 0;
		lastobj=0;	
	}
	virtual void hadToPrice();
	virtual bool ShouldWePrice(SCIP* scip);
	virtual ~Alpha_Pricing(){}

};

class K_Pricing : public Alternance{
	public :

	queue<SCIP_Real> lastObj;
	int k;
	
	K_Pricing(int k_, C_master_coloring* M_){
		k=k_;
		M=M_;
	}
	bool ShouldWePrice(SCIP* scip); //called to ask if we should start a pricing round
	void hadToPrice(){}  //called when the separator failed finding a constraint
	virtual ~K_Pricing(){}
};





















#endif
