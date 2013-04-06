/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * psat.c
 *
 *    This is the main file of pSAT.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */
 
 
#include "psat.h"


/* pExit()
 *
 * This function combines the function printing an error message 
 * and exit the program with the status code defined in EXIT_FAILURE. */
void pExit(const char errorMessage[], ...) {
	/* Get all arguments for the formated output. */
    va_list args;
    va_start(args, errorMessage);
    vprintf(errorMessage, args);
    va_end(args);

	exit(EXIT_FAILURE);
}


/* main()
 *
 * This is the main program function. It gets and verifies the
 * program parameters, run the solving process and prints the
 * result. */
int main(int argc, char* argv[]) {
	char instanceFilePath[PSAT_INSTANCEFILE_PATH_MAXLENGTH];	/* The path to the instance file. It has to be set through the program argument! */
	char algoName[PSAT_ALGONAME_MAXLENGTH];  					/* The algorithm name. It has to be set through the program argument! */
	unsigned int randomSeed = time(NULL); 						/* The random seed. Default is the current unix time step. */
	
	unsigned int iArgc = 1;	/* 0 is the command to run the program... */
	
	/* The founded solution as a array with the
	 * boolean status of each variable (which are the indices).
	 * The first index (solution[0]) contains the number
	 * of literals.
	 */
	unsigned short *solution;
	int solutionQuality = -1; 	/* The qulaity of the solution represented by the number of unsatisfied clause ("-1" = the solution is unknown). */
	unsigned int iSolution; 	/* Loop variable for solution solution */


	/* Argument verification. */
    while (iArgc < argc) {
	   if (strcmp(argv[iArgc], "-h") == 0) { /* The help page */
	   	   printf("-------------------------------------------------------\n");
	   	   printf("-                                                     -\n");
	   	   printf("-                pSAT - SLS SAT Solver                -\n");
	   	   printf("-                                                     -\n");
	   	   printf("- Developed by                                        -\n");
	   	   printf("-   Marcel Kliemannel <dev[at]marcel-kliemannel.de>   -\n");
	   	   printf("-                                                     -\n");
	   	   printf("-   https://github.com/mkliemannel/pSAT               -\n");
	   	   printf("-                                                     -\n");
	   	   printf("-------------------------------------------------------\n");
	   	   printf("\n");
	   	   printf("Program parameters:\n");
	   	   printf("\n");
	   	   printf("    -f  The path to the instance file (with an maximal length of %d). Required!\n", PSAT_INSTANCEFILE_PATH_MAXLENGTH);
	   	   printf("\n");
	   	   printf("    -a  The algorithm name (with an maximal length of %d). Required!\n", PSAT_ALGONAME_MAXLENGTH);
	   	   printf("        Currently implemented are:\n");
	   	   printf("        - Robust Tabu Search (RoTS), name: \"rots\" and\n");
	   	   printf("        - Iterated Local Search with Simulated Annealing (ILS/SA), name: \"ilssa\".\n");
	   	   printf("\n");
	   	   printf("    -r  An random seed as a positive integer number between 0 and %u. Default is the current UNIX timestamp.\n", UINT_MAX);
	   	   printf("\n");
	   	   printf("    -h  This page.\n");
	   		
		   return EXIT_SUCCESS;
	   }
	   
	   if (strcmp(argv[iArgc], "-f") == 0) { /* The instance file path */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PSAT_INSTANCEFILE_PATH_MAXLENGTH)
	   				strcpy(instanceFilePath, argv[(iArgc + 1)]);
	   			else
	   				pExit("The path to the instance file can have an maximal length of %d!\n", PSAT_INSTANCEFILE_PATH_MAXLENGTH);
	   		} else {
	   			pExit("You must specify an path to the instance file after the -f parameter!\n");
	   		}
	   }
	   
	   if (strcmp(argv[iArgc], "-a") == 0) { /* The algorithm name */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PSAT_ALGONAME_MAXLENGTH)
	   				strcpy(algoName, argv[(iArgc + 1)]);
	   			else
	   				pExit("The algorithm name can have an maximal length of %d!\n", PSAT_ALGONAME_MAXLENGTH);
	   		} else {
	   			pExit("You must specify an solver algorithm after the -a parameter!\n");
	   		}
	   }
	   
	   if (strcmp(argv[iArgc], "-r") == 0) { /* The random seed */
	   		if ((iArgc + 1) < argc) {
	   			if (atoi(argv[(iArgc + 1)]) >= 0 && atoi(argv[(iArgc + 1)]) < UINT_MAX)
	   				randomSeed = atoi(argv[(iArgc + 1)]);
	   			else
	   				pExit("The random seed has to be and positive integer number between 0 and %u!\n", UINT_MAX);
	   		} else {
	   			pExit("You must specify an random seed after the -r parameter!\n");
	   		}
	   }
	   	   
	   iArgc++;
    }
    
    if (strlen(instanceFilePath) == 0 || strlen(algoName) == 0) pExit("You must specify at least the instance file path (-f) and the algorithm name (-a). Use \"%s -h\" for more information.\n", argv[0]); /* TO-DO: Usage description */

    
    /* Get a solution */
    srand(randomSeed);

    solutionQuality = solver(&solution, instanceFilePath, algoName);


    /* Print the best known solution */
    if (solutionQuality == -1) {		/* The solution is unknown. */
		printf("The solution is unknown.\n");
	} else if (solutionQuality == 0) {	/* The instance was satisfiable. */
		printf("v ");
		for (iSolution = 1; iSolution <= solution[0]; iSolution++) {
			if (solution[iSolution] == 0) {
				printf("-%d ", iSolution);
			} else if (solution[iSolution] == 1) {
				printf("%d ", iSolution);
			} else {
				pExit("The solution array has at position %d a wrong value of %d.\n", iSolution, solution[iSolution]);
			} 
		}
		printf("0\n");
		
		printf("s SATISFIABLE\n");
	} else { 							/* The instance is unsatisfiable/the termination criterium was reached. */
		printf("s UNSATISFIABLE\n");
	}


    /* Clean up! */
    free(solution);
    

    return EXIT_SUCCESS;
}
