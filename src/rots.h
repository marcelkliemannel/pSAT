/* ############################################################# 
 * #                                                           #
 * #                 poseidonsat - SLS SAT Solver              #
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


#define ROTS_TABUTENURE_MIN 5
#define ROTS_TABUTENURE_MAX 10


void rotsInitialisation(unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList);

void rotsReInitialisation(unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList);

int rotsGetFlippedVariable(unsigned int solverIerations, unsigned short **solution, int ***clauseList, int ***varList, int **clauseStatusList);


#endif /* ROTS_H */