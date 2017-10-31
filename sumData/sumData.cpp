#include <iostream>
#include <math.h>
#include <unistd.h>
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
    int opt;
    string input;
    string outputCells;
    string outputPoints;
    string value;
    int lineNo = 0;
    unsigned dimension;
    int epsilon;
    unsigned noFields;  // number of fields
    vector<string> lineCSV, headerCSV, clusterFields;
    vector<Cell*> listCells; // Vector of Cells
    vector<double> maxValue, minValue;

    FILE* pFileCells;
    FILE* pFilePoints;

    cout << "Grid Clustering algorithm - Summarization Phase" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
    cout << "===============================================================================\n" << endl;

    // Default values of arguments
    unsigned epsilonDefault = 10;
    string inputDefault = "input.csv";
    string outputCellsDefault = "cells.csv";
    string outputPointsDefault = "points.csv";

    epsilon = epsilonDefault;
    input = inputDefault;
    outputCells = outputCellsDefault;
    outputPoints = outputPointsDefault;

    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "e:i:c:p:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input = optarg;
            break;
        case 'c':
            outputCells = optarg;
            break;
        case 'p':
            outputPoints = optarg;
            break;
        case 'e':
            epsilon = atoi(optarg);
            break;
        case 'h':
        default: /* '?' */
            cout << "OVERVIEW: Clustering data using grid-based algorithm - Summarization Phase" << endl;
            cout << "          The program output a csv file with Cells data " << endl;
            cout << endl;
            cout << "USAGE: sumData <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-e <epsilon> (default: " << epsilonDefault << ")" << endl;
            cout << "-i <file>\tinput file (default: " << inputDefault <<")" << endl;
            cout << "-c <file>\tcells output file (default: " << outputCellsDefault << ")" << endl;
            cout << "-p <file>\tpoints output file (default: " << outputPointsDefault <<")" << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << "Parameters " << endl;
    cout << "-----------" << endl;
    cout << "epsilon    : " << epsilon << endl;
    cout << "Input file : " << input << endl;
    cout << "Cells file : " << outputCells << endl;
    cout << "Points file: " << outputPoints << endl;
    cout << endl <<"Running" << endl;
    cout << "-------" << endl;

    infile.open(input.c_str());
    if (!infile)
    {
        cout << "Error: Unable to open input file."<< endl;
        exit (EXIT_FAILURE);
    }

    pFileCells = fopen(outputCells.c_str(), "w");

    if (pFileCells == NULL)
    {
        cout << "Error: Unable do open cells output file." << endl;
        infile.close();
        exit (EXIT_FAILURE);
    }

    pFilePoints = fopen(outputPoints.c_str(), "w");

    if (pFilePoints == NULL)
    {
        cout << "Error: Unable do open points output file." << endl;
        infile.close();
        fclose(pFileCells);
        exit (EXIT_FAILURE);
    }

    if (!getCSVLine(infile, headerCSV))
    {
        cout << "Error reader CSV File (Header) on the first line" << endl;
        infile.close();
        fclose(pFileCells);
        fclose(pFilePoints);
        exit (EXIT_FAILURE);
    }

    noFields = headerCSV.size(); // Define number of fields

    cout << "Reading CSV File header. Number of Fields:" << noFields << endl;

    for (unsigned i = 0; i < headerCSV.size(); i++)
        cout << (i?",": "") << headerCSV[i];

    cout << endl;

    if (!getCSVLine(infile, clusterFields))
    {
        cout << "Error reading CSV File (Header) on Clusters Line" << endl;
        infile.close();
        fclose(pFileCells);
        fclose(pFilePoints);
        exit(EXIT_FAILURE);
    }

    if (clusterFields.size() != noFields)
    {
        cout << "Error reading CSV File (Header): Clusters Line (line 2)- "
             << "number of fields doesn't "
             << "match. Read " << clusterFields.size() << " Expected: " << noFields
             << " fields " << endl;
        exit(EXIT_FAILURE);
    }

    dimension = 0;
    for (unsigned i = 0; i < clusterFields.size(); i++)
    {
        if (clusterFields[i] != "C" && clusterFields[i] != "N")
        {
            cout << "Error reading CSV File (Header): Clusters Line (line 2)"
                 << "- Invalid Parameter"
                 << " on field " << i+1 << " \"" << clusterFields[i] << "\""
                 << " Must be \"C\" (C)luster Field " <<
                 " or \"N\" (N)ot a Cluster Field " << endl;
            exit(EXIT_FAILURE);
        }
        if (clusterFields[i] == "C")
            dimension++;
    }

    if (dimension == 0)
    {
        cout << "Error reading CSV File (Header): Cluster Line (line 2) "
             << "- There is not clusters fields "
             << endl;
        exit(EXIT_FAILURE);
    }

    cout << "CSV File - Dimension: " << dimension << endl;

    cout << "Fields used on clustering: ";

    for (unsigned i = 0; i < clusterFields.size(); i++)
    {
        if (clusterFields[i] == "C")
            cout << headerCSV[i] << " ";
    }

    cout << endl;

    if (!getCSVLine(infile, lineCSV))
    {
        cout << "Error reading CSV File (Header) on Max line (line 3)" << endl;
        exit(EXIT_FAILURE);
    }

    if (lineCSV.size() != noFields)  // Test if line of max values has the same number of fields than header
    {
        cout << "Error reading CSV File (Header): Max line (line 3)"
             << "- number of fields doesn't match. "
             << " Read " << lineCSV.size() << " Expected: " << noFields <<  endl ;
        exit(EXIT_FAILURE);
    }

    maxValue.resize(noFields);
    minValue.resize(noFields);

    for (unsigned i = 0; i < noFields; i++)
    {
        if (!isDouble(lineCSV[i].c_str()))
        {
            cout << "Error reading CSV File (Header): Max line (line 3)"
                 << " Field number " << i+1 << ", "
                 << lineCSV[i] << " is not double" << endl;
            exit(EXIT_FAILURE);
        }
        maxValue[i] = stod(lineCSV[i]);
    }

    cout << "CSV File - Max Line read - OK" << endl;
    lineCSV.resize(0);
    lineCSV.shrink_to_fit();

    if (!getCSVLine(infile, lineCSV))
    {
        cout << "Error reading CSV File (Header) on Min line (line 4)" << endl;
        exit(EXIT_FAILURE);
    }

    if (lineCSV.size() != noFields) // Test if line of min values has the same dimension than header
    {
        cout << "Error reading CSV File (Header): Min line (line 4) - number of fields "
             << "doesn't match. Read " << lineCSV.size() << " Expected: " << noFields << endl ;
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < noFields; i++)
    {
        if (!isDouble(lineCSV[i].c_str()))
        {
            cout << "Error reading CSV File (Header): Min line (line 4)"
                 << " Field number " << i+1 << ", "
                 << lineCSV[i] << " is not double" << endl;
            exit(EXIT_FAILURE);
        }
        minValue[i] = stod(lineCSV[i]);

        if (minValue[i] >= maxValue[i]) // Test if min value is greater or equal than max value
        {
            cout << "Error reading CSV File (Header) - line 4: Min value gretaer or equal"
                 << "than Max value for dimension "
                 << i+1 << ": " << minValue[i] << " isn't less than " << maxValue[i] << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << "CSV File - Min Line read - OK" << endl;
    lineCSV.resize(0);
    lineCSV.shrink_to_fit();

    cout << "CSV File - Reading Data" << endl;

    // Start read data (5th line)

    vector <Point> pointsSample;

    // Now start reading actual data
    while (infile)
    {
        int r = getCSVLine(infile, lineCSV);
        ++lineNo;
        if (!r)
        {
            cout << "Error reading CSV File (Data) on Line " << lineNo + 4  << "\n";
            exit(EXIT_FAILURE);
        }
        if (lineCSV.size() != noFields)
        {
            cout << "Error reading CSV File (Data) on Line " << lineNo + 4 << ": number of "
                 << "fields doesn't match" << endl;
            exit(EXIT_FAILURE);
        }

        // Create a variable Sample
        Point sample(dimension);

        // Coordinate index
        int posSample = 0;

        for (unsigned i = 0; i < lineCSV.size(); i++)
        {
            if (clusterFields[i] != "C")
                continue;
            if (!isDouble(lineCSV[i].c_str()))
            {
                cout << "Error reading CSV File (Data) on Line: " << lineNo + 4 << ", Field: "
                     << i+1 << " \""
                     << lineCSV[i] << "\" is not double" << endl;
                exit(EXIT_FAILURE);
            }
            // Update the coordinates with normalized values
            sample.coord[posSample++] =
                ((stod(lineCSV[i])-minValue[i])/(maxValue[i]-minValue[i]));
        }

        // Insert new vector on pointsSample list
        pointsSample.push_back(sample);

        // Here we know the CSV line is correct, so let's create a sample normalized
        Cell* pNewCell;

        vector<int> coordCell; // Cells coordinates

        coordCell.resize(dimension);

        // First point to NULL
        pNewCell = NULL;

        // Determine the cell's coordinates

        for (unsigned i = 0; i < dimension; i++)
            coordCell[i] = (int)(sample.coord[i] * epsilon);

        for (unsigned i=0; i<listCells.size(); i++) // Search cells
        {
            // Compare listCells and current cell
            if (coordCell == listCells[i]->coord) // all coordinates are equal
            {
                // Point New Cell to Existed cell
                pNewCell = listCells[i];
                break;
            }
        }

        if (pNewCell == NULL) // Create a New Cell
        {
            pNewCell = new Cell(dimension);
            for (unsigned i=0; i < dimension; i++)
                pNewCell->coord[i] = coordCell[i];
            listCells.push_back(pNewCell);
        }

        if (!pNewCell->insertPoint(sample))
        {
            cout << "Error on insert Point on Cell - Line " << lineNo << endl;
            exit(EXIT_FAILURE);
        }

        cout << flush <<  "Number of Lines: " << lineNo << " Number of Cells: " << listCells.size() << "\r";

        lineCSV.resize(0);
    } // End of CSV File


    Point a(dimension);
    cout << "CSV File - Total of " << lineNo << " lines of data read" << endl;
    cout << "Size of each point: " << sizeof(a.coord[0]) << endl;
    cout << "Size of vector: " << sizeof(a.coord) << " dimension: " << dimension <<  endl;
    cout << "Number of Cells: " << listCells.size() << endl;
    cout << "Size of each cell: " << sizeof(*listCells[0]) << endl;
    cout << "Size of Cells: " << sizeof(*listCells[0])*listCells.size() << endl;
    cout << "Size of Points: " << sizeof(a) * lineNo << endl;

    if (listCells.size() == 0)
    {
        cout << "Error on algorithm: There is no cells" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "------------------------------" << endl;
    cout << "Creating Cells and Points file" << endl;
    cout << "------------------------------" << endl ;

    // ----------------------------
    // points.csv file
    // ----------------------------
    // Labels of dimensions
    bool first=true;
    for (unsigned i=0; i < headerCSV.size(); i++)
    {
        if (clusterFields[i]=="C")
        {
            if (!first)
                fprintf(pFilePoints, ",");
            fprintf(pFilePoints, "%s", headerCSV[i].c_str());
            first = false;
        }
    }

    fprintf(pFilePoints, "\n");

    for (unsigned i = 0; i < pointsSample.size(); i++)
    {
        fprintf(pFilePoints, "%f", pointsSample[i].coord[0]);
        for(unsigned j = 1; j < pointsSample[i].coord.size(); j++)
            fprintf(pFilePoints, ",%f", pointsSample[i].coord[j]);
        fprintf(pFilePoints, "\n");
    }

    // -----------------------
    // cell.csv file
    // -----------------------


    // Create header of cell.csv file

    fprintf(pFileCells, "id,Label,Number_Points");

    // Labels from coordinates
    for (unsigned i=0; i < dimension; i++)
        fprintf(pFileCells, ",Coord-%d", i);

    // Labels of dimensions
    for (unsigned i=0; i < headerCSV.size(); i++)
    {
        if (clusterFields[i]=="C")
            fprintf(pFileCells, ",%s", headerCSV[i].c_str());
    }

    fprintf(pFileCells, "\n");

    // iterate all cells
    for (unsigned i = 0; i<listCells.size(); i++)
    {
        fprintf(pFileCells, "%d,%d", i, i);

        // Number of points
        fprintf(pFileCells, ",%ld", listCells[i]->getQtyPoints());

        // Cell's coordinates
        for (unsigned j = 0; j < listCells[i]->coord.size(); j++)
            fprintf(pFileCells, ",%d", listCells[i]->coord[j]);

        Point CM = listCells[i]->getCenterMass(); // Center of mass
        // Center of mass coordinates
        for (unsigned j = 0; j < listCells[i]->coord.size(); j++)
            fprintf(pFileCells, ",%f", CM.coord[j]);

        fprintf(pFileCells, "\n");

        // if we are at last cell, there is not a next cell, so we can exit the loop

    }

    infile.close();
    fclose(pFileCells);
    fclose(pFilePoints);
}
