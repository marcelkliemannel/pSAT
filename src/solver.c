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


/* flipVarQuality()
 *
 * This function determine the quality of given variable with the
 * new value. It counts the (un)satisfied clauses which contains
 * this variable. It returns the difference bewteen unsatisfoed
 * and satisfied clauses. A return of 0 means, that there are
 * no changes in the ratio between unsat./sat. clause. A negative
 * value indicates, that are now more clause satisfied as before
 * and a positive more unsatisfied.
 */
int flipVarQuality(unsigned int var, unsigned short newValue, int ***clauseList, unsigned int **varClauseList, int **clauseStatusList) {
	unsigned int iVarClauseList = 0; /* Loop variable for varClauseList. */
	unsigned int iClauseListLit = 0; /* Loop variable for iClauseList literal (second index). */
	
	unsigned int statisfiedClauses = 0;		/* Number of clauses new statisfied by flipping this variable. */
	unsigned int unstatisfiedClauses = 0;	/* Number of clauses new unstatisfied by flipping this variable. */
	
	
	for (iVarClauseList = 1; iVarClauseList <= (*varClauseList)[0]; iVarClauseList++) {											/* Loop over every clause which contains this variable. */
		if ((*clauseStatusList)[(*varClauseList)[iVarClauseList]] >= 1) { 					/* The clause is satisfied by more then one literal. */
			continue;
		} else if ((*clauseStatusList)[(*varClauseList)[iVarClauseList]] == (var * -1)) {	/* The clause is satisfied by this variable only. */
			unstatisfiedClauses++;
		} else if ((*clauseStatusList)[(*varClauseList)[iVarClauseList]] == 0) {			/* The clause is unsatisfied so far. */
			for (iClauseListLit = 1; iClauseListLit <= (*clauseList)[(*varClauseList)[iVarClauseList]][0]; iClauseListLit++) {	/* Loop over every literal in this clause. */
				if (((*clauseList)[(*varClauseList)[iVarClauseList]][iClauseListLit] == var && newValue == 1) ||
					(((*clauseList)[(*varClauseList)[iVarClauseList]][iClauseListLit] == (var * -1)) && newValue == 0)) {
						statisfiedClauses++;
				}
			}
		}
	}
	
	
	return unstatisfiedClauses - statisfiedClauses;
}


/* readInstanceFile()
 *
 * This function analyses the instance file according to the
 * DIMACS rules given in 4.1 from
 * http://www.satcompetition.org/2011/rules.pdf. It tries to
 * catch all possible violations of the DIMACS rules. But when
 * the file doesn't fit the rules, the program maybe crash.
 */
