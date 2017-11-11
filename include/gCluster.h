/*
 *
 * Include file for gCluster project
 *
 */
#ifndef GCLUSTER_H
#define GCLUSTER_H
#include <stdio.h>
#include <stdlib.h>
#include "point.h"

/*
 * Definitions
 */
typedef struct tdRawPoint
{
    Point rawData;
    int classAlgo;  // Classification of Algorithm
    int classGT;    // Classification of Ground Truth
} rawPoint;

/*
 *
 *  Functions and Methods
 *
 */

bool isDouble(const char *str);

#endif

