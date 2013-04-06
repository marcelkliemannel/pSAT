/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * rots.h
 *
 *    This is the header file for rots.c.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */


#ifndef ROTS_H
#define ROTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "solver.h"


#define ROTS_TABUTENURE_MIN 10		/* Minimum tabu tenure */
#define ROTS_TABUTENURE_MAX 15		/* Maximum tabu tenure */

/* This factor multiplied with the number of
 * variables for the stagnation detection.
 * Care has to be taken that this number is lower
 * then the maximum solver iterations factor
 * definded in the solver.h. */
#define ROTS_STAGNATION_FACTOR 10	


void rotsInitialisation(int ***varList);

void rotsReInitialisation(int ***varList);

short rotsGetFlippedVariables(int **flippedVariables, unsigned int solverIteration, unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList);

void rotsCleanUp();


#endif /* ROTS_H */