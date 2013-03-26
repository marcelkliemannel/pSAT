/*
 * ############################################################# 
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
 *
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


#define S_INSTANCEFILE_LINE_MAXLENGTH 255 /* The maximum length of a line from the instance file. */


void solver(unsigned short **solution, char instanceFilePath[], char solver[], unsigned int randomSeed);


#endif /* SOLVER_H */