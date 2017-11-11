#ifndef DRAW_CLUSTER_H
#define DRAW_CLUSTER_H

#include "gCluster.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include "getCluster.h"
#include "cell.h"
#include "csv-reader.h"
#include "graphs.h"

/*
 * Functions to create SVG strings
 *
 */
string strSVGRect(string opacity, float graphMult, unsigned epsilon, string color, int x, int y);
string strSVGTextCell(float graphMult, unsigned epsilon, int x, int y,
                      unsigned cellID, int cluster, int cellGT);

#endif
