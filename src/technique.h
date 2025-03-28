#ifndef __tech
#define __tech
using namespace std;


#include "scip/scip.h"
#include <list>
#include <algorithm>
#include "SCIP_master.h"

class C_master_coloring;


SCIP_Real arrondiS(SCIP_Real b);
double arrondis(double b);
int arrondiSup(SCIP_Real b);
double abs(double r1, double r2);
bool contains(list<int> &l, int x);
bool isSolEntier(SCIP* scip, C_master_coloring* M);
float maxx(float a, float b);
SCIP_Real sumValue(SCIP* scip, C_master_coloring* M);
SCIP_Real rounding(SCIP* scip, SCIP_Real r);



#endif