void readInstanceFile(char instanceFilePath[], int ***clauseList, unsigned int ***varList, unsigned short **solution, int **clauseStatusList) {
	FILE *instanceFileHandle;									/* File hande for the instance file. */
	char instanceFileLineBuf[S_INSTANCEFILE_LINE_MAXLENGTH]; 	/* Buffer for a line of the instance file. */
	
	unsigned int numClauses = 0;	/* Number of clauses in the instance file. */
	unsigned int numVars = 0;		/* Number of variables in the instance file. */
	unsigned int ii; 				/* Loop variable for the clauseList initialisation. */
	unsigned int jj; 				/* Loop variable for the varList initialisation. */
	bool pLineAnalysed = false;		/* Is the "p cnf <nbvar> <nbclauses>" line already founded and analysed? */

	unsigned int analysedClauses = 0;		/* Number of founded and analysed clauses lines in the instance file. (0 is for total number of clauses) */
	unsigned int analysedClauseLit = 0;		/* Number of founded and analysed literals in a clause line (!) in the instance file. (0 is for total number of clause literals) */
	unsigned int kk; 						/* Loop variable for the clause line analysis. */
	char *pEnd; 							/* Pointer needed for the strtol() function in the clause analysis loop. */
	int litTmp; 							/* Temporal variable for one literal in the clause analysis loop. */


	instanceFileHandle = fopen(instanceFilePath, "r");

	if (instanceFileHandle == NULL) {
		pExit("Can't open instance file!\n");
	} else {
		while (fgets(instanceFileLineBuf, S_INSTANCEFILE_LINE_MAXLENGTH, instanceFileHandle) != NULL) {
			if (instanceFileLineBuf[0] == 'p') { 		/* This is the "p cnf <nbvar> <nbclauses>" line. */
				if (pLineAnalysed == false) {
					sscanf(instanceFileLineBuf, "p cnf %d %d", &numVars, &numClauses);
					
					/* Initialise clauseList, variableList, solution and clauseStatusList. */
					*clauseList = malloc((numClauses + 1) * sizeof(int *)); 		/* +1 for index 0 for the number of clauses. */
					if (*clauseList == NULL)
						perror("malloc() for clauseList rows failed");
					
					for(ii = 0; ii <= numClauses; ii++) { /* <= for the additional row! */
						(*clauseList)[ii] = calloc((numVars + 1), sizeof(int));	/* +1 for index 0 for the number of literals in this clause. */
						if ((*clauseList)[ii] == NULL)
							perror("calloc() for clauseList collums failed");
					}
					
					(*clauseList)[0][0] = numClauses;
					
					
					*varList = malloc((numVars + 1) * sizeof(unsigned int *)); 				/* +1 for index 0 for the number variables. */
					if (*varList == NULL)
						perror("malloc() for varList rows failed");
					
					for(jj = 0; jj <= numVars; jj++) { /* <= for the additional collum ! */
						(*varList)[jj] = calloc((numClauses + 1), sizeof(unsigned int)); 	/* +1 for index 0 for the number of clauses which contains this variable. */
						if ((*varList)[jj] == NULL)
							perror("calloc() for varList collums failed");
					}
					
					(*varList)[0][0] = numVars;
					
					
					*solution = calloc((numVars + 1), sizeof(unsigned short)); 				/* +1 for the quality of the solution at index 0. */
					if (*solution == NULL)
						perror("calloc() for solution failed");
						
					
					*clauseStatusList = calloc((numClauses + 1), sizeof(int)); 	/* +1 beacuse the clauses indices starts at 1 (index 0 unused so far...). */
					if (*clauseStatusList == NULL)
						perror("calloc() for clauseStatusList failed");
					
					
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
void solver(unsigned short **solution, char instanceFilePath[], char algoName[]) {
	/* This list contains all clauses with their literals. If
	 * the literal has a negation, the variable is represented
	 * as an negativ integer. The index 0 in the clause
	 * (clauseList[x][0]) contains the number of literals in
	 * this clause.
	 * The index 0 (clauseList[0][0]) contains the total number
	 * of clauses.
	 */
	int **clauseList;
	
	/* This list contains all variables mapped to the clause
	 * number in which they occur. The index 0 in the variable
	 * (varList[x][0]) contains the total number of clauses.
	 * The index 0 (varList[0][0]) contains the total number
	 * of variables.
	 */
	unsigned int **varList;
	
	/* For each clause this list holds the number of true
	 * literals.
	 * A value of 0 means, the clause is unsatisfied so far.
	 * A negative value mena, that the clause is satisfied
	 * by exactly one variable. The index of this variable
	 * is the negative value (seen as a positive integer).
	 */
	int *clauseStatusList;

	unsigned int restartsCount = 0;		/* Number of restarts. */
	unsigned int solverIterations = 0;	/* Number of solver iterations. This is reseted after each restart! */
	unsigned int iRandSolAsgmt; 		/* Loop variable for the random solution assignment. */
	unsigned int iClauseList = 0;		/* Loop variable for clauseList after the random solution assignment. */
	unsigned int iClauseListLit = 0;	/* Loop variable for every literal in the clauseList. */
	int clauseStatus = 0;				/* Current clause status inside the clauseStatusList initialisation. */
	
	unsigned int iclauseListCU; /* Loop variable for the clauseList clean up. */
	unsigned int iVarListCU; 	/* Loop variable for the varList clean up. */
	
	
	readInstanceFile(instanceFilePath, &clauseList, &varList, &(*solution), &clauseStatusList);


	while(restartsCount < S_RESTARTS_MAX) {
		/* Search initialisation. */
		solverIterations = 0;

		for (iRandSolAsgmt = 0; iRandSolAsgmt < varList[0][0]; iRandSolAsgmt++) /* Random solution assignment. */
			(*solution)[iRandSolAsgmt] = rand() % 2;
	
		for (iClauseList = 1; iClauseList <= clauseList[0][0]; iClauseList++) {							/* Loop over every clause to determine the initialisation of the clauseStatusList with the random solution assignment. */
			clauseStatus = 0;
			
			for (iClauseListLit = 1; iClauseListLit <= clauseList[iClauseList][0]; iClauseListLit++) {	/* Loop over every literal in the clause. */
				if ((clauseList[iClauseList][iClauseListLit] > 0 && (*solution)[clauseList[iClauseList][iClauseListLit]] == 1) ||
					(clauseList[iClauseList][iClauseListLit] < 0 && (*solution)[(clauseList[iClauseList][iClauseListLit] * -1)] == 0)) {
					if (clauseStatus == 0) {
						if (clauseList[iClauseList][iClauseListLit] > 0) clauseStatus = clauseList[iClauseList][iClauseListLit] * -1;
						else  clauseStatus = clauseList[iClauseList][iClauseListLit];
					} else if (clauseStatus < 0) {
						clauseStatus = 2;
					} else {
						clauseStatus++;
					}
				}
			}
			
			clauseStatusList[iClauseList] = clauseStatus;
		}
		
		
		
		
		/* The solving process. */
		while(solverIterations < (S_SOLVERITERATIONS_MAXFACTOR * varList[0][0])) {
			
			
			solverIterations++;
		}
		
		restartsCount++;
	}

	
	/* Clean up! */
	for(iclauseListCU = 0; iclauseListCU < varList[0][0]; iclauseListCU++) free(clauseList[iclauseListCU]);
	free(clauseList);
	
	for(iVarListCU = 0; iVarListCU < clauseList[0][0]; iVarListCU++) free(varList[iVarListCU]);
	free(varList);
	
	free(clauseStatusList);
}
