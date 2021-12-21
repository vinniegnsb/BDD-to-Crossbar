#include <stdio.h>
#include <stdlib.h>
#include "xbar.h"
#include "general_functions.h"

Xbar *read_xbar_from_file(char *path)
{
  int i, j;

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
      rows {numrows}
      cols {numcols}
      {on/off state} {leftChildNode} {rightChildNode} {decisionVarID}
      ...
  */

  Xbar *myXbar = malloc(sizeof(Xbar));

  char *word = NULL;
  int val1, val2, val3, val4;
  word = malloc(sizeof(char) * 20);

  fscanf(fp, "%s %d", word, &myXbar->vars);
  fscanf(fp, "%s %d", word, &myXbar->rows);
  fscanf(fp, "%s %d", word, &myXbar->cols);

  free(word);

  myXbar->xbar = (int **)malloc(sizeof(int *) * myXbar->rows);

  for (i = 0; i < myXbar->rows; i++)
  {
    myXbar->xbar[i] = (int *)malloc(sizeof(int) * myXbar->cols);
    for (j = 0; j < myXbar->cols; j++)
      fscanf(fp, "%d ", &myXbar->xbar[i][j]);
  }

  fclose(fp);

  return myXbar;
}

void free_xbar(Xbar *myXbar)
{
  int i;
  for (i = 0; i < myXbar->rows; i++)
  free(myXbar->xbar[i]);
  free(myXbar->xbar);
  free(myXbar);
}

void print_xbar(Xbar myXbar)
{
  int i, j, out;
  char temp[5];

  // for (i = 0; i < myXbar.rows; i++)
  for (i = 0; i < myXbar.rows; i++)
  {
    // sprintf(temp, "R%d", i);
    // printf(" %3s", temp);
    // for (j = 0; j < myXbar.cols; j++)
    for (j = 0; j < myXbar.cols; j++)
    {
      if (myXbar.xbar[i][j] == 99)
        printf("%3d ", 1);
      else if (myXbar.xbar[i][j] == 0)
        printf("%3d ", 0);
      else
      {

        if (myXbar.xbar[i][j] < 0)
        {
          sprintf(temp, "x%d\'", -myXbar.xbar[i][j]);
          printf("%4s", temp);
        }
        else
        {
          sprintf(temp, "x%d", myXbar.xbar[i][j]);
          printf("%3s ", temp);
        }
      }
    }
    printf("\n");
  }

  return;
}

void save_xbar_to_file(char *fileName, Xbar myXbar)
{
  int i, j;

  FILE *fp = NULL;     // file pointer
  fp = fopen(fileName, "w");

  // start writing xbar to file
  fprintf(fp, "vars %d\n", myXbar.vars);
  fprintf(fp, "rows %d\n", myXbar.rows);
  fprintf(fp, "cols %d\n", myXbar.cols);

  for (i = 0; i < myXbar.rows; i++)
  {
    for (j = 0; j < myXbar.cols; j++)
    {
      if (j == myXbar.cols - 1)
        fprintf(fp, "%d", myXbar.xbar[i][j]);
      else
        fprintf(fp, "%d ", myXbar.xbar[i][j]);
    }
    fprintf(fp, "\n");
  }

  fclose(fp);

  return;
}

int evaluate_xbar(int *varInput, Xbar myXbar)
{
  int i, j, col;
  int **resultantXbar;
  int **visitedNodes;
  char temp[5];

  // if (size != myXbar.vars)
  // {
  //   printf("Incorrect number of variables\n");
  //   return -1;
  // }

  // for now will use recursion, might switch to
  // using a queue later for runtime performance

  // generate crossbar with inputs

  resultantXbar = (int **)malloc(sizeof(int *) * myXbar.rows);
  visitedNodes = (int **)malloc(sizeof(int *) * myXbar.rows);

  for (i = 0; i < myXbar.rows; i++)
  {
    resultantXbar[i] = (int *)malloc(sizeof(int) * myXbar.cols);
    visitedNodes[i] = (int *)malloc(sizeof(int) * myXbar.cols);
    for (j = 0; j < myXbar.cols; j++)
    {
      visitedNodes[i][j] = 0; // setting nodes to unvisited

      if (myXbar.xbar[i][j] == 0)
        resultantXbar[i][j] = 0;
      else if (myXbar.xbar[i][j] == 99)
        resultantXbar[i][j] = 1;
      else if (myXbar.xbar[i][j] > 0)
        resultantXbar[i][j] = varInput[myXbar.xbar[i][j] - 1];
      else if (myXbar.xbar[i][j] < 0)
      {
        if (varInput[-myXbar.xbar[i][j] - 1] == 0)
          resultantXbar[i][j] = 1;
        else if (varInput[-myXbar.xbar[i][j] - 1] == 1)
          resultantXbar[i][j] = 0;
      }
    }
  }

  // at this point, we have generated an instance of the crossbar

  col = 0;

  for (; col < myXbar.cols; col++)
  {
    if (resultantXbar[0][col] == 1)
      if (recursive_xbar_evaluation(resultantXbar, 0, col,
                              visitedNodes, myXbar.rows, myXbar.cols))
      {
        // free 2d arrays
        for (i = 0; i < myXbar.rows; i++)
        {
          free(resultantXbar[i]);
          free(visitedNodes[i]);
        }
        free(resultantXbar);
        free(visitedNodes);

        return 1;
      }
  }

  // free 2d arrays
  for (i = 0; i < myXbar.rows; i++)
  {
    free(resultantXbar[i]);
    free(visitedNodes[i]);
  }
  free(resultantXbar);
  free(visitedNodes);

  return 0;
}

