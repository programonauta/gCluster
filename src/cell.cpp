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

  coord.resize(dimen);
  for (int i=0; i < dimen; i++)
    coord[i] = 0;
}

