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
    int cellId;     // Id of cell to which point belongs
    int classAlgo;  // Classification of Algorithm
    int classGT;    // Classification of Ground Truth
    Point CM;       // Center of Mass of correspondent cell
} rawPoint;

/*
 *
 *  Functions and Methods
 *
 */

bool isDouble(const char *str);

#endif

