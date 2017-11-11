/*
*
* Include file for getCluster project
*
*/
#ifndef GETCLUSTER_H
#define GETCLUSTER_H
#include "point.h"

typedef struct tdRawPoint
{
    Point rawPoint;
    int classAlgo;  // Classification of Algorithm
    int classGT;    // Classification of Ground Truth
} pointGT;

#endif
