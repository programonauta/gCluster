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
    vector<Cell*> adjCells;  // Adjacents cells

    // Methods
    
    // Constructor
    Cell(int dimen = 2);

    // Method to get dimension of cell
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

#endif
