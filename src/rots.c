/* ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
 * #                                                           #
 * ############################################################# 
 *
 * rots.c
 *	
 * 	This file contains the implementation of the 
 *  Robust Tabu Search (RoTS) solving alogirthm.
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


unsigned int tabuTenure;
unsigned int *tabuList;
unsigned int numSolutionCandidates;
unsigned int *solutionCandidates;


void rotsInitialisation(unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList) {
	tabuList = calloc((*varList)[0][0] + 1, sizeof(unsigned int));
	solutionCandidates = calloc((*varList)[0][0], sizeof(unsigned int));
}


void rotsReInitialisation(unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList) {
	memset(tabuList, 0, ((*varList)[0][0] + 1) * sizeof(unsigned int));
}


int rotsGetFlippedVariable(unsigned int solverIteration, unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList) {
	int flippedVariable;
	unsigned int ii, quality, hightestQuality;
	
	
	hightestQuality = (*clauseStatusList)[0];

	if (solverIteration % (*varList)[0][0] == 0)
		tabuTenure =  ((ROTS_TABUTENURE_MAX + 1) - ROTS_TABUTENURE_MIN) + ROTS_TABUTENURE_MIN;


	/* Aspiration criterion */
	flippedVariable = rand() % ((*varList)[0][0]) + 1;
	
	if (varScore(flippedVariable, false, &(*solution), &((*varList)[flippedVariable]), &(*clauseStatusList)) < hightestQuality)
		return flippedVariable;
		
	
	numSolutionCandidates = 0;

	for (ii = 1; ii <= (*varList)[0][0]; ii++) {
		quality = varScore(ii, false, &(*solution),  &((*varList)[ii]), &(*clauseStatusList));
		

		if (tabuList[ii] < (solverIteration - (10 * (*varList)[0][0]))) {
			tabuList[ii] = ii;
			return ii; /* Stagnation detected */
		}	

		if (tabuList[ii] < (solverIteration - tabuTenure)) {
			if (quality < hightestQuality) {
				numSolutionCandidates = 1;
				solutionCandidates[0] = ii;
				hightestQuality = quality;
			} else if(quality == hightestQuality) {
				solutionCandidates[numSolutionCandidates] = ii;
				numSolutionCandidates++;
			}
		}
		
	}
	
	if (numSolutionCandidates >= 1) {
		flippedVariable = solutionCandidates[rand() % numSolutionCandidates];
		tabuList[flippedVariable] = solverIteration;
		
		
		return flippedVariable;
	}
	
	
	return -1;
}