// 
// Read file csv
//

#include "cell.h"
#include "csv-reader.h"


bool isDouble(const char *str)
{
  char* endptr = 0;
  strtod(str, &endptr);

  return !((endptr == str));
}

int main(int argc, char* argv[]) 
{
  ifstream infile;
  string input = "../data/input.csv";
  string value;
  int lineNo = 0;
  int fieldCount = 0;
  int dimension;
  size_t lfFound;
  double dataVar = 0.;
  vector<string> lineCSV, headerCSV;
  vector<Cell*> listCells; // Vector of Cells
  vector<double> maxValue, minValue;

  int epsilon = 10; // Epsilon: number of divisions

  cout << "Grid Clustering algorithm" << endl;
  cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
  cout << "===============================================================================\n" << endl;

  infile.open(input.c_str());
  if (!infile)
  {
    cout << "Error: Unable to open input."<< endl;
    return 1;
  }

  int r = 0;

  if (!getCSVLine(infile, headerCSV))
  {
    cout << "Error reader CSV File (Header) on the first line" << endl;
    return 1;
  }

  dimension = headerCSV.size();

  cout << "Reading CSV File header. Dimensions:" << dimension << endl;

  for (int i = 0; i < headerCSV.size(); i++)
    cout << (i?",": "") << headerCSV[i];

  cout << endl;

  if (!getCSVLine(infile, lineCSV))
  {
    cout << "Error reading CSV File (Header) on Max line" << endl;
    return 1;
  }

  if (lineCSV.size() != dimension)  // Test if line of max values has the same dimension than header
  {
    cout << "Error reading CSV File (Header): Max line - dimension doesn't match. Read "  
      << lineCSV.size() << " Expected: " << dimension << " dimensions"  << endl ; 
    return 1;
  }

  maxValue.resize(dimension);
  minValue.resize(dimension);

  for (int i = 0; i < dimension; i++)
  {
    if (!isDouble(lineCSV[i].c_str()))
    { 
      cout << "Error reading CSV File (Header): Max line Field number " << i+1 << ", " 
        << lineCSV[i] << " is not double" << endl;
      return 1;
    }
    maxValue[i] = stod(lineCSV[i]);
  }

  cout << "CSV File - Max Line read - OK" << endl;
  lineCSV.resize(0);
  lineCSV.shrink_to_fit();

  if (!getCSVLine(infile, lineCSV))
  {
    cout << "Error reading CSV File (Header) on Min line" << endl;
    return 1;
  }

  if (lineCSV.size() != dimension)  // Test if line of min values has the same dimension than header
  {
    cout << "Error reading CSV File (Header): Min line - dimension doesn't match. Read " << lineCSV.size() 
      << " Expected: " << dimension << " dimensions"  << endl ; 
    return 1;
  }

  for (int i = 0; i < dimension; i++)
  {
    if (!isDouble(lineCSV[i].c_str()))
    { 
      cout << "Error reading CSV File (Header): Min line Field number " << i+1 << ", " 
        << lineCSV[i] << " is not double" << endl;
      return 1;
    }
    minValue[i] = stod(lineCSV[i]);

    if (minValue[i] >= maxValue[i]) // Test if min value is greater or equal than max value
    {
      cout << "Error reading CSV File (Header): Min value gretaer or equal than Max value for dimension " 
        << i+1 << ": " << minValue[i] << " isn't less than " << maxValue[i] << endl;
      return 1;
    }
  }

  cout << "CSV File - Min Line read - OK" << endl;
  lineCSV.resize(0);
  lineCSV.shrink_to_fit();

  cout << "CSV File - Reading Data" << endl;

  // Start read data (3rd line)

  while (infile)
  {
    r = getCSVLine(infile, lineCSV);
    ++lineNo;
    if (!r)
    {
      cout << "Error reading CSV File (Data) on Line " << lineNo << "\n";
      return 1;
    }
    if (lineCSV.size() != dimension)
    {
      cout << "Error reading CSV File (Data) on Line " << lineNo << ": dimension doesn't match" << endl;
      return 1;
    }

    Point sample(dimension);

    for (int i = 0; i < lineCSV.size(); i++)
    {
      if (!isDouble(lineCSV[i].c_str()))
      {
        cout << "Error reading CSV File (Data) on Line: " << lineNo << ", Field: " << i+1 << " (" 
          << lineCSV[i] << ") is not double" << endl;
        return 1;
      }
      // Update the coordinates with normalized values
      sample.coord[i] = ((stod(lineCSV[i])-minValue[i])/(maxValue[i]-minValue[i]));
    }

    // Here we know the CSV line is correct, so let's create a sample normalized

    Cell* pNewCell;

    vector<int> coordCell; // Cells coordinates

    coordCell.resize(dimension);

    pNewCell = NULL;

    // Determine the cell's coordinates

    for (int i = 0; i < dimension; i++)
      coordCell[i] = (int)(sample.coord[i] * epsilon); 

    for (int i=0; i<listCells.size(); i++) // Search cells
    {
      bool foundCell = true;
      // Start the comparison btw listCells and current cell
      for (int j=0; j<dimension; j++)
      {
        if (listCells[i]->coord[j] != coordCell[j]) // At least one dimension is different
        {
          foundCell = false;
          break;
        }
      }
      if (foundCell) // all coordinates are equal
      {
        pNewCell = listCells[i];
        break;
      }
    }

    if (pNewCell == NULL) // Create a New Cell
    {
      pNewCell = new Cell(dimension); 
      for (int i=0; i < dimension; i++)
        pNewCell->coord[i] = coordCell[i];
      listCells.push_back(pNewCell);
    }


    if (!pNewCell->insertPoint(sample))
    {
      cout << "Error on insert Point on Cell - Line " << lineNo << endl;
      return 1;
    }


    lineCSV.resize(0);
  }

  cout << "CSV File - Total of " << lineNo << " lines of data read" << endl;
  cout << "Number of Cells: " << listCells.size() << endl;
  cout << "Size of Cells: " << sizeof(listCells) << endl;

}
