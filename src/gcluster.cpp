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

char* getCmdOption(char ** begin, char ** end, const std::string & option) 
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) 
{
  ifstream infile;
  string input = "../data/input.csv"; 
  string outputNodes = "nodes.txt";
  string outputEdges = "edges.txt";
  string value;
  int lineNo = 0;
  int fieldCount = 0;
  int dimension;
  size_t lfFound;
  double dataVar = 0.;
  vector<string> lineCSV, headerCSV;
  vector<Cell*> listCells; // Vector of Cells
  vector<Edge> listEdges; // Vector of Edges
  vector<Cell*> roots;    // Vector of cluster's roots
  vector<double> maxValue, minValue;

  int epsilon = 10; // Epsilon: number of divisions

  cout << "Grid Clustering algorithm" << endl;
  cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
  cout << "===============================================================================\n" << endl;

// Verify command line options
//   
// Show help message if -h or there is no command options
  if (argc <= 1 || cmdOptionExists(argv, argv+argc, "-h")) {
    cout << "OVERVIEW: Clustering data using grid-based algorithm" << endl;
    cout << "          The program output node and edges file in a format " << 
      "to be imported by Gephi software" << endl;
    cout << endl;
    cout << "USAGE: gcluster <options>" << endl;
    cout << endl;
    cout << "OPTIONS: " << endl;
    cout << "-e <value>\tvalue of epsilon (default: 10)" << endl;
    cout << "-if <file>\tinput file (default: ../data/input.csv)" << endl;
    cout << "-nf <file>\tnodes output file (default: nodes.txt)" << endl;
    cout << "-ef <file>\tedge output file (default: output.txt)" << endl;
    return 0;
  }

  // Verify epsilon option
  if (cmdOptionExists(argv, argv+argc, "-e"))
    epsilon = atoi(getCmdOption(argv, argv+argc, "-e"));

  if (epsilon < 1)
    epsilon = 10;

  cout << "Parameters" << endl;
  cout << "----------" << endl;

  cout << "epsilon: " << epsilon << endl;

  if (cmdOptionExists(argv, argv+argc, "-if")) 
  {
    input = string(getCmdOption(argv, argv+argc, "-if"));

    if (input == "")
      input = "../data/input.txt";
  }

  cout << "Input file: " << input << endl;

  if (cmdOptionExists(argv, argv+argc, "-nf")) 
  {
    outputNodes = string(getCmdOption(argv, argv+argc, "-nf"));

    if (input == "")
      outputNodes = "nodes.txt";
  }

  cout << "Nodes file: " << outputNodes << endl;

  if (cmdOptionExists(argv, argv+argc, "-ef")) 
  {
    outputEdges = string(getCmdOption(argv, argv+argc, "-ef"));

    if (input == "")
      outputEdges = "edges.txt";
  }

  cout << "Edges file: " << outputEdges << endl;

  cout << endl <<"Running" << endl;
  cout << "-------" << endl;

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
      // Compare listCells and current cell
      if (coordCell == listCells[i]->coord) // all coordinates are equal
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

  Point a(dimension);

  cout << "CSV File - Total of " << lineNo << " lines of data read" << endl;
  cout << "Number of Cells: " << listCells.size() << endl;
  cout << "Size of Cells: " << sizeof(*listCells[0])*listCells.size() <<
    " Size of Points: " << sizeof(a) * lineNo << endl;

  if (listCells.size() == 0) 
  {
    cout << "Error on algorithm: There is no cells" << endl;
    return 1;
  }

  cout << "-------------------" << endl;
  cout << "Starting clustering" << endl;
  cout << "-------------------" << endl ;

  // Put the first cell as root;

  roots.push_back(listCells[0]);

  // iterate all cells until penultimate cell
  for (int i = 0; i<listCells.size() - 1; i++)
  {
    // test next cell until end
    bool hasAdjacent = false;
    for (int j = i+1; j<listCells.size(); j++)
    {
      if (areAdjacents(listCells[i], listCells[j]))
      {
        cout << "Cells " << i << " and " << j << " are adjacents\n";
        hasAdjacent = true;
      }
    }
  }

}
