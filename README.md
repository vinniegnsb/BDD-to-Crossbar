# BDD-to-Crossbar
Tool for synthesizing and verifying BDDs and memristor crossbars. Additionally, BDDs can be mapped to corresponding crossbar configurations. 

# Running
After your changes are complete
- run make
- execute the generated "bddxbar" file

# Selecting operations
- See the header files under /include for available functions
- In main.c are commented out lines of example code to perform some basic operations

# Note
The map_bdd_to_xbar_compact() function currently does not function properly when passing in the bdd files for var15, var20, and var25. However, map_bdd_to_xbar() does work properly for all cases.
