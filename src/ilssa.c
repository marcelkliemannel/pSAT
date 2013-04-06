/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * ilssa.c
 *	
 * 	This file contains the implementation of the 
 *  Iterated Local Search with Simulated Annealing (ILS/SA)
 *  solving alogrithm.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */
 
 
#include "ilssa.h"


/* External variable for the solution candidates
 * for the ilssaSimulatedAnnealing() function.
 * The index 0 (solutionCandidates[0]) is the
 * number of solution candidates. */
unsigned int *solutionCandidates;


/* ilssaSimulatedAnnealing()
 *
 * This function implements the simulated annealing algorithm.
 * It returns:
 * - "-1" when a restart is needed (the temperature is under
 *   the minimum temperature),
 * - "0" no variable flipped or
 * - "1" at least one variable was flipped. */
short ilssaSimulatedAnnealing(int **flippedVariables, unsigned int solverIteration, int ***varList, int **varScoreList, int **clauseStatusList) {
	unsigned int iVarList;	/* Loop variable for the varList */
	int quality;			/* The current quality of and possible solution candidate */
	int highestQuality;		/* The highest quality of possible solution candidates */
	double temperature;		/* The current temperature */
	double probability;		/* The probability of the current solution candidate */
	double random;			/* An random barrier for the probability */
	
	int flippedVariable;	/* The selected variable to flip */
	
	
	
	temperature = ILSSA_TEMPERATURE_MAX * exp(-1.0 * (double)solverIteration * (1.0 / (double)(*varList)[0][0]));

	if (temperature < ILSSA_TEMPERATURE_MIN)
		return -1; /* An restart is needed */
		
	
	highestQuality = (*clauseStatusList)[0];

	solutionCandidates[0] = 0;

	for (iVarList = 1; iVarList <= (*varList)[0][0]; iVarList++) {	/* Loop over every variable */
		quality = (*clauseStatusList)[0] - (*varScoreList)[iVarList];
		
		/* Acceptance criterium */
		if (quality < highestQuality) {
			solutionCandidates[0] = 1;
			solutionCandidates[1] = iVarList;
			highestQuality = quality;
		} else if(quality == highestQuality) {
			/* Metropolis condition */
			probability = (1.0 / (1.0 + exp(-1.0 * ((double)((*clauseStatusList)[0] - quality) / temperature))));
			random = (double)rand() / (double)RAND_MAX;
			
			if (random > probability) {
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

	return 0;
}


/* ilssaInitialisation()
 * 
 * Initialisation for the ILS/SA algorithm. */
void ilssaInitialisation(int ***varList) {
	solutionCandidates = calloc(((*varList)[0][0] + 1), sizeof(unsigned int));
	if (solutionCandidates == NULL)
		perror("calloc() for solutionCandidates failed");
}


/* ilssaGetFlippedVariables()
 * 
 * This function implements the
 * - initial subsidiary local search (solverIteration == 0),
 * - perturbation,
 * - subsidiary local search and
 * - acceptance criterium
 * steps of the ILS-Framework which using simulated
 * annealing as a subsidiary local search.
 * See ilssaSimulatedAnnealing() for the returning value. */
short ilssaGetFlippedVariables(int **flippedVariables, unsigned int solverIteration, unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList) {
	int perturbFlippedVariable;	/* Flipped variable in the perturbation step. */
	
	short saStatus;	/* The returning value of the ilssaSimulatedAnnealing(), also the returning value of this function. */


	(*flippedVariables)[0] = 0;

	
	/* Perturbation 
	 * 
	 * The ILS-Framework applies after the solution candidate
	 * initialisation a subsidiary local search to find a
	 * first local optima. For this, the solver iteration step
	 * 0 can be seen as the "initialisation" step.*/
	if (solverIteration > 0) {
		(*flippedVariables)[0] = 1;
		
		perturbFlippedVariable = (rand() % (*varList)[0][0] + 1);
		(*flippedVariables)[1] = perturbFlippedVariable;
		(*solution)[perturbFlippedVariable] = 1 - (*solution)[perturbFlippedVariable];


		updateClauseStatusList(&(*flippedVariables), &(*solution), &(*varList), &(*clauseStatusList));
	}
	
	
	/* Subsidiary local search and acceptance criterium */
	saStatus = ilssaSimulatedAnnealing(&(*flippedVariables), solverIteration, &(*varList), &(*varScoreList), &(*clauseStatusList));
	
	
	return saStatus;
}


/* ilssaCleanUp()
 *
 * Deallocate memory which was used by the ILS/SA
 * algorithm. */
void ilssaCleanUp() {
	free(solutionCandidates);
}
