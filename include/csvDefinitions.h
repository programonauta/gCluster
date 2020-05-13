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
 * dimension + 4  int       adj-n           Id of adjacent's cells
 *
 */

#define resultgC_qtyFixedLabels 4
#define resultgC_posDimension() 0
#define resultgC_posCellId() dimension + 0
#define resultgC_posQtyPts() dimension + 1
#define resultgC_posConnCells() dimension + 2
#define resultgC_posGCLabel() dimension + 3
#define resultgC_posAdjacent() dimension + 4

/* Program: mapCluster
 * ===================
 *
 * File output (default name): map-cell-gC.csv
 * Prefix: mapC
 * Description: file with cells values after statCluster algorithm,
 *              In this file there is a correspondence of labels from gCluster and GT
 *              The format is almost the same, the only difference is the GT column
 *
 * Field number   Type      Name            Description
 *--------------------------------------------------------------------------
 * [0..z]         double    CM-x            Coordinates of Center of Mass
 * dimension + 0  int       cell-id         Identification of cell
 * dimension + 1  int       label-gC        Label of cluster found out by gCluster
 * dimension + 2  int       GT-label        Label of Ground Truth
 * dimension + 3  int       adj-n           Id of adjacent's cells
 *
 */

#define mapC_qtyFixedLabels 3
#define mapC_posDimension() 0
#define mapC_posCellId() dimension + 0
#define mapC_posGCLabel() dimension + 1
#define mapC_posGT() dimension + 2
#define mapC_posAdjacent() dimension + 3



#endif
