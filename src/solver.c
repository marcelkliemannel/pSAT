/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * solver.c
 *
 *    This file contains the framework for the solving
 *    algorithm.
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


/* updateVarScoreList()
 *
 * Updating the varScoreList based on the current solution
 * candidate.
 * See definition of varScoreList in solver() for more. */
void updateVarScoreList(unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList) {
	unsigned int iVarList;			/* Loop variable for varList. */
	unsigned int iVarListClause;	/* Loop variable for clauses in the varList. */
	short varValue;					/* Current variable status. */
	int curClause; 					/* Current selected clause in the iVarListClause loop. */


	memset((*varScoreList), 0, sizeof(int) * ((*varList)[0][0] + 1));


	for (iVarList = 1; iVarList <= (*varList)[0][0]; iVarList++) {								/* Loop over every variable. */
		varValue = (*solution)[iVarList];

		for (iVarListClause = 1; iVarListClause <= (*varList)[iVarList][0]; iVarListClause++) {	/* Loop over every clause which contains this variable. */
			curClause = (*varList)[iVarList][iVarListClause];
			
			if ((curClause > 0 && (*clauseStatusList)[curClause] > 1) || (curClause < 0 && (*clauseStatusList)[(curClause * -1)] > 1)) {			/* This clause is satisfied by more then one variable, an flip change nothing. */
				continue;
			} else if ((curClause > 0 && (*clauseStatusList)[curClause] == 1) || (curClause < 0 && (*clauseStatusList)[(curClause * -1)] == 1)) {	/* This clause is satisfied by one variable. */
				if ((curClause > 0 && varValue == 1) || (curClause < 0 && varValue == 0))	/* This clause is satisfied by this variable. */
					(*varScoreList)[iVarList] = (*varScoreList)[iVarList] - 1;	/* An flipp will unsatisfied this clause. */
			} else if ((curClause > 0 && (*clauseStatusList)[curClause] == 0) || (curClause < 0 && (*clauseStatusList)[(curClause * -1)] == 0)) {	/* This clause is unsatisfied. */
				if ((curClause > 0 && varValue == 0) || (curClause < 0 && varValue == 1))	/* An flipp will satisfied this clause. */
					(*varScoreList)[iVarList] = (*varScoreList)[iVarList] + 1;
			}
		}
	}
}


/* updateClauseStatusList()
 *
 * Updating the clauseStatusList for all flipped variables.
 * See definition of clauseStatusList in solver() for more. */
void updateClauseStatusList(int **flippedVariables, unsigned short **solution, int ***varList, int **clauseStatusList) {
	unsigned int iFlippedVariables;		/* Loop variable for flippedVariables. */
	unsigned int iVarListClause; 		/* Loop variable for varList. */
	unsigned int unsatisfiedClauses;	/* Number of unsatisfied clauses. */
	unsigned int oldClauseStatus;		/* Number of clauses new satisfied by flipping this variable. */
	unsigned int newClauseStatus;		/* Number of clauses new unsatisfied by flipping this variable. */
	unsigned short varValue;			/* Current variable status. */
	int curClause; 						/* Current selected clause in the iVarClauseList loop. */


	unsatisfiedClauses = (*clauseStatusList)[0];

	for (iFlippedVariables = 1; iFlippedVariables <= (*flippedVariables)[0]; iFlippedVariables++) {								/* Loop over every flipped variable. */
		varValue = (*solution)[(*flippedVariables)[iFlippedVariables]];

		for (iVarListClause = 1; iVarListClause <= (*varList)[(*flippedVariables)[iFlippedVariables]][0]; iVarListClause++) {	/* Loop over every clause which contains this variable. */
			curClause = (*varList)[(*flippedVariables)[iFlippedVariables]][iVarListClause];

			oldClauseStatus = (*clauseStatusList)[(curClause > 0 ? curClause : (curClause * -1))];
			newClauseStatus = oldClauseStatus;
		
			if ((curClause > 0 && varValue == 1) || (curClause < 0 && varValue == 0)) {			/* After the variable flip, the literal is true. */
				newClauseStatus = newClauseStatus + 1;
			} else if ((curClause > 0 && varValue == 0) || (curClause < 0 && varValue == 1)) {	/* After the variable flip, the literal is flase. */
				newClauseStatus = newClauseStatus - 1;
			}

			if (oldClauseStatus == 0 && newClauseStatus > 0) {
				unsatisfiedClauses--;
			} else if (oldClauseStatus > 0 && newClauseStatus == 0) {
				unsatisfiedClauses++;
			}
		
			(*clauseStatusList)[(curClause > 0 ? curClause : (curClause * -1))] = newClauseStatus;
		}
	}
	
	(*clauseStatusList)[0] = unsatisfiedClauses;
}


/* readInstanceFile()
 *
 * This function analyses the instance file according to the
 * DIMACS rules given in 4.1 from
 * http://www.satcompetition.org/2011/rules.pdf. It tries to
 * catch all possible violations of the DIMACS rules. But when
 * the file doesn't fit the rules, the program maybe crash. */
