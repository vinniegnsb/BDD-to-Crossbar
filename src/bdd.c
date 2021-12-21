#include <stdio.h>
#include <stdlib.h>
#include "bdd.h"
#include "xbar.h"
#include "general_functions.h"

BDD *read_bdd_from_file(char *path)
{
  int i;
  FILE *fp = NULL;     // file pointer
  fp = fopen(path, "r");
  // catch invalid filenames
  if (fp == NULL)
  {
    printf("file does not exist\n");
    return NULL;
  }

  /*
      file format:
      vars {numvars}
      nodes {numnodes}
      {nodeID} {leftChildNode} {rightChildNode} {decisionVarID}
      ...
  */

  BDD *myBDD = malloc(sizeof(BDD));

  char *word = NULL;
  int val1, val2, val3, val4;
  word = malloc(sizeof(char) * 20);

  fscanf(fp, "%s %d", word, &myBDD->vars);
  fscanf(fp, "%s %d", word, &myBDD->nodes);

  free(word);

  myBDD->nodeArray = malloc(sizeof(BDDNode) * myBDD->nodes);

  for (i = 0; i < myBDD->nodes; i++)
  {
    fscanf(fp, "%d %d %d %d", &val1, &val2, &val3, &val4);
    myBDD->nodeArray[i].nodeID = val1;
    myBDD->nodeArray[i].leftChildNode = val2;
    myBDD->nodeArray[i].rightChildNode = val3;
    myBDD->nodeArray[i].decisionVarID = val4;
  }

  fclose(fp);

  return myBDD;
}

void free_bdd(BDD *myBDD)
{
  free(myBDD->nodeArray);
  free(myBDD);
}

void print_bdd(BDD myBDD)
{
  int i;

  printf("%s %d\n%s %d\n", "vars", myBDD.vars, "nodes", myBDD.nodes);
  for (i = 0; i < myBDD.nodes; i++)
  {
    printf("%d %d %d %d\n", myBDD.nodeArray[i].nodeID, myBDD.nodeArray[i].leftChildNode,
                  myBDD.nodeArray[i].rightChildNode, myBDD.nodeArray[i].decisionVarID);
  }
  return;
}

int evaluate_bdd(int *varInput, BDD myBDD)
{
  int i = 0;

  // evaluate the bdd with the variable instance

  while (1)
  {
    // just checking left child since if a child is negative, then both are
    if (myBDD.nodeArray[i].leftChildNode == -1)
      return myBDD.nodeArray[i].decisionVarID;
    else if (varInput[myBDD.nodeArray[i].decisionVarID - 1] == 1)
      i = myBDD.nodeArray[i].leftChildNode - 1;
    else
      i = myBDD.nodeArray[i].rightChildNode - 1;
  }
}

void print_bdd_truth_table(BDD myBDD)
{
  int j, val, rowLim, *varInstance;
  unsigned int i;
  char temp[5];
  varInstance = (int *)malloc(sizeof(int) * myBDD.vars);
  rowLim = powerOfTwo(myBDD.vars);

  // print out x's and f
  for (i = 0; i < myBDD.vars; i++)
  {
    sprintf(temp, "x%d", myBDD.vars - i);
    printf(" %3s", temp);
  }
  printf("  f\n");

  // populate truth table and compute
  for (i = 0; i < rowLim; i++)
  {
    for (j = 0; j < myBDD.vars; j++)
    {
      // if odd 0, if even 1
      val = (i / powerOfTwo(myBDD.vars - j - 1)) % 2;
      printf("  %d ", val);
      varInstance[myBDD.vars - j - 1] = val;
    }
    printf("  %d\n", evaluate_bdd(varInstance, myBDD));
  }
  free(varInstance);
}