int recursive_xbar_evaluation(int **xbarInstance, int row, int col,
                        int **visited, int maxRows, int maxCols)
{
  int i;

  // check if inbounds
  if (row >= 0 && row < maxRows && col >= 0 && col < maxCols)
  {
    // check if at last row (solved)
    if (row == maxRows - 1)
    {
      // printf("solved\n");
      // printf("%d %d\n", row, col);
      visited[row][col] = 1;
      return 1;
    }

    // advance to down a row
    for (i = row; i < maxRows; i++)
    {
      visited[i][col] = 1;
      if (i + 1 < maxRows)
      {
        if (visited[i + 1][col])
          continue;
        else if (xbarInstance[i + 1][col] == 1)
          if (recursive_xbar_evaluation(xbarInstance, i + 1, col,
                                        visited, maxRows, maxCols))
            return 1;
      }
    }
    // go up a row (prev row, not necessarily prev column)
    for (i = row; i < maxRows; i++)
    {
      visited[i][col] = 1;
      if (i - 1 >= 0)
      {
        if (visited[i - 1][col])
          continue;
        else if (xbarInstance[i - 1][col] == 1)
          if (recursive_xbar_evaluation(xbarInstance, i - 1, col,
                                        visited, maxRows, maxCols))
            return 1;
      }
    }
    // here we are trying both left and right
    // try left
    for (i = col; i >= 0; i--)
    {
      visited[row][i] = 1;
      if (i - 1 >= 0)
      {
        if (visited[row][i - 1])
          continue;
        else if (xbarInstance[row][i - 1] == 1)
          if (recursive_xbar_evaluation(xbarInstance, row, i - 1,
                                        visited, maxRows, maxCols))
            return 1;
      }
    }
    // try right
    for (i = col; i < maxCols; i++)
    {
      visited[row][i] = 1;
      if (i + 1 < maxCols)
      {
        if (visited[row][i + 1])
          continue;
        else if (xbarInstance[row][i + 1] == 1)
          if (recursive_xbar_evaluation(xbarInstance, row, i + 1,
                                        visited, maxRows, maxCols))
            return 1;
      }
    }
  }
  // printf("not solved\n");
  // printf("%d %d\n", row, col);
  return 0;
}

void print_xbar_truth_table(Xbar myXbar)
{
  int j, val, rowLim, *varInstance;
  unsigned int i;
  char temp[5];
  varInstance = (int *)malloc(sizeof(int) * myXbar.vars);
  rowLim = powerOfTwo(myXbar.vars);

  // print out x's and f
  for (i = 0; i < myXbar.vars; i++)
  {
    sprintf(temp, "x%d", myXbar.vars - i);
    printf(" %3s", temp);
  }
  printf("  f\n");

  // populate truth table and compute
  for (i = 0; i < rowLim; i++)
  {
    for (j = 0; j < myXbar.vars; j++)
    {
      // if odd 0, if even 1
      val = (i / powerOfTwo(myXbar.vars - j - 1)) % 2;
      printf("  %d ", val);
      varInstance[myXbar.vars - j - 1] = val;
    }
    printf("  %d\n", evaluate_xbar(varInstance, myXbar));
  }
  free(varInstance);
}

int count_non_constant_memristors(char *filename)
{
  int i, j;
  int nonConstMemristorCount = 0;

  Xbar *myXbar;

  myXbar = read_xbar_from_file(filename);

  for (i = 0; i < myXbar->rows; i++)
    for (j = 0; j < myXbar->cols; j++)
      if (myXbar->xbar[i][j] != 0 && myXbar->xbar[i][j] != 99)
        nonConstMemristorCount++;
  printf("Non-constant memristor count: %d\n", nonConstMemristorCount);

  free_xbar(myXbar);

  return nonConstMemristorCount;
}
