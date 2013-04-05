/* ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
 * #                                                           #
 * ############################################################# 
 *
 * poseidonsat.c
 *
 *    This is the main file of poseidonsat.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */
 
 
#include "poseidonsat.h"


/* pExit()
 *
 * This function combines the function printing an error message 
 * and exit the program with the status code defined in EXIT_FAILURE.
 */
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
 * program arguments, run the solving process and prints the
 * result.
 */
int main(int argc, char* argv[]) {
	char instanceFilePath[PS_INSTANCEFILE_PATH_MAXLENGTH];	/* The path to the instance file. It has to be set through the program argument! */
	char algoName[PS_ALGONAME_MAXLENGTH];  					/* The algorithm name. It has to be set through the program argument! */
	unsigned int randomSeed = time(NULL); 					/* The random seed. Default is the current unix time step. */
	
	unsigned int iArgc = 1;	/* 0 is the command to run the program... */
	
	/* The founded solution as an array with the
	 * boolean status of each variable (which are the indices).
	 * The first index (solution[0]) contains the number
	 * of literals.
	 */
	unsigned short *solution;
	int solutionQuality = -1; 	/* The qulaity of the solution = number of unsatisfied clause (-1 = the solution is unknown). */
	unsigned int iSolution; 	/* Loop variable for solution printing */


	/* Argument verification. */
    while (iArgc < argc) {
	   if (strcmp(argv[iArgc], "-f") == 0) { /* The instance file path */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PS_INSTANCEFILE_PATH_MAXLENGTH)
	   				strcpy(instanceFilePath, argv[(iArgc + 1)]);
	   			else
	   				pExit("The path to the instance file can have a maximal length of %d!\n", PS_INSTANCEFILE_PATH_MAXLENGTH);
	   		} else {
	   			pExit("You must specify a path to the instance file after the -f parameter!\n");
	   		}
	   }
	   
	   if (strcmp(argv[iArgc], "-a") == 0) { /* The algorithm name */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PS_ALGONAME_MAXLENGTH)
	   				strcpy(algoName, argv[(iArgc + 1)]);
	   			else
	   				pExit("The algorithm name can have a maximal length of %d!\n", PS_ALGONAME_MAXLENGTH);
	   		} else {
	   			pExit("You must specify a solver algorithm after the -a parameter!\n");
	   		}
	   }
	   
	   if (strcmp(argv[iArgc], "-r") == 0) { /* The random seed */
	   		if ((iArgc + 1) < argc) {
	   			if (atoi(argv[(iArgc + 1)]) >= 0 && atoi(argv[(iArgc + 1)]) < UINT_MAX)
	   				randomSeed = atoi(argv[(iArgc + 1)]);
	   			else
	   				pExit("The random seed has to be and positive integer number between 0 and %u!\n", UINT_MAX);
	   		} else {
	   			pExit("You must specify a random seed after the -r parameter!\n");
	   		}
	   }
	   	   
	   iArgc++;
    }
    
    if (strlen(instanceFilePath) == 0 || strlen(algoName) == 0) pExit("Usage: %s -f -s...\n", argv[0]); /* TO-DO: Usage description */

    
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
