/* ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
 * #                                                           #
 * ############################################################# 
 *
 * solver.c
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
 
 
#include "solver.h"


/* readInstanceFile()
 *
 * This function analyses the instance file according to the
 * DIMACS rules given in 4.1 from
 * http://www.satcompetition.org/2011/rules.pdf. It tries to
 * catch all possible violations of the DIMACS rules. But when
 * the file doesn't fit the rules, the program maybe crash.
 */
void readInstanceFile(char instanceFilePath[], unsigned int ***clauseList, unsigned int ***varList, unsigned short **solution) {
	FILE *instanceFileHandle;									/* File hande for the instance file. */
	char instanceFileLineBuf[S_INSTANCEFILE_LINE_MAXLENGTH]; 	/* Buffer for a line of the instance file. */
	
	unsigned int numClauses = 0;	/* Number of clauses in the instance file (-1 = not yet determined). */
	unsigned int numVars = 0;		/* Number of variables in the instance file (-1 = not yet determined). */
	unsigned int ii; 				/* Loop variable for the clauseList initialisation. */
	unsigned int jj; 				/* Loop variable for the varList initialisation. */
	bool pLineAnalysed = false;		/* Is the "p cnf <nbvar> <nbclauses>" line already founded and analysed? */

	unsigned int analysedClauses = 0;	/* Number of founded and analysed clauses lines in the instance file. (0 is for total number of clauses) */
	unsigned int analysedClauseLit = 0;	/* Number of founded and analysed literals in a clause line (!) in the instance file. (0 is for total number of clause literals) */
	unsigned int kk; 					/* Loop variable for the clause line analysis. */
	char *pEnd; 						/* Pointer needed for the strtol() function in the clause analysis loop. */
	int litTmp; 						/* Temporal variable for one literal in the clause analysis loop. */


	instanceFileHandle = fopen(instanceFilePath, "r");

	if (instanceFileHandle == NULL) {
		pExit("Can't open instance file!\n");
	} else {
		while (fgets(instanceFileLineBuf, S_INSTANCEFILE_LINE_MAXLENGTH, instanceFileHandle) != NULL) {
			if (instanceFileLineBuf[0] == 'p') { 		/* This is the "p cnf <nbvar> <nbclauses>" line. */
				if (pLineAnalysed == false) {
					sscanf(instanceFileLineBuf, "p cnf %d %d", &numVars, &numClauses);
					
					/* Initialise clauseList, variableList and solution. */
					*clauseList = malloc((numClauses + 1) * sizeof(unsigned int *));	/* +1 for the total number of literals in this clause. */
					if (*clauseList == NULL)
						perror("malloc() for clauseList rows failed");
					
					for(ii = 0; ii <= numClauses; ii++) { /* <= for the additional row */
						(*clauseList)[ii] = calloc(numVars, sizeof(unsigned int));
						if ((*clauseList)[ii] == NULL)
							perror("calloc() for clauseList collums failed");
					}
					
					*varList = malloc((numVars + 1) * sizeof(unsigned int *));			/* +1 for the total number of literals in this clause. */
					if (*varList == NULL)
						perror("malloc() for varList rows failed");
					
					for(jj = 0; jj <= numVars; jj++) { /* <= for the additional row */
						(*varList)[jj] = calloc(numClauses, sizeof(unsigned int));
						if ((*varList)[jj] == NULL)
							perror("calloc() for varList collums failed");
					}
					
					*solution = calloc(numVars, sizeof(unsigned short));
					if (*solution == NULL)
						perror("calloc() for solution failed");
						
					
					(*clauseList)[0][0] = numClauses;
					(*varList)[0][0] = numVars;
					
					
					pLineAnalysed = true;
				} else {
					pExit("There is more then \"p cnf <nbvar> <nbclauses>\" line in the instance file!\n");
				}
			} else if (instanceFileLineBuf[0] == 'c') { /* This is a comments line */
				
			} else { 									/* This is a clause line */
				if (pLineAnalysed == false) {
					pExit("There is a clause line before the \"p cnf <nbvar> <nbclauses>\" line in the instance file!\n");
				} else {
					if (analysedClauses > numClauses) {
						pExit("There are more clauses when specified in the \"p cnf %d %d\" line!\n", numVars, numClauses);
					} else {
						analysedClauseLit = 0;
						pEnd = instanceFileLineBuf;

						for (kk = 0; kk < S_INSTANCEFILE_LINE_MAXLENGTH; kk++) { /* Looping over a clause line character by character. */
							litTmp = (int)strtol(pEnd, &pEnd, 10);
						
							if (litTmp == 0) { 															/* This is the DIMACS line end marker. */
								break; 
							} else if (litTmp != 0 && (kk + 1) == S_INSTANCEFILE_LINE_MAXLENGTH) { 		/* Line end but no DIMACS line end marker founded. */
								pExit("In clause line %d the maximum of line length is reached without finding the DIMACS line end marker \"0\"!\n", (analysedClauses + 1));
							} else if ((litTmp > 0 && litTmp > numVars) || (litTmp < 0 && (litTmp * -1) > numVars)) {	/* The variable is not in the given range by. */
								pExit("The variable %d in clause line %d is out of range of %d!\n", litTmp, (analysedClauses + 1), numVars);
							} else {
								(*clauseList)[(analysedClauses + 1)][(analysedClauseLit + 1)] = litTmp;
								if (litTmp > 0) {
									(*varList)[litTmp][0] = (*varList)[litTmp][0] + 1;
									(*varList)[litTmp][(*varList)[litTmp][0]] = analysedClauses;
								} else {
									(*varList)[(litTmp * -1)][0] = (*varList)[(litTmp * -1)][0] + 1;
									(*varList)[(litTmp * -1)][(*varList)[(litTmp * -1)][0]] = analysedClauses;
								}
							}					
							
							analysedClauseLit++;
						}
					
						(*clauseList)[(analysedClauses + 1)][0] = analysedClauseLit; /* Number of founded literals in this clause. */
					}
				}
				
				analysedClauses++;
			}
		} /* End instance file line loop */
		
		if (analysedClauses < numClauses)
			pExit("There are not the same number of clauses when specified in the \"p cnf %d %d\" line!\n", numVars, numClauses);
	}
}


/* solver()
 *
 * This is the main program function. It gets and verifies the
 * program arguments, run the solving process and prints the
 * result.
 */
void solver(unsigned short **solution, char instanceFilePath[], char solver[], unsigned int randomSeed) {
	/* This list contains all clauses with their literals. If
	 * the literal has a negation, the variable is represented
	 * as an negativ integer. The index 0 in the clause
	 * (clauseList[x][0]) contains the number of literals in
	 * this clause.
	 * The index 0 (clauseList[0][0]) contains the total number
	 * of clauses.
	 */
	unsigned int **clauseList;
	
	/* This list contains all variables mapped to the clause
	 * number in which they occur. The index 0 in the variable
	 * (varList[x][0]) contains the total number of clauses.
	 * The index 0 (varList[0][0]) contains the total number
	 * of variables.
	 */
	unsigned int **varList;


	unsigned int ii; /* Loop variable for the clauseList clean up. */
	
	
	readInstanceFile(instanceFilePath, &clauseList, &varList, &(*solution));

	
	/* Clean up! */
	for(ii = 0; ii <= clauseList[0][0]; ii++) free(clauseList[ii]);
	free(clauseList);
	
	free(varList);
}
