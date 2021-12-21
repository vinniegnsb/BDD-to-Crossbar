#include <stdio.h>
#include <stdlib.h>
#include "general_functions.h"
#include "bdd.h"
#include "xbar.h"

int main(int argc, char **argv)
{
  // BDD *testBDD = read_bdd_from_file("bdds/my6var.bdd");
  // print_bdd_truth_table(*testBDD);
  // free_bdd(testBDD);
  // printf("\n");
  // printf("\n");

  // map_bdd_to_xbar_compact("bdds/my6var.bdd", "xbars/my6var_compact2.xbar");
  //
  // Xbar *testXbar = read_xbar_from_file("xbars/my6var_compact2.xbar");
  // print_xbar_truth_table(*testXbar);
  // print_xbar(*testXbar);
  // free_xbar(testXbar);
  // printf("\n");

  clock_t start_t, end_t, total_t;

  // *** Question 2a ***

  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/test1.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_test1");
  //
  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/test2.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_test2");
  //
  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/test3.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_test3");
  //
  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/test4.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_test4");
  //
  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/test5.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_test5");

  // *** Question 2b ***

  // start_t = clock();
  // equivalence_check("bdds/var2.bdd", "xbars/var2.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var2_var2");
  //
  // start_t = clock();
  // equivalence_check("bdds/var5.bdd", "xbars/var5.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_var5");
  //
  // start_t = clock();
  // equivalence_check("bdds/var10.bdd", "xbars/var10.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var10_var10");
  //
  // start_t = clock();
  // equivalence_check("bdds/var15.bdd", "xbars/var15.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var15_var15");
  //
  // start_t = clock();
  // equivalence_check("bdds/var20.bdd", "xbars/var20.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var20_var20");
  //
  // start_t = clock();
  // equivalence_check("bdds/var25.bdd", "xbars/var25.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var25_var25");

  // *** Question 3 w/ Optional verification ***

  // start_t = clock();
  // map_bdd_to_xbar("bdds/var2.bdd", "map2.xbar");
  // // equivalence_check("bdds/var2.bdd", "map2.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var2_map2");
  //
  // start_t = clock();
  // map_bdd_to_xbar("bdds/var5.bdd", "map5.xbar");
  // // equivalence_check("bdds/var5.bdd", "map5.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var5_map5");
  //
  // start_t = clock();
  // map_bdd_to_xbar("bdds/var10.bdd", "map10.xbar");
  // // equivalence_check("bdds/var10.bdd", "map10.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var10_map10");
  //
  // start_t = clock();
  // map_bdd_to_xbar("bdds/var15.bdd", "map15.xbar");
  // // equivalence_check("bdds/var15.bdd", "map15.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var15_map15");
  //
  // start_t = clock();
  // map_bdd_to_xbar("bdds/var20.bdd", "map20.xbar");
  // // equivalence_check("bdds/var20.bdd", "map20.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var20_map20");
  //
  // start_t = clock();
  // map_bdd_to_xbar("bdds/var25.bdd", "map25.xbar");
  // // equivalence_check("bdds/var25.bdd", "map25.xbar");
  // end_t = clock();
  // display_time(start_t, end_t, "var25_map25");

  // project 3 Task 5.1

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var2.bdd", "map2_compact.xbar");
  equivalence_check("bdds/var2.bdd", "map2_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var2_map2");
  count_non_constant_memristors("map2_compact.xbar");

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var5.bdd", "map5_compact.xbar");
  equivalence_check("bdds/var5.bdd", "map5_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var5_map5");
  count_non_constant_memristors("map5_compact.xbar");

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var10.bdd", "map10_compact.xbar");
  equivalence_check("bdds/var10.bdd", "map10_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var10_map10");
  count_non_constant_memristors("map10_compact.xbar");

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var15.bdd", "map15_compact.xbar");
  equivalence_check("bdds/var15.bdd", "map15_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var15_map15");
  count_non_constant_memristors("map15_compact.xbar");

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var20.bdd", "map20_compact.xbar");
  equivalence_check("bdds/var20.bdd", "map20_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var20_map20");
  count_non_constant_memristors("map20_compact.xbar");

  start_t = clock();
  map_bdd_to_xbar_compact("bdds/var25.bdd", "map25_compact.xbar");
  equivalence_check("bdds/var25.bdd", "map25_compact.xbar");
  end_t = clock();
  display_time(start_t, end_t, "var25_map25");
  count_non_constant_memristors("map25_compact.xbar");

  return 0;
}
