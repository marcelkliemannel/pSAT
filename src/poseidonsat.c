/*
 * ############################################################# 
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
 *
 */
 
 
#include "poseidonsat.h"


/*
 * pExit()
 *
 * This function combines the function printing an error message 
 * and exit the program with the status code defined in EXIT_FAILURE.
 *
 */
void pExit(const char errorMessage[], ...) {
	/* Get all arguments for the formated output. */
    va_list args;
    va_start(args, errorMessage);
    vprintf(errorMessage, args);
    va_end(args);

	exit(EXIT_FAILURE);
}


/*
 * main()
 *
 * This is the main program function. It gets and verifies the
 * program arguments, run the solving process and prints the
 * result.
 *
 */
int main(int argc, char* argv[]) {
	char ifPath[PS_IFPATH_MAXLENGTH]; 		/* The path to the instance file. It has to be set through the program argument! */
	char solver[PS_SOLVERNAME_MAXLENGTH];  	/* The solver name. It has to be set through the program argument! */
	unsigned int randomSeed = time(NULL); 	/* The random seed. Default is the current unix time step. */
	
	int iArgc = 1; /* 0 is the command to run the program... */
	

	/* Argument verification. */
    while (iArgc < argc) {
	   if (strcmp(argv[iArgc], "-i") == 0) { /* The instance file path */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PS_IFPATH_MAXLENGTH)
	   				strcpy(ifPath, argv[(iArgc + 1)]);
	   			else
	   				pExit("The path to the instance file can have a maximal length of %d!\n", PS_IFPATH_MAXLENGTH);
	   		} else {
	   			pExit("You must specify a path to the instance file after the -i parameter!\n");
	   		}
	   }
	   
	   if (strcmp(argv[iArgc], "-s") == 0) { /* The solver name */
	   		if ((iArgc + 1) < argc) {
	   			if (strlen(argv[(iArgc + 1)]) <= PS_SOLVERNAME_MAXLENGTH)
	   				strcpy(solver, argv[(iArgc + 1)]);
	   			else
	   				pExit("The solver name can have a maximal length of %d!\n", PS_SOLVERNAME_MAXLENGTH);
	   		} else {
	   			pExit("You must specify a solver name after the -s parameter!\n");
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
    
    if (strlen(ifPath) == 0 || strlen(solver) == 0) pExit("Usage: %s -i -s...\n", argv[0]); /* TO-DO: Usage description */


    return EXIT_SUCCESS;
}
