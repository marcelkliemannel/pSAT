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
 * The index 0 (ilssaSolutionCandidates[0]) is the
 * number of solution candidates. */
int *ilssaSolutionCandidates;


/* External variable for the tabu variables
 * which are flipped in the perturbation
 * phase.
 * The index 0 (ilssaTabuVariables[0]) is the
 * number of tabu variables. */
int *ilssaTabuVariables;


/* ilssaSimulatedAnnealing()
 *
 * This function implements the simulated annealing algorithm.
 * It returns:
 * - "-1" when a restart is needed (the temperature is under
 *   the minimum temperature),
 * - "0" no variable flipped or
 * - "1" at least one variable was flipped. */
short ilssaSimulatedAnnealing(int **flippedVariables, unsigned int solverIteration, int ***varList, int **varScoreList, int **clauseStatusList) {
	unsigned int iVarList;			/* Loop variable for the varList */
	unsigned int iTabuVariables;	/* Loop variable for ilssaTabuVariables */
	bool isTabu;					/* Indicates, if the variable is tabu for this step. */
	int quality;					/* The current quality of and possible solution candidate */
	int highestQuality;				/* The highest quality of possible solution candidates */
	double temperature;				/* The current temperature */
	double probability;				/* The probability of the current solution candidate */
	double random;					/* An random barrier for the probability */
	
	int flippedVariable;	/* The selected variable to flip */
	
	
	
	temperature = ILSSA_TEMPERATURE_MAX * exp(-1.0 * (double)solverIteration * (1.0 / (double)(*varList)[0][0]));

	if (temperature < ILSSA_TEMPERATURE_MIN)
		return -1; /* A restart is needed */
		
	
	highestQuality = (*clauseStatusList)[0];

	ilssaSolutionCandidates[0] = 0;

	for (iVarList = 1; iVarList <= (*varList)[0][0]; iVarList++) {	/* Loop over every variable */
		isTabu = false;
		for (iTabuVariables = 1; iTabuVariables <= ilssaTabuVariables[0]; iTabuVariables++) { /* Loop over every tabu variable */
			if (ilssaTabuVariables[iTabuVariables] == iVarList) {  /* This variable was flipped by the perturbation phase and is tabu for this iteration. */
				isTabu = true;
				break;
			}
		}
		
		if (isTabu == true)
			continue;
		

		quality = (*clauseStatusList)[0] - (*varScoreList)[iVarList];
		
		/* Acceptance criterium */
		if (quality < highestQuality) {
			ilssaSolutionCandidates[0] = 1;
			ilssaSolutionCandidates[1] = iVarList;
			highestQuality = quality;
		} else if(quality == highestQuality) {
			/* Metropolis condition */
			probability = (1.0 / (1.0 + exp(-1.0 * ((double)((*clauseStatusList)[0] - quality) / temperature))));
			random = (double)rand() / (double)RAND_MAX;
			
			if (random > probability) {
				ilssaSolutionCandidates[0] = ilssaSolutionCandidates[0] + 1;
				ilssaSolutionCandidates[ilssaSolutionCandidates[0]] = iVarList;
			}
		}
		
	}

	if (ilssaSolutionCandidates[0] >= 1) { /* Select a solution candidate uniformly */
		flippedVariable = ilssaSolutionCandidates[((rand() % ilssaSolutionCandidates[0]) + 1)];
		
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
	ilssaSolutionCandidates = calloc(((*varList)[0][0] + 1), sizeof(int));
	if (ilssaSolutionCandidates == NULL)
		perror("calloc() for ilssaSolutionCandidates failed");

	ilssaTabuVariables = calloc(((*varList)[0][0] + 1), sizeof(int));
	if (ilssaTabuVariables == NULL)
		perror("calloc() for ilssaTabuVariables failed");
		
		ilssaTabuVariables[0] = 1;
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
		
		ilssaTabuVariables[0] = 1;
		ilssaTabuVariables[1] = perturbFlippedVariable;

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
	free(ilssaSolutionCandidates);
	
	free(ilssaTabuVariables);
}
