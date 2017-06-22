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

    // Coordinates of cel
    vector<int> coord;       // Cell coordinates
    // Methods
    
    // Constructor
    Cell(int dimen = 2);

    // Methods to get private variables 
    int getDimension();

    Point getCenterMass();

    long getQtyPoints();

    // Method to insert a new point in the cell
    bool insertPoint(Point coordPoint);

  private:
    // Dimension
    int dimen;
    // Coordinates cell's center of mass 
    Point centerMass;
    // Quantity of points
    long qtyPoints;

};

// Define de Edges of a graph of cells
class Edge
{
  public:

    Cell from, to;
    
    double attractionForce;
};

bool areAdjacents(Cell *cell1, Cell *cell2);

#endif