void equivalence_check(char *bddFile, char *xbarFile)
{
  BDD *myBDD;
  Xbar *myXbar;

  myBDD = read_bdd_from_file(bddFile);
  myXbar = read_xbar_from_file(xbarFile);

  // return if no file comes back
  if ((myBDD == NULL) || (myXbar == NULL))
  return;

  // check if var count is the same
  if (myBDD->vars != myXbar->vars)
  {
    printf("The representations are not equivalent!\n");
    free_bdd(myBDD);
    free_xbar(myXbar);
    return;
  }

  int j, val, rowLim, *varInstance, bddEval, xbarEval;
  unsigned int i;
  rowLim = powerOfTwo(myBDD->vars);
  varInstance = (int *)malloc(sizeof(int) * myBDD->vars);

  // populate truth table and compute
  for (i = 0; i < rowLim; i++)
  {
    for (j = 0; j < myBDD->vars; j++)
    {
      // if odd 0, if even 1
      val = (i / powerOfTwo(myBDD->vars - j - 1)) % 2;
      varInstance[myBDD->vars - j - 1] = val;
    }
    bddEval = evaluate_bdd(varInstance, *myBDD);
    xbarEval = evaluate_xbar(varInstance, *myXbar);

    if (bddEval != xbarEval)
    {
      printf("The representations are not equivalent!\n");
      free_bdd(myBDD);
      free_xbar(myXbar);
      free(varInstance);
      return;
    }
  }
  // truth table matches, therefore equivalent
  printf("The representations are equivalent!\n");
  free_bdd(myBDD);
  free_xbar(myXbar);
  free(varInstance);
  return;
}

void shift_and_edge_replace(BDDNode *node, int zeroID)
{
  if (node->nodeID > zeroID)
    node->nodeID--;
  if (node->leftChildNode >= zeroID)
  {
    if (node->leftChildNode > zeroID)
      node->leftChildNode--;
    else
      node->leftChildNode = 0;
  }
  if (node->rightChildNode >= zeroID)
  {
    if (node->rightChildNode > zeroID)
      node->rightChildNode--;
    else
      node->rightChildNode = 0;
  }
  return;
}

void map_bdd_to_xbar(char *bddFile, char *xbarFile)
{
  BDD *myBDD;
  Xbar *myXbar;

  int i, j, ztfIndexer, edgeIndex, wordlineCount, bitlineCount;
  int oneNodeID, zeroNodeID;
  BDDNode *zeroTerminalFree;
  Edge *validEdges;

  myBDD = read_bdd_from_file(bddFile);
  myXbar = malloc(sizeof(Xbar));

  // return if no file comes back
  if (myBDD == NULL)
    return;

  wordlineCount = myBDD->nodes - 1; // all nodes except for terminal 0

  zeroTerminalFree = (BDDNode *)malloc(sizeof(BDDNode) * wordlineCount);
  ztfIndexer = 0;

  // find zero terminal node
  for (i = 0; i < myBDD->nodes; i++)
    if (myBDD->nodeArray[i].leftChildNode == -1)
      if (myBDD->nodeArray[i].decisionVarID == 0)
        zeroNodeID = myBDD->nodeArray[i].nodeID;

  bitlineCount = 0;
  edgeIndex = 0;

  // iterate through nodes and trim terminal node 0 and all connected edges
  for (i = 0; i < myBDD->nodes; i++)
  {
    // just checking left child since if a child is negative, then both are
    if (myBDD->nodeArray[i].leftChildNode == -1)
    {
      // possibly a terminal node is not one of the last nodes in bdd file
      if (myBDD->nodeArray[i].decisionVarID == 1)
      {
        // oneNodeID = myBDD->nodeArray[i].nodeID;
        zeroTerminalFree[ztfIndexer] = myBDD->nodeArray[i];
        shift_and_edge_replace(&zeroTerminalFree[ztfIndexer], zeroNodeID);
        ztfIndexer++;
      }
    }
    else
    {
      zeroTerminalFree[ztfIndexer] = myBDD->nodeArray[i];
      shift_and_edge_replace(&zeroTerminalFree[ztfIndexer], zeroNodeID);
      // update amount of edges not pointing to terminal node 0
      if (zeroTerminalFree[ztfIndexer].leftChildNode != 0)
        bitlineCount++;
      if (zeroTerminalFree[ztfIndexer].rightChildNode != 0)
        bitlineCount++;
      ztfIndexer++;
    }
  }

  validEdges = (Edge *)malloc(sizeof(Edge) * bitlineCount);
  edgeIndex = 0;

  // check for valid edges and add them to edge array
  for (i = 0; i < wordlineCount; i++)
  {
    if (zeroTerminalFree[i].leftChildNode > 0)
    {
      validEdges[edgeIndex].parentID = zeroTerminalFree[i].nodeID;
      validEdges[edgeIndex].childID = zeroTerminalFree[i].leftChildNode;
      // varID true
      validEdges[edgeIndex].varID = zeroTerminalFree[i].decisionVarID;
      edgeIndex++;
    }
    if (zeroTerminalFree[i].rightChildNode > 0)
    {
      validEdges[edgeIndex].parentID = zeroTerminalFree[i].nodeID;
      validEdges[edgeIndex].childID = zeroTerminalFree[i].rightChildNode;
      // varID false
      validEdges[edgeIndex].varID = -(zeroTerminalFree[i].decisionVarID);
      edgeIndex++;
    }
  }

  myXbar->vars = myBDD->vars;
  myXbar->rows = wordlineCount;
  myXbar->cols = bitlineCount;
  myXbar->xbar = (int **)malloc(sizeof(int *) * myXbar->rows);

  for (i = 0; i < myXbar->rows; i++)
    myXbar->xbar[i] = (int *)malloc(sizeof(int) * myXbar->cols);

  for (i = 0; i < myXbar->cols; i++)
  {
    for (j = 0; j < myXbar->rows; j++)
    {
      if (validEdges[i].parentID == (j + 1) || validEdges[i].childID == (j + 1))
        myXbar->xbar[j][i] = validEdges[i].varID;
      else
        myXbar->xbar[j][i] = 0;
    }
  }

  save_xbar_to_file(xbarFile, *myXbar);
  free_bdd(myBDD);
  free_xbar(myXbar);
  free(zeroTerminalFree);
  free(validEdges);
}

