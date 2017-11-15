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
    this->labelGT = -1;
    this->labelgC = -1;
    this->mapGT.resize(0);

    coord.resize(dimen);
    Cell::centerMass.coord.resize(dimen);

    for (int i=0; i < dimen; i++)
    {
        coord[i] = 0;
        Cell::centerMass.coord[i] = 0;
    }
}

unsigned Cell::getDimension()
{
    return this->dimen;
}

Point Cell::getCenterMass()
{
    return this->centerMass;
}

unsigned long Cell::getQtyPoints()
{
    return this->qtyPoints;
}

void Cell::setQtyPoints(unsigned long qty)
{
    this->qtyPoints = qty;
    return;
}

void Cell::setLabelGT(int label)
{
    this->labelGT = label;
    return;
}

void Cell::setLabelgC(int label)
{
    this->labelgC = label;
    return;
}

void Cell::setCellId(int id)
{
    this->cellId = id;
    return;
}

void Cell::setCenterMass(Point p)
{
    this->centerMass = p;
    return;
}

void Cell::setAdjacent(vector<int> v)
{
    this->adjacent = v;
    return;
}

int Cell::getLabelGT()
{
    return this->labelGT;
}

int Cell::getLabelgC()
{
    return this->labelgC;
}

int Cell::getCellId()
{
    return this->cellId;
}

vector<int> Cell::getAdjacent()
{
    return this->adjacent;
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
    for (unsigned i=0; i<this->dimen; i++)
        this->centerMass.coord[i] =
            ((this->centerMass.coord[i] * (this->qtyPoints-1))+ coordPoint.coord[i]) /
            this->qtyPoints;

    return true;
}

/*
 * Insert a the GT label of a point that belongs to cell
 *
 * At the and the quantity of labels must be the same of quantity of points
 */
void Cell::insertLabelGT(int informedGT)
{
    // Try to find the label of GT on the Map

    int idxGT = -1;
    for (unsigned i = 0; i < mapGT.size(); i++)
    {
        if (mapGT[i][0] == informedGT)
        {
            idxGT = i;
            break;
        }
    }
    if (idxGT >= 0) // Found GT
    {
        mapGT[idxGT][1]++; // Increase the number of occurrences of this GT label
;    }
    else
    {
        vector<int> matAux = {informedGT, 1}; // Create a position on Matrix
        mapGT.push_back(matAux);
    }

    // Find out the Ground True with max occurrence on cell

    unsigned maxOcc = 0;
    int labelFound = -1;
    for (vector<int> a : mapGT)
    {
        if (a[1] > maxOcc)
        {
            maxOcc = a[1];
            labelFound = a[0];
        }
    }

    this->labelGT = labelFound;

}

bool Cell::mergePoints(Point cM, unsigned qP)
{
    // The dimension must match

    if (cM.coord.size() != this->dimen)
        return false;

    // if Quantity of Points is zero do nothing
    if (qP == 0)
        return true;

    // Calculate the new center of mass
    for (unsigned i=0; i<this->dimen; i++)
        this->centerMass.coord[i] =
            ((this->centerMass.coord[i] * (this->qtyPoints))+ (cM.coord[i]) * qP) /
            (this->qtyPoints + qP);

    this->qtyPoints += qP;

    return true;
}

bool areAdjacents(Cell *cell1, Cell *cell2)
{

    if (cell1->coord == cell2->coord) // The two cells are the same
        return false;
    for (unsigned i=0; i<cell1->coord.size(); i++)
        if (abs(cell1->coord[i]-cell2->coord[i])>1)
            return false;

    return true;

}

// Recursive Method to calculate the coordinates of hypercube (or cell)

void Cell::calcCubeCoord(vector<vector <double>> &g, vector<double> v, int len, int start, int epsilon)
{
    if (len == 0)
    {
        g.push_back(v);
        return;
    }

    for (int i = 0; i<2; i++)
    {
        int d = v.size() - len; // d = dimension
        v[d] = (double)this->coord[d]/epsilon + i * (1./epsilon);
        this->calcCubeCoord(g, v, len-1, i, epsilon);
    }

    return;
}

vector<vector <double> > Cell::cubeCoord(int epsilon)
{
    vector <double> v(this->dimen);
    vector <vector <double> > g;

    calcCubeCoord(g, v, this->dimen, 0, epsilon);

    return g;
}

// Return the cell coordinate given a point and the Epsilon
vector<int> getCellCoord(Point p, int epsilon)
{
    vector<int> v(p.coord.size());

    unsigned dimension = p.coord.size();
    for (unsigned i = 0; i < dimension; i++)
        v[i] = (int)(p.coord[i] * epsilon);

    return v;
}

void calcCellGT()
{
}

