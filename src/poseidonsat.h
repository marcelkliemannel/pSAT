/*
 * ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
 * #                                                           #
 * ############################################################# 
 *
 * poseidonsat.h
 *
 *    This is the header file for poseidonsat.c.
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


#ifndef POSEIDONSAT_H
#define POSEIDONSAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include "solver.h"


#define PS_INSTANCEFILE_PATH_MAXLENGTH 255 	/* The maximum length of the instance file path */
#define PS_ALGONAME_MAXLENGTH 10 			/* The maximum length of the algorithm name. */


void pExit(const char errorMessage[], ...);

#endif /* POSEIDONSAT_H */