void map_bdd_to_xbar_compact(char *bddFile, char *xbarFile)
{
  BDD *myBDD;
  Xbar *myXbar;

  int i, j, l, r, ztfIndexer, edgeIndex, wordlineCount, bitlineCount;
  int oneNodeID, zeroNodeID, bCount, rCount, bIndex, rIndex;
  BDDNode *zeroTerminalFree;
  Edge *validEdges;

  int currSize = 0;
  int *colorIndex;
  char *colorArray;
  int parentVar;
  int childVar;

  int *colorOrderArray;

  myBDD = read_bdd_from_file(bddFile);
  myXbar = malloc(sizeof(Xbar));

  // return if no file comes back
  if (myBDD == NULL)
    return;

  wordlineCount = myBDD->nodes - 1; // all nodes except for terminal 0

  zeroTerminalFree = (BDDNode *)malloc(sizeof(BDDNode) * wordlineCount);
  ztfIndexer = 0;

  currSize = wordlineCount;
  colorArray = (char *)malloc(sizeof(char) * currSize);

  // find zero terminal node
  for (i = 0; i < myBDD->nodes; i++)
    if (myBDD->nodeArray[i].leftChildNode == -1)
      if (myBDD->nodeArray[i].decisionVarID == 0)
        zeroNodeID = myBDD->nodeArray[i].nodeID;

  bitlineCount = 0;
  edgeIndex = 0;

  // iterate through nodes and trim terminal node 0 and all connected edges
  for (i = 0; i < myBDD->nodes; i++)
  {
    // just checking left child since if a child is negative, then both are
    if (myBDD->nodeArray[i].leftChildNode == -1)
    {
      // possibly a terminal node is not one of the last nodes in bdd file
      if (myBDD->nodeArray[i].decisionVarID == 1)
      {
        // oneNodeID = myBDD->nodeArray[i].nodeID;
        zeroTerminalFree[ztfIndexer] = myBDD->nodeArray[i];
        shift_and_edge_replace(&zeroTerminalFree[ztfIndexer], zeroNodeID);
        oneNodeID = zeroTerminalFree[ztfIndexer].nodeID;
        ztfIndexer++;
      }
    }
    else
    {
      zeroTerminalFree[ztfIndexer] = myBDD->nodeArray[i];
      shift_and_edge_replace(&zeroTerminalFree[ztfIndexer], zeroNodeID);
      // update amount of edges not pointing to terminal node 0
      if (zeroTerminalFree[ztfIndexer].leftChildNode != 0)
        bitlineCount++;
      if (zeroTerminalFree[ztfIndexer].rightChildNode != 0)
        bitlineCount++;
      ztfIndexer++;
    }
  }

  for (i = 0; i < currSize; i++)
  {
    if (i == oneNodeID - 1)
      colorArray[i] = 'b'; // preemptively color terminal one blue
    else
      colorArray[i] = 'n';
  }

  colorArray[0] = 'b';
  // two-color BDD array and add nodes to resolve conflicts
  for (i = 0; i < wordlineCount; i++)
  {
    if (colorArray[i] == 'b')
    {
      l = zeroTerminalFree[i].leftChildNode;
      if (l > 0)
      {
        if (colorArray[l - 1] == 'n')
        {
          colorArray[l - 1] = 'r';
        }
        else if (colorArray[l - 1] == 'b')
        {
          // conflict detected, going to have to add node
          currSize++;
          zeroTerminalFree =
            (BDDNode *)realloc(zeroTerminalFree, sizeof(BDDNode) * currSize);
          // this doesn't take care of other allocs
          if (zeroTerminalFree == NULL)
          {
            printf("Failed to increase zeroTerminalFree size\n");
            return;
          }
          colorArray = (char *)realloc(colorArray, sizeof(char) * currSize);
          if (colorArray == NULL)
          {
            printf("Failed to increase colorArray size\n");
            return;
          }
          // copy IDs over from current node to newly added node
          zeroTerminalFree[currSize - 1].nodeID = currSize;
          zeroTerminalFree[currSize - 1].leftChildNode =
            zeroTerminalFree[i].leftChildNode;
          zeroTerminalFree[currSize - 1].rightChildNode = 0;
            // zeroTerminalFree[i].rightChildNode;
          zeroTerminalFree[currSize - 1].decisionVarID =
            zeroTerminalFree[i].decisionVarID;

          zeroTerminalFree[i].leftChildNode =
            zeroTerminalFree[currSize - 1].nodeID;

          colorArray[currSize - 1] = 'r';
        }
      }

      r = zeroTerminalFree[i].rightChildNode;
      if (r > 0)
      {
        if (colorArray[r - 1] == 'n')
        {
          colorArray[r - 1] = 'r';
        }
        else if (colorArray[r - 1] == 'b')
        {
          // conflict detected, going to have to add node
          currSize++;
          zeroTerminalFree =
            (BDDNode *)realloc(zeroTerminalFree, sizeof(BDDNode) * currSize);
          // this doesn't take care of other allocs
          if (zeroTerminalFree == NULL)
          {
            printf("Failed to increase zeroTerminalFree size\n");
            return;
          }
          colorArray = (char *)realloc(colorArray, sizeof(char) * currSize);
          if (colorArray == NULL)
          {
            printf("Failed to increase colorArray size\n");
            return;
          }
          // copy IDs over from current node to newly added node
          zeroTerminalFree[currSize - 1].nodeID = currSize;
          zeroTerminalFree[currSize - 1].leftChildNode = 0;
            // zeroTerminalFree[i].leftChildNode;
          zeroTerminalFree[currSize - 1].rightChildNode =
            zeroTerminalFree[i].rightChildNode;
          zeroTerminalFree[currSize - 1].decisionVarID =
            zeroTerminalFree[i].decisionVarID;

          zeroTerminalFree[i].rightChildNode =
            zeroTerminalFree[currSize - 1].nodeID;

          colorArray[currSize - 1] = 'r';
        }
      }
    }
    else if (colorArray[i] == 'r')
    {
      l = zeroTerminalFree[i].leftChildNode;
      if (l > 0) // check child is not trimmed zero terminal first
      {
        if (colorArray[l - 1] == 'n')
        {
          colorArray[l - 1] = 'b';
        }
        else if (colorArray[l - 1] == 'r')
        {
          // conflict detected, going to have to add node
          currSize++;
          zeroTerminalFree =
            (BDDNode *)realloc(zeroTerminalFree, sizeof(BDDNode) * currSize);
          // this doesn't take care of other allocs
          if (zeroTerminalFree == NULL)
          {
            printf("Failed to increase zeroTerminalFree size\n");
            return;
          }
          colorArray = (char *)realloc(colorArray, sizeof(char) * currSize);
          if (colorArray == NULL)
          {
            printf("Failed to increase colorArray size\n");
            return;
          }
          // copy IDs over from current node to newly added node
          zeroTerminalFree[currSize - 1].nodeID = currSize;
          zeroTerminalFree[currSize - 1].leftChildNode =
            zeroTerminalFree[i].leftChildNode;
          zeroTerminalFree[currSize - 1].rightChildNode = 0;
            // zeroTerminalFree[i].rightChildNode;
          zeroTerminalFree[currSize - 1].decisionVarID =
            zeroTerminalFree[i].decisionVarID;

          zeroTerminalFree[i].leftChildNode =
            zeroTerminalFree[currSize - 1].nodeID;

          colorArray[currSize - 1] = 'b';
        }
      }

      r = zeroTerminalFree[i].rightChildNode;
      if (r > 0) // check child is not trimmed zero terminal first
      {
        if (colorArray[r - 1] == 'n')
        {
          colorArray[r - 1] = 'b';
        }
        else if (colorArray[r - 1] == 'r')
        {
          // conflict detected, going to have to add node
          currSize++;
          zeroTerminalFree =
            (BDDNode *)realloc(zeroTerminalFree, sizeof(BDDNode) * currSize);
          // this doesn't take care of other allocs
          if (zeroTerminalFree == NULL)
          {
            printf("Failed to increase zeroTerminalFree size\n");
            return;
          }
          colorArray = (char *)realloc(colorArray, sizeof(char) * currSize);
          if (colorArray == NULL)
          {
            printf("Failed to increase colorArray size\n");
            return;
          }
          // copy IDs over from current node to newly added node
          zeroTerminalFree[currSize - 1].nodeID = currSize;
          zeroTerminalFree[currSize - 1].leftChildNode = 0;
            // zeroTerminalFree[i].leftChildNode;
          zeroTerminalFree[currSize - 1].rightChildNode =
            zeroTerminalFree[i].rightChildNode;
          zeroTerminalFree[currSize - 1].decisionVarID =
            zeroTerminalFree[i].decisionVarID;

          zeroTerminalFree[i].rightChildNode =
            zeroTerminalFree[currSize - 1].nodeID;

          colorArray[currSize - 1] = 'b';
        }
      }
    }
  }

  colorIndex = (int *)malloc(sizeof(int) * currSize);
  colorOrderArray = (int *)malloc(sizeof(int) * currSize);

  // count blue and red nodes
  bCount = 0;
  rCount = 0;

  // set all nodes as unchecked (-1)
  for (i = 0; i < currSize; i++)
    colorIndex[i] = UNCHECKED;

  // iterate through each node in logical order
  for (i = 0; i < currSize; i++)
  {
    // check we are not at the terminal node
    if (i == oneNodeID - 1)
      continue;
    // check if colorIndex has already been assigned at current node
    else if (i > oneNodeID - 1 && colorIndex[i] != UNCHECKED)
      continue;
    else
    {
      if (colorArray[i] == 'b')
      {
        if (colorIndex[i] == UNCHECKED)
        {
          // set current node's blue index
          colorIndex[i] = bCount;
          bCount++;
        }

        // check left node, then right node (children)
        parentVar = zeroTerminalFree[i].decisionVarID;
        if (zeroTerminalFree[i].leftChildNode > 0 && colorIndex[zeroTerminalFree[i].leftChildNode - 1] == UNCHECKED)
        {
          childVar =
           zeroTerminalFree[zeroTerminalFree[i].leftChildNode - 1].decisionVarID;
          // don't assign if var dist greater than 1
          if (childVar - parentVar >= 0)
          {

            // set left node's red index
            colorIndex[zeroTerminalFree[i].leftChildNode - 1] = rCount;
            rCount++;
          }
        }
        if (zeroTerminalFree[i].rightChildNode > 0 && colorIndex[zeroTerminalFree[i].rightChildNode - 1] == UNCHECKED)
        {
          childVar =
           zeroTerminalFree[zeroTerminalFree[i].rightChildNode - 1].decisionVarID;
          // don't assign if var dist greater than 1
          if (childVar - parentVar >= 0)
          {
            // set right node's red index
            colorIndex[zeroTerminalFree[i].rightChildNode - 1] = rCount;
            rCount++;
          }
        }
      }
      else if (colorArray[i] == 'r')
      {
        if (colorIndex[i] == UNCHECKED)
        {
          // set current node's red index
          colorIndex[i] = rCount;
          rCount++;
        }

        // check left node, then right node (children)
        parentVar = zeroTerminalFree[i].decisionVarID;
        if (zeroTerminalFree[i].leftChildNode > 0 && colorIndex[zeroTerminalFree[i].leftChildNode - 1] == UNCHECKED)
        {
          childVar =
           zeroTerminalFree[zeroTerminalFree[i].leftChildNode - 1].decisionVarID;
          // don't assign if var dist greater than 1
          if (childVar - parentVar >= 0)
          {

            // set left node's blue index
            colorIndex[zeroTerminalFree[i].leftChildNode - 1] = bCount;
            bCount++;
          }
        }
        if (zeroTerminalFree[i].rightChildNode > 0 && colorIndex[zeroTerminalFree[i].rightChildNode - 1] == UNCHECKED)
        {
          childVar =
           zeroTerminalFree[zeroTerminalFree[i].rightChildNode - 1].decisionVarID;
          // don't assign if var dist greater than 1
          if (childVar - parentVar >= 0)
          {
              // set right node's blue index
              colorIndex[zeroTerminalFree[i].rightChildNode - 1] = bCount;
              bCount++;
          }
        }
      }
    }
  }
  // terminal node is the last blue node
  colorIndex[oneNodeID - 1] = bCount;
  bCount++;

  myXbar->vars = myBDD->vars;
  myXbar->rows = bCount;
  myXbar->cols = rCount;
  myXbar->xbar = (int **)malloc(sizeof(int *) * myXbar->rows);

  // initialize array;
  for (i = 0; i < myXbar->rows; i++)
  {
    myXbar->xbar[i] = (int *)malloc(sizeof(int) * myXbar->cols);
    for (j = 0; j < myXbar->cols; j++)
      myXbar->xbar[i][j] = 0;
  }

  bIndex = 0;
  rIndex = 0;
  // populate xbar with blue as rows and red as columns
  for (i = 0; i < currSize; i++)
  {
    if (colorArray[i] == 'b')
    {
      if (zeroTerminalFree[i].leftChildNode > 0)
      {
        bIndex = colorIndex[i];
        rIndex = colorIndex[zeroTerminalFree[i].leftChildNode - 1];
        myXbar->xbar[bIndex][rIndex] = zeroTerminalFree[i].decisionVarID;
      }
      if (zeroTerminalFree[i].rightChildNode > 0)
      {
        bIndex = colorIndex[i];
        rIndex = colorIndex[zeroTerminalFree[i].rightChildNode - 1];
        myXbar->xbar[bIndex][rIndex] = -(zeroTerminalFree[i].decisionVarID);
      }
    }
    else if (colorArray[i] == 'r')
    {
      if (zeroTerminalFree[i].leftChildNode > 0)
      {
        bIndex = colorIndex[zeroTerminalFree[i].leftChildNode - 1];
        rIndex = colorIndex[i];
        myXbar->xbar[bIndex][rIndex] = zeroTerminalFree[i].decisionVarID;
      }
      if (zeroTerminalFree[i].rightChildNode > 0)
      {
        bIndex = colorIndex[zeroTerminalFree[i].rightChildNode - 1];
        rIndex = colorIndex[i];
        myXbar->xbar[bIndex][rIndex] = -(zeroTerminalFree[i].decisionVarID);
      }
    }
  }

  save_xbar_to_file(xbarFile, *myXbar);
  free_bdd(myBDD);
  free_xbar(myXbar);
  free(zeroTerminalFree);
  free(colorIndex);
  free(colorArray);
}
