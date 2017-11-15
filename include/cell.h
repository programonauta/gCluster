//
// Header of Cell class
//
#ifndef CELL_H
#define CELL_H

#include <vector>
#include "point.h"

class Cell
{
public:

/*
 * Public Attributes
 *
 */
    // Coordinates of cel
    vector<int> coord;  // Cell coordinates
/*
 *
 * Public Methods
 *
 */
    // Constructor
    Cell(int dimen = 2);

/*
 *
 * Methods to get attributes
 *
 */
    unsigned getDimension();

    unsigned long getQtyPoints();
    void setQtyPoints(unsigned long);

    int getLabelGT();
    void setLabelGT(int label);

    int getLabelgC();
    void setLabelgC(int label);

    void setCellId(int id);
    int getCellId();

    void setCenterMass(Point p);
    Point getCenterMass();

    void setAdjacent(vector<int>);
    vector<int> getAdjacent();

/*
 * Methods to deal with cells
 *
 */
    // Method to insert a new point in the cell
    bool insertPoint(Point coordPoint);
    void insertLabelGT(int informedGT);

    // Method to merge points in the cell
    bool mergePoints(Point cM, unsigned qP);

    // Method to return a vector with coordinates of the Cell
    vector<vector <double> > cubeCoord(int epslon);

private:
    unsigned dimen; // Dimension
    Point centerMass; // Coordinates cell's center of mass
    long qtyPoints; // Quantity of points

    int labelgC;    // Label gCluster
    int labelGT;    // Cell Ground Truth
    int cellId;     // Cell Identification gave by getCluster
    vector<vector <int> > mapGT; // A vector to map quantity of ground truth points inside the cell
    vector<int> adjacent;

    // recursive method to calculate cube Coordinates
    void calcCubeCoord(vector<vector <double> > &g, vector<double> v, int len, int start, int epsilon);
};

// Define de Edges of a graph of cells
class Edge
{
public:

    Cell from, to;
    double attractionForce;
};

// Return true if two cells are adjacents
bool areAdjacents(Cell *cell1, Cell *cell2);

// Return a int vector with cells' coordinates given a point.
vector<int> getCellCoord(Point p, int epsilon);

#endif
