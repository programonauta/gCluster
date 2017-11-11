#ifndef CSV_DEFINITIONS
#define CSV_DEFINITIONS
/*
 * File to define positions of CSV Files
 *
 * Program: sumData
 * ================
 *
 * Consolidated Points
 * -------------------
 * File output (default name): points.csv
 * prefix: consolidPts
 * Description: File with normalized points and Ground Truth
 *
 * Field number   Type      Name               Description
 *------------------------------------------------------------------
 * [0..z]         double    Get from config    Coordinates of Point
 * dimension + 0  int       Get from config(*) Ground Truth
 *
 * (*) If there is no ground truth the header will be the string "_#NO_CLASS_#
 */
#define consolidPts_posDimension() 0
#define consolidPts_posGT() dimension + 0

 /* Program: getCluster
 * ===================
 *
 * File output (default name): result-gCluster.csv
 * Prefix: resultgC
 * Description: file with cells values after gGluster algorithm
 *
 * Field number   Type      Name            Description
 *--------------------------------------------------------------------------
 * [0..z]         double    CM-x            Coordinates of Center of Mass
 * dimension + 0  int       cell-id         Identification of cell
 * dimension + 1  unsigned  qty-points      Quantity of points inside the cell
 * dimension + 2  unsigned  connect-cells   Number of cells that forms the cluster
 * dimension + 3  int       gCluster-label  Label of cluster
 *
 */

#define resultgC_qtyFixedLabels 4
#define resultgC_posDimension() 0
#define resultgC_posCellId() dimension + 0
#define resultgC_posNumberPts() dimension + 1
#define resultgC_posConnCells() dimension + 2
#define resultgC_posGCLabel() dimension + 3

#endif
