/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * rots.c
 *	
 * 	This file contains the implementation of the 
 *  Robust Tabu Search (RoTS) solving algorithm.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */
 
 
#include "rots.h"


/* External variable for the tabu list.
 * It contains for each variable the last solver
 * iteration when this variable was flipped.
 * The index 0 (tabuList[0]) contains the tabu
 * tenure. */
unsigned int *tabuList;


/* External variable for the solution candidates
 * for the rotsGetFlippedVariables() function.
 * The index 0 (solutionCandidates[0]) is the
 * number of solution candidates. */
unsigned int *solutionCandidates;


/* rotsInitialisation()
 * 
 * Initialisation for the RoTS algorithm. */
void rotsInitialisation(int ***varList) {
	tabuList = calloc((*varList)[0][0] + 1, sizeof(unsigned int));
	if (tabuList == NULL)
		perror("calloc() for solutionCandidates failed");
		
		
	solutionCandidates = calloc((*varList)[0][0] + 1, sizeof(int));
	if (solutionCandidates == NULL)
		perror("calloc() for solutionCandidates failed");
}


/* rotsInitialisation()
 * 
 * Reinitialisation for the RoTS algorithm. */
void rotsReInitialisation(int ***varList) {
	memset(tabuList, 0, ((*varList)[0][0] + 1) * sizeof(unsigned int));
}


/* rotsGetFlippedVariables()
 * 
 * This function implements the
 * - aspiration criterion),
 * - stagnation detection and
 * - selecting variable in respect of the tabu tenure
 * steps of the robust tabu search algorithm.
 * Possible return values:
 * - "1" when at least one variable was flipped or
 * - "0" no variables were flipped. */
short rotsGetFlippedVariables(int **flippedVariables, unsigned int solverIteration, unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList) {
	unsigned int iVarList;	/* Loop variable for the varList */
	int quality;			/* The current quality of and possible solution candidate */
	int highestQuality;		/* The highest quality of possible solution candidates */

	int flippedVariable; 	/* The selected variable to flip */
		
	
	if (solverIteration % (*varList)[0][0] == 0)
		tabuList[0] = ((ROTS_TABUTENURE_MAX + 1) - ROTS_TABUTENURE_MIN) + ROTS_TABUTENURE_MIN;	/* Reset the tabu tenure */


	(*flippedVariables)[0] = 0;
	

	highestQuality = (*clauseStatusList)[0];


	/* Aspiration criterion */
	flippedVariable = rand() % ((*varList)[0][0]) + 1;
	
	if (((*clauseStatusList)[0] - (*varScoreList)[flippedVariable]) < highestQuality) {
		(*flippedVariables)[0] = 1;
		(*flippedVariables)[1] = flippedVariable;
		
		return 1;
	}
		
	
	solutionCandidates[0] = 0;

	for (iVarList = 1; iVarList <= (*varList)[0][0]; iVarList++) {	/* Loop over every variable */
		quality = (*clauseStatusList)[0] - (*varScoreList)[iVarList];

		if (tabuList[iVarList] < (solverIteration - (ROTS_STAGNATION_FACTOR * (*varList)[0][0]))
			&& (solverIteration > (ROTS_STAGNATION_FACTOR * (*varList)[0][0]))) { /* Stagnation detected */
			tabuList[iVarList] = solverIteration;
			
			(*flippedVariables)[0] = 1;
			(*flippedVariables)[1] = iVarList;
			
			return 1;
		}	

		if (tabuList[iVarList] < (solverIteration - tabuList[0])) {
			if (quality < highestQuality) {
				solutionCandidates[0] = 1;
				solutionCandidates[1] = iVarList;
				
				highestQuality = quality;
			} else if(quality == highestQuality) {
				solutionCandidates[0] = solutionCandidates[0] + 1;
				solutionCandidates[solutionCandidates[0]] = iVarList;
			}
		}
		
	}

	if (solutionCandidates[0] >= 1) { /* Select a solution candidate uniformly */
		flippedVariable = solutionCandidates[((rand() % solutionCandidates[0]) + 1)];
		
		(*flippedVariables)[0] = (*flippedVariables)[0] + 1;
		(*flippedVariables)[(*flippedVariables)[0]] = flippedVariable;

		return 1;
	}
	
	
	(*flippedVariables)[0] = 0;

	return 0;
}


/* rotsCleanUp()
 *
 * Deallocate memory which was used by the RoTS
 * algorithm. */
void rotsCleanUp() {
	free(tabuList);
	free(solutionCandidates);
}
