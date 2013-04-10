/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * solver.h
 *
 *    This is the header file for solver.c.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */


#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include "psat.h"
#include "rots.h"
#include "ilssa.h"


#define S_INSTANCEFILE_LINE_MAXLENGTH 255 /* The maximum length of a line from the instance file. */

#define S_RESTARTS_MAX 1000000000		  	/* The maximum number of possible restarts. */
#define S_SOLVERITERATIONS_MAXFACTOR 10		/* This factor multiplied with the number of variables is the maximum number of solver iterations. */


void updateVarScoreList(unsigned short **solution, int ***varList, int **varScoreList, int **clauseStatusList);

void updateClauseStatusList(int **flippedVariables, unsigned short **solution, int ***varList, int **clauseStatusList);

int solver(unsigned short **solution, char instanceFilePath[], char algoName[]);


#endif /* SOLVER_H */