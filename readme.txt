simulator runs using 3 hard coded algorithms:
- NaiveAlgorithm - our implementation of the algorithm required in ex1
- MoreNaiveAlgorithm - implementation of naive algorithm that doesn't use MOVE operations
- IncorrectAlgorithm - implementation of an algorithm that makes mistakes to check that simulator works properly

Run program with a parameter that represents a path to a directory, which holds directories that represents travels:

For each travel to run correctly!! each directory must consist of:
1) File named "route" 
2) File named "shipPlan"
3) Many files that hold a list of containers to load in each port (as explained in ex1 document)

1) route - must consist of a list of ports of type "XX XXX" with a line seperating each
2) shipPlan - must consist of the shipPlan just like explained in the recommended input output file for ex1.

IMPORTANT - without these two files a travel will not run properly ! 

3) files of type .cargo_data to represent a list of containers in each port:
every cargo_data file will have a line for each container:
<legal container id ISO846>, <weight>, <Destination port>

Results:

Simulator creates a directory named "SimulatorFiles"
In directory will be 4 more directories:
-Naive Algorithm
-MoreNaiveAlgorithm
-IncorrectAlgorithm
-Travel_File_Errors

algorithm directories will hold a directory for every given travel run on that algorithm with each directory holding 3 types of files (if not empty)
- <travelname>AlgoErrors - all mistakes made by algorithm found by simulation
- <travelName>AllOperations - all operations performed by algorithm during travel
- <travelName>cargoDataErrors - errors when collecting cargo information from port (illegal container or invalid format of file)

 Travel_File_Errors will consist of a file of general errors for specific travel:
- <travelName>FileErrors - holds errors due to invalid files needed to create travels, missing file information or too many files errors.


