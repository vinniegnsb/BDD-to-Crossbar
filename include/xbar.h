#ifndef __XBAR_H
#define __XBAR_H

typedef struct Xbar {
  int vars;
  int rows;
  int cols;
  int **xbar;
} Xbar;

// Function prototypes
Xbar *read_xbar_from_file(char *path);
void free_xbar(Xbar *myXbar);
void print_xbar(Xbar myXbar);
void save_xbar_to_file(char *fileName, Xbar myXbar);
int evaluate_xbar(int *varInput, Xbar myXbar);
int recursive_xbar_evaluation(int **xbarInstance, int row, int col,
                              int **visited, int maxRows, int maxCols);
void print_xbar_truth_table(Xbar myXbar);
int count_non_constant_memristors(char *filename);

#endif
