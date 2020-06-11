Run program with a parameter that represents a path to a directory, which holds directories that represents travels:

For each travel to run correctly!! each directory must consist of:
1) A single file with .route extension
2) A single file named .ship_plan extension
3) Many files that hold a list of containers to load in each port (as explained in ex1 document)

NOTE - program does not support multiple .route or .ship_plan files, there must be only one of each in each travel directory !

program output:
in the output file given to the program in the command line via -output flag
if the output is legal, program will create multiple directories in the given output (or create the path if it does not exist) and if the path is un-writeable will work in current directory

In directory will be a directory for each travel_algo pair as stated in instructions if and only if the travel was legal and algorithm was successfully loaded

In the output file there will be a directory named errors which will consist of multiple types of error files:
- <travel name>.general_errors (errors regarding missing/unused file in travel directory and .route , .ship_plan errors)
- command.errors (in case of illegal command line arguments for program)
- <algo Name>_<travel Name>.errors (for errors that occurred in simulation of algorithm on the travel, including algorithm reported errors)
- algoRegistration.errors (consists of errors regarding dynamic loading errors of .so files)


Run Examples:
We included two different Travels:
1) Travel1
2) Travel2

In these travels our algorithms run differently and create a different amount of operations