void readInstanceFile(char instanceFilePath[], int ***clauseList, int ***varList, int **varScoreList, unsigned short **solution, int **clauseStatusList, int **flippedVariables) {
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
					
					/* Initialise clauseList, variableList, varScoreList, solution, clauseStatusList and flippedVariables. */
					*clauseList = malloc((numClauses + 1) * sizeof(int *)); /* +1 for index 0 for the number of clauses. */
					if (*clauseList == NULL)
						perror("malloc() for clauseList rows failed");
					
					for(ii = 0; ii <= numClauses; ii++) { 	/* <= for the additional row! */
						(*clauseList)[ii] = calloc((numVars + 1), sizeof(int));	/* +1 for index 0 for the number of literals in this clause. */
						if ((*clauseList)[ii] == NULL)
							perror("calloc() for clauseList collums failed");
					}
					
					(*clauseList)[0][0] = numClauses;
					
					
					*varList = malloc((numVars + 1) * sizeof(int *)); /* +1 for index 0 for the number variables. */
					if (*varList == NULL)
						perror("malloc() for varList rows failed");
					
					for(jj = 0; jj <= numVars; jj++) { 		/* <= for the additional collum ! */
						(*varList)[jj] = calloc((numClauses + 1), sizeof(int)); /* +1 for index 0 for the number of clauses which contains this variable. */
						if ((*varList)[jj] == NULL)
							perror("calloc() for varList collums failed");
					}
					
					(*varList)[0][0] = numVars;
					
					
					*varScoreList = calloc((numVars + 1), sizeof(int)); /* +1 beacuse the variable indices starts at 1 (index 0 unused so far...). */
					if (*varScoreList == NULL)
						perror("calloc() for varScoreList rows failed");
					
					
					*solution = calloc((numVars + 1), sizeof(unsigned short)); /* +1 for the quality of the solution at index 0. */
					if (*solution == NULL)
						perror("calloc() for solution failed");
						
					(*solution)[0] = numVars;
					
					
					*clauseStatusList = calloc((numClauses + 1), sizeof(int)); /* +1 for the index 0 which contains the number of unsatisfied clause. */
					if (*clauseStatusList == NULL)
						perror("calloc() for clauseStatusList failed");
					
					
					*flippedVariables = calloc((numVars + 1), sizeof(int)); /* +1 for the number of flipped variables at index 0. */
					if (*flippedVariables == NULL)
						perror("calloc() for flippedVariables failed");
					
					
					pLineAnalysed = true;
				} else {
					pExit("There is more then one \"p cnf <nbvar> <nbclauses>\" line in the instance file!\n");
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
									(*varList)[litTmp][(*varList)[litTmp][0]] = (analysedClauses + 1);
								} else {
									(*varList)[(litTmp * -1)][0] = (*varList)[(litTmp * -1)][0] + 1;
									(*varList)[(litTmp * -1)][(*varList)[(litTmp * -1)][0]] = (analysedClauses + 1) * -1;
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
			pExit("There are not the same number of clauses in the file when specified in the \"p cnf %d %d\" line!\n", numVars, numClauses);
	}
}


/* solver()
 *
 * The is the main solving function. The idea is to
 * encapsulate the functionality, which are used
 * by all alogorithm.
 * The solving process generate an random initial
 * solution candidate. In each solver iteration step
 * a "get flipped variable" function for the specific
 * algorithm is called to retrieve a list of variables
 * to flipp. Then the status of each clause (see
 * clauseList variable description) and the score of
 * each variable (see varScoreList variable description).
 * The function returns the number of the unsatisfied
 * clauses. */
