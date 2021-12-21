#ifndef __BDD_H
#define __BDD_H

#define UNCHECKED -1

typedef struct BDDNode {
  int nodeID;
  int leftChildNode;
  int rightChildNode;
  int decisionVarID;
} BDDNode;

typedef struct BDD {
  int vars;
  int nodes;
  BDDNode *nodeArray;
} BDD;

typedef struct Edge {
  int parentID;
  int childID;
  int varID;
} Edge;

// Function prototypes
BDD *read_bdd_from_file(char *path);
void free_bdd(BDD *myBDD);
void print_bdd(BDD myBDD);
int evaluate_bdd(int *varInput, BDD myBDD);
void print_bdd_truth_table(BDD myBDD);
void equivalence_check(char *bddFile, char *xbarFile);
void shift_and_edge_replace(BDDNode *node, int zeroID);
void map_bdd_to_xbar(char *bddFile, char *xbarFile);
void map_bdd_to_xbar_compact(char *bddFile, char *xbarFile);

#endif
