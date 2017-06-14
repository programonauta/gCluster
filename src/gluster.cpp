// 
// Read file csv
//

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
  string input = "input.csv";
  string value;
  int lineNo = 0;
  int fieldCount = 0;
  size_t lfFound;
  double dataVar = 0.;
  vector<string> lineCSV;

  infile.open(input.c_str());
  if (!infile)
  {
    cout << "Unable to open input."<< endl;
    return 1;
  }

  int r = 0;

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