int solver(unsigned short **solution, char instanceFilePath[], char algoName[]) {
	/* This list contains all clauses with their literals. If
	 * the literal has a negation, the variable is represented
	 * as an negativ integer. The index 0 in the clause
	 * (clauseList[x][0]) contains the number of literals in
	 * this clause.
	 * The index 0 (clauseList[0][0]) contains the total number
	 * of clauses. */
	int **clauseList;
 	
	/* This list contains all variables mapped to the clause
	 * number in which they occur (negative then the literal
	 * has a negation, positive otherwise). The index 0 in the
	 * variable (varList[x][0]) contains the total number of
	 * clauses. The index 0 (varList[0][0]) contains the total
	 * number of variables. */
	int **varList;
 
	/* This list contains the score (the number of clause 
	 * satisfied [positiv] or unsatisfied [negative], if
	 * this variable will be flipped in the next iteration step. */
	int *varScoreList;

	/* For each clause this list holds the number of true
	 * literals. */
	int *clauseStatusList;

	unsigned int restartsCount = 0;		/* Number of restarts. */
	unsigned int solverIterations = 0;	/* Number of solver iterations. This is reseted after each restart! */
	
	unsigned int iRandSolAsgmt; 	/* Loop variable for the random solution assignment. */
	unsigned int iClauseList;		/* Loop variable for clauseList in the random solution assignment. */
	unsigned int iClauseListLit;	/* Loop variable for every literal in the clauseList in the random solution assignment. */
		
	/* The returing value of the getFlippedVariables() function
	 * of the specific alogrithm. Possible return values:
	 * 1 = variables flipped,
	 * 0 = no variables flipped or
	 * -1 = an restart is needed. */
	short getFlippedVariablesStatus;	
	
	int *flippedVariables;			/* The flipped variables selected by the algorithm. */
	unsigned int iFlippedVariables;	/* Loop variable for the flippedVariables. */
	
	unsigned int iclauseListCU; /* Loop variable for the clauseList clean up. */
	unsigned int iVarListCU; 	/* Loop variable for the varList clean up. */
	
	
	readInstanceFile(instanceFilePath, &clauseList, &varList, &varScoreList, &(*solution), &clauseStatusList, &flippedVariables);


	while(restartsCount < S_RESTARTS_MAX) {										/* Restart loop */
		solverIterations = 0;

		/* Generate random solution candidate assignment */
		for (iRandSolAsgmt = 1; iRandSolAsgmt <= varList[0][0]; iRandSolAsgmt++) {
			(*solution)[iRandSolAsgmt] = rand() % 2;
		}

		clauseStatusList[0] = 0;

		for (iClauseList = 1; iClauseList <= clauseList[0][0]; iClauseList++) {							/* Loop over every clause to determine the initialisation of the clauseStatusList with the random solution assignment. */
			clauseStatusList[iClauseList] = 0;
			for (iClauseListLit = 1; iClauseListLit <= clauseList[iClauseList][0]; iClauseListLit++) {	/* Loop over every literal in the clause. */
				if ((clauseList[iClauseList][iClauseListLit] > 0 && (*solution)[clauseList[iClauseList][iClauseListLit]] == 1) ||
					(clauseList[iClauseList][iClauseListLit] < 0 && (*solution)[(clauseList[iClauseList][iClauseListLit] * -1)] == 0)) { /* The clause is now satisfied by this variable. */
						clauseStatusList[iClauseList] = clauseStatusList[iClauseList] + 1; 
				}
			}
			
			if (clauseStatusList[iClauseList] == 0)
				clauseStatusList[0] = clauseStatusList[0] + 1;
		}
		
		updateVarScoreList(&(*solution), &varList, &varScoreList, &clauseStatusList);


		/* Alogrithm (re)initialisation */
		if (strcmp(algoName, "rots") == 0) {			/* Robust Tabu Search (RoTS) */
			if (restartsCount < 1)
				rotsInitialisation(&varList);
			else
				rotsReInitialisation(&varList);
		} else if (strcmp(algoName, "ilssa") == 0) {	/* ILS/SA */
			if (restartsCount < 1)
				ilssaInitialisation(&varList);
		} else {
			pExit("No (re)initialisation function for the solving alogrithm with the name \"%s\" not founded!\n", algoName);
		}


		while(solverIterations < (S_SOLVERITERATIONS_MAXFACTOR * varList[0][0])) { /* The solving process */
			flippedVariables[0] = 0;

			if (strcmp(algoName, "rots") == 0) {			/* Robust Tabu Search (RoTS) */
				getFlippedVariablesStatus = rotsGetFlippedVariables(&flippedVariables, solverIterations, &(*solution), &varList, &varScoreList, &clauseStatusList);
			} else if (strcmp(algoName, "ilssa") == 0) {	/* ILS */
				getFlippedVariablesStatus = ilssaGetFlippedVariables(&flippedVariables, solverIterations, &(*solution), &varList, &varScoreList, &clauseStatusList);
			} else {
				pExit("No \"get flipped variable\" function for the solving alogrithm with the name \"%s\" not founded!\n", algoName);
			}

			
			if (getFlippedVariablesStatus == 1) {			/* Variables flipped */
				for (iFlippedVariables = 1; iFlippedVariables <= flippedVariables[0]; iFlippedVariables++) {	/* Loop over every flipped variable and flip the value in the solution. */
					(*solution)[flippedVariables[iFlippedVariables]] = 1 - (*solution)[flippedVariables[iFlippedVariables]];
				}
				
				updateClauseStatusList(&flippedVariables, &(*solution), &varList, &clauseStatusList);

				updateVarScoreList(&(*solution), &varList, &varScoreList, &clauseStatusList);
				
				if (clauseStatusList[0] == 0)
					break; /* Solution founded */
			} else if (getFlippedVariablesStatus == -1) {	/* An restart is needed */
				break;
			}
			
			solverIterations++;
		}
		
		if (clauseStatusList[0] == 0)
			break; /* Solution founded */
			
		restartsCount++;
	}


	/* Clean up! */
	if (strcmp(algoName, "rots") == 0) {			/* Robust Tabu Search (RoTS) */
		rotsCleanUp();
	} else if (strcmp(algoName, "ilssa") == 0) {	/* ILS/SA */
		ilssaCleanUp();
	}

	for(iclauseListCU = 0; iclauseListCU <= clauseList[0][0]; iclauseListCU++) free(clauseList[iclauseListCU]);
	free(clauseList);
	
	for(iVarListCU = 0; iVarListCU <= varList[0][0]; iVarListCU++) free(varList[iVarListCU]);
	free(varList);


	free(varScoreList);
	
	free(clauseStatusList);
	
	free(flippedVariables);
	
	
	return clauseStatusList[0];
}
