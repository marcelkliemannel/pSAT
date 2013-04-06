/* ############################################################# 
 * #                                                           #
 * #                    pSAT - SLS SAT Solver                  #
 * #                                                           #
 * ############################################################# 
 *
 * psat.h
 *
 *    This is the header file for psat.c.
 *
 *
 * #############################################################
 * #                                                           #
 * #   Developed by                                            #
 * #      Marcel Kliemannel <dev[at]marcel-kliemannel.de>      #
 * #                                                           #
 * #############################################################
 */


#ifndef PSAT_H
#define PSAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include "solver.h"


#define PSAT_INSTANCEFILE_PATH_MAXLENGTH 255 	/* The maximum length of the instance file path */
#define PSAT_ALGONAME_MAXLENGTH 10 				/* The maximum length of the algorithm name. */


void pExit(const char errorMessage[], ...);

#endif /* PSAT_H */