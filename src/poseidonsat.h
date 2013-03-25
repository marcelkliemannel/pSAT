/*
 * ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
 * #                                                           #
 * ############################################################# 
 *
 * poseidonsat.c
 *
 *    This is the main file of the sls sat solver.
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
#include <math.h>
#include <regex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>


#define PS_IFPATH_MAXLENGTH 255 /* The maximum length of the path to the instance file. */
#define PS_SOLVERNAME_MAXLENGTH 10 /* The maximum length of the solver name. */


void pExit(const char errorMessage[], ...);

#endif /* POSEIDONSAT_H */