#  Column generation for the 2-defective coloring


This project implements all algorithms we have tried in order to
developp the ones described in the companion paper.

## What is a 2-defective coloring

The coloring problem consisting of assigning colors to the vertices
of a graph such that two adjacent vertices have different colors is
a well studied NP-hard problem. It can be seen as covering the graph
with stable sets (sets of non adjacent vertices), where each stable 
set correspond to a color class. A variant of this problem is the 
k-defective coloring where we cover the graph we co-k-plexes that
are sets of vertices inducing graphs of maximum degree 1.


## Description of the project

The algorithms are different variants of branch and cut and price.
We use SCIP optimization to handle the column generation framework
but CPLEX is used to solve the intermediate LP resolutions. 

The exact pricing subproblem is solved using a compact ILP solved
with CPLEX. Two heuristics are used to fasten the pricing subproblem
resolutions. 


## Short description of the files

### main.cpp

Contains the setups of CPLEX and SCIP according to the arguments
launch the algorithms and retrieve the solutions statistics into
a file.

### SCIP_master.h

This file contains a class that will be long to each elements such
as separator, pricer, to keep track of the current variables and 
contraints of the ILP.

### Alternance.h

We want to have full control on when the separators are called
so that we can separate before no negative reduced cost column
exists. We tried to do so because in column generation there
often are problems with pricing convergence which we try to above
by cutting the primal incumbent solution earlier. In SCIP, it is
not possible to do so by default. Alternance will then be an
attribute of a pricing algorithm deciding whether to cut or price.
It must be decided inside a Pricing algorithm because it will
be the objects called by SCIP before achieving primal convergence.

### Pricer_coloring.h

This file describes an abstract pricer class which will be implemented
by all our pricing algorithms (exact and heuristics). 

The three algorithms that implement this class are ExactPricer, 
GloutPricer (a greedy algorithm ordering the vertices and adding
them iteratively) and MaxNodePricer (an ILP with a limit on the 
number of branching nodes).

### Separator

This file contains the abstract class for separation algorithm.
We have tried to add chvatal gomory cuts but it was not succesfull,
seoparated with a MILP, written in the file ChvatalGomorySep. 
The separation that worked the best was the exact separation of
triangle inequalities ExactTriangleSep.

### BranchRule/Handler

These are the files permitting to handle a dedicated branching.
### Technique

It contains some unused rounding functions that we used to make
some experimental tries, it seems like they are not needed.



## How to launch the project

exec <name of the DIMACS instance> 
