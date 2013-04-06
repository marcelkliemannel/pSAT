/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * ilssa.h
 *
 *    This is the header file for ilssa.c.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */


#ifndef ILSSA_H
#define ILSSA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "solver.h"


#define ILSSA_TEMPERATURE_MAX 0.3		/* Maximum temperature */
#define ILSSA_TEMPERATURE_MIN 0.01		/* Minimum temperature */


void ilssaInitialisation(int ***varList);

short ilssaGetFlippedVariables(int **flippedVariables, unsigned int solverIteration, unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList);

void ilssaCleanUp();


#endif /* ILSSA_H */