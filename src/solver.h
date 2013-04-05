/* ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
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

#include "poseidonsat.h"
#include "rots.h"


#define S_INSTANCEFILE_LINE_MAXLENGTH 255 /* The maximum length of a line from the instance file. */

#define S_RESTARTS_MAX 10000						/* The maximum number of possible restarts. */
#define S_SOLVERITERATIONS_MAXFACTOR 100 		/* The maximum number of solver iterations = this factor multiplied with the number of variables. */


int varScore(unsigned int var, bool saveNewClauseStatus, unsigned short **solution, int **varListClauses, int **clauseStatusList);

int solver(unsigned short **solution, char instanceFilePath[], char algoName[]);


#endif /* SOLVER_H */