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
    vector<int> coord;
    vector<Cell*> adjCells;

    // Methods
    
    // Constructor
    Cell(int dimen = 2);


  private:
    // Dimension
    int dimen;
    // Coordinates cell's center of mass 
    Point centerMass;
    // Quantity of points
    long qtyPoints;

};

#endif
