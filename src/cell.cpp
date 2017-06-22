//
// Cell Class
//


#include "cell.h"

// Constructor
Cell::Cell(int dimen) 
{
  // Dimension must be greater then 2
  if (dimen < 2) 
    dimen = 2;

  this->dimen = dimen;
  this->qtyPoints = 0;

  coord.resize(dimen);
  Cell::centerMass.coord.resize(dimen);
  
  for (int i=0; i < dimen; i++)
  {
    coord[i] = 0;
    Cell::centerMass.coord[i] = 0;
  }
}

int Cell::getDimension()
{
  return this->dimen;
}

Point Cell::getCenterMass()
{
  return this->centerMass;
}

long Cell::getQtyPoints()
{
  return this->qtyPoints;
}
//
// Include a new point in the cell
//
// Just increase the number of points in the cell
// and update the center of mass
//
// Return false if the coordPoint dimension doesn't match with cell dimension
//
// Return true if matchs
//
bool Cell::insertPoint(Point coordPoint)
{
  this->qtyPoints++;

  // The dimension must match

  if (coordPoint.coord.size() != this->dimen)
    return false;

  // Calculate the new center of mass
  for (int i=0; i<this->dimen; i++)
    this->centerMass.coord[i] = (this->centerMass.coord[i] + coordPoint.coord[i]) / this->qtyPoints;

  return true;
}

bool areAdjacents(Cell *cell1, Cell *cell2)
{

  if (cell1->coord == cell2->coord) // The two cells are the same
    return false;
  for (int i=0; i<cell1->coord.size(); i++)
    if (abs(cell1->coord[i]-cell2->coord[i])>1)
      return false;

  return true;

}
