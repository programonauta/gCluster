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

  Cell newCell = Cell(2);

  if (!getCSVLine(infile, headerCSV))
  {
    cout << "Error on CSV Header" << endl;
    return 1;
  }

  dimension = headerCSV.size();

  cout << "CSV File header read. Dimension: " << dimension << endl;

  Point sample(dimension);

  sample.coord[0] = 2.323;
  sample.coord[1] = -5.332;
  sample.coord[2] = 0.393;

  cout << "Point " << sample << endl;

  while (infile)
  {
    r = getCSVLine(infile, lineCSV);
    cout << "Line number: " << ++lineNo << " Number of fields: " << lineCSV.size() << "\n";
    for (int i = 0; i < lineCSV.size(); i++)
    {
      cout << "   " << lineCSV[i] << " (is ";
      if (!isDouble(lineCSV[i].c_str()))
        cout << "not ";
      cout << "double) ,";
      if (isDouble(lineCSV[i].c_str())) cout << "|" << stod(lineCSV[i]) << "|, ";
    }
    cout << "\n";
    if (!r)
    {
      cout << "Error - line " << lineNo << "\n\n";
      return 0;
    }
    lineCSV.resize(0);
    cout << "\n";
  }

  cout << "Everything OK!\n**==**==**==**==**==**==**==\n\n";

}
