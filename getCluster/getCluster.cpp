/*
 * This program perform gCluster Algorithm
 *
 * It just read a CSV file gathered from devices and generate a CSV file with cell configuration
 *
 *
 */
#include "gCluster.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include "point.h"
#include "cell.h"
#include "csv-reader.h"
#include "graphs.h"

#define PRINT_DEBUG 0

int main(int argc, char* argv[])
{
    ofstream resultFile;
    int opt;
    ifstream infile;

    int lineNo = 0;
    unsigned dimension;
    unsigned noFields;  // qty of fields
    vector<string> lineCSV, headerCSV, cellCSV;
    vector<Cell*> listCells; // Vector of Cells
    //vector<Edge> listEdges; // Vector of Edges

    // Parameters and theirs defaults
    string input;
    string inputDefault = "cells.csv";
    string outputResultCells;
    string outputResultCellsDefault = "result-gCluster.csv";
    unsigned epsilon;
    unsigned epsilonDefault = 10;
    unsigned minCells;
    unsigned minCellsDefault = 3; // Minimum number of cells
    double minForce;
    double minForceDefault = 150.;

    cout << endl << "gCluster algorithm - clustering module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/gCluster" << endl;
    cout << "===============================================================================" << endl;

    // Initialize options

    epsilon = epsilonDefault;
    input = inputDefault;
    outputResultCells = outputResultCellsDefault;
    minCells = minCellsDefault;
    minForce = minForceDefault;

    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "e:m:f:i:o:h")) != -1)
    {
        switch (opt)
        {
        case 'e':
            epsilon = atoi(optarg);
            if (epsilon > 10000)
            {
                cerr << "epsilon value invalid: " << epsilon << endl;
                cerr << "Must be less than or equal 10000" << endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            minCells = atoi(optarg);
            if (minCells > 10000)
            {
                cerr << "minCells value invalid: " << epsilon << endl;
                cerr << "Must be less than or equal 10000" << endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'f':
            minForce = stod(optarg);
            break;
        case 'i':
            input = optarg;
            break;
        case 'o':
            outputResultCells = optarg;
            break;
        case 'h':
        default: /* '?' */
            cout << "OVERVIEW: Clustering data using gCluster algorithm" << endl;
            cout << endl;
            cout << "USAGE: getCluster <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-h print this help" << endl;
            cout << "-e <value>\tEpsilon (defaut: " << epsilonDefault << ")" << endl;
            cout << "-m <value>\tMinimum Cells (default: " << minCellsDefault << ")" << endl;
            cout << "-f <value>\tMinimum Force (default: " << minForceDefault << ")" << endl;
            cout << "-i <file>\tinput file (default: " << inputDefault <<")" << endl;
            cout << "-o <file>\toutput file (" << outputResultCellsDefault << ")" << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << " Parameters" << endl;
    cout << "----------------------------" << endl;
    cout << "Epsilon     : " << epsilon << endl;
    cout << "Min Cells   : " << minCells << endl;
    cout << "Min Force   : " << minForce << endl;
    cout << "Input file  : " << input << endl;
    cout << "Output file : " << outputResultCells << endl;
    cout << endl <<"Running" << endl;
    cout << "----------------------------" << endl;

    infile.open(input.c_str());
    if (!infile)
    {
        cerr << "Error: Unable to open input file: "<< input << endl;
        exit(EXIT_FAILURE);
    }

    // Start to read CSV file
    int r = 0;

    if (!getCSVLine(infile, headerCSV))
    {
        cerr << "Error reader CSV File (Header) on the first line" << endl;
        exit(EXIT_FAILURE);
    }

    noFields = headerCSV.size(); // Define number of fields

    cout << "Reading CSV File header. Number of Fields:" << noFields << endl;

    for (unsigned i = 0; i < headerCSV.size(); i++)
        cout << (i?",": "") << headerCSV[i];

    cout << endl;

    dimension = headerCSV.size() - 3;

    cout << "CSV File - Dimension: " << dimension << endl;
    cout << "Fields used on clustering: ";

    unsigned dimPosition = (headerCSV.size()-(dimension));

    for (unsigned i = dimPosition; i < headerCSV.size(); i++)
        cout << headerCSV[i] << " ";
    cout << endl;

    cout << "CSV File - Reading Data" << endl;

    // Define max number of points to use as reference to force calculation
    unsigned maxPoints = 0;

    // Counter of points
    unsigned totPoints = 0;

    // Start reading file
    while (infile)
    {
        r = getCSVLine(infile, lineCSV);
        ++lineNo;
        if (!r)
        {
            cerr << "Error reading CSV File (Data) on Line " << lineNo + 2  << "\n";
            exit(EXIT_FAILURE);
        }
        if (lineCSV.size() != noFields)
        {
            cerr << "Error reading CSV File (Data) on Line " << lineNo + 2 << ": number of "
                 << "fields doesn't match" << endl;
            cerr << "\tNumber of Fields expected: " << noFields << endl;
            cerr << "\tNumber of Fields read    : " << lineCSV.size() << endl;

            exit(EXIT_FAILURE);
        }

        // Define a Cell pointer
        Cell* pNewCell;

        // variable to get center of mass's coordinates
        Point centerMass(dimension); // Center of Mass coordinates

        unsigned qtyPoints;

        // And initialize the pointer
        pNewCell = NULL;

        // Read the center of mass's coordinates from CSV line
        for (unsigned i = 0; i < dimension; i++)
            centerMass.coord[i] = stod(lineCSV[i+dimPosition]);

        vector<int> coordCell = getCellCoord(centerMass, epsilon); // Cells coordinates

#if PRINT_DEBUG

 cout << "Line " << lineNo << ":" << centerMass;
 cout << " Cell coord " << coordCell.size() << ": ";
 for (int a : coordCell)
    cout << a << ",";

 cout << endl;

#endif // PRINT_DEBUG


        // Read Qty points
        qtyPoints = stoi(lineCSV[2]);
        totPoints += qtyPoints;

        for (unsigned i=0; i<listCells.size(); i++) // Search cells
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
            for (unsigned i=0; i < dimension; i++)
                pNewCell->coord[i] = coordCell[i];
            listCells.push_back(pNewCell);
        }

        if (!pNewCell->mergePoints(centerMass, qtyPoints))
        {
            cerr << "Error on Merge Points on Cell - Line " << lineNo << endl;
            exit (EXIT_FAILURE);
        }

        if (pNewCell->getQtyPoints() > maxPoints)
            maxPoints = pNewCell->getQtyPoints();

        cout << flush <<  "Number of Lines: " << lineNo << " Number of Cells: " << listCells.size() << "\r";

        lineCSV.resize(0);
    }

    Point a(dimension);

    cout << "CSV File - Total of " << lineNo << " lines of data read" << endl;
    cout << "Number of Cells   : " << listCells.size() << endl;
    cout << "Number of Points  : " << totPoints << endl;
    cout << "Avg Points by Cell: " << (double)totPoints / (double)listCells.size() << endl;

    if (listCells.size() == 0)
    {
        cerr << "Error on algorithm: There is no cells" << endl;
        exit (EXIT_FAILURE);
    }

    // All cells were processed, now process the graph

    // Create a Graph with number of nodes = number of cells
    Graph g(listCells.size());

    // iterate all cells
    for (unsigned i = 0; i<(listCells.size()-1); i++)
    {
        vector<unsigned> nodesToBeConnected;
        for (unsigned j = i+1; j<listCells.size(); j++)
        {
            if (areAdjacents(listCells[i], listCells[j]))
            {
                // Calculate gravitational force
                double distance = listCells[i]->getCenterMass().dist(listCells[j]->getCenterMass());
                distance /= 1/(double)epsilon;
                // Gravitational force is relative Qty of Points[j] * relative Qty of Poinst[i] / distance ^ 2
                double relativeQtyI = (double)listCells[i]->getQtyPoints() / maxPoints;
                double relativeQtyJ = (double)listCells[j]->getQtyPoints() / maxPoints;
                double gravForce = (relativeQtyI)*(relativeQtyJ)/(distance*distance);
                // Test if need to create a edge between cells
                if (gravForce > minForce || minForce == 0)
                    nodesToBeConnected.push_back(j);
            }
        }

        // for this cell (index = i), include in the correspondent node (index = i)
        // an adjacent matrix, where there is all nodes connected to this cell
        // The last parameter is the weight of edge. It´s not being used at this implementation
        for (unsigned j = 0; j < nodesToBeConnected.size(); j++)
            g.addEdge(i, nodesToBeConnected[j], 1.); // Create an edge on Graph

    }

    // connected Components give labels for clusters and
    // for each node store its cluster on g.[<nodeIndex>].clusterIndex
    // Also, update an internal vector of graph that store the numbers of nodes each cluster has
    // This vector could be gathered by getCluster method
    g.connectedComponents(); // Verify the connected components of graph

    vector<vector <int> > clusters = g.getClusters();

    cout << endl
         << "\t--------------------" << endl
         << "\tCluster\tQty Cells"<< endl
         << "\t--------------------" << endl;

    unsigned totClusters = 0;
    unsigned labelCluster = 0;

    for (unsigned i = 0; i < clusters.size(); i++)
    {
        if (clusters[i][1] >= minCells)
        {
            clusters[i][0] = labelCluster++; // Indicate the number of cluster
            cout << "\t" << clusters[i][0] << "\t" << clusters[i][1] << endl;
            totClusters++;
        }
        else
        {
            clusters[i][0] = -1; // Cluster Label -1
        }
    }
    cout << "\t----------------------" << endl;
    cout << "\t Total Clusters: " << totClusters << endl;
    cout << "\t----------------------" << endl << endl;

    cout << "\t----------------------------" << endl;
    cout << "\t Creating result cells file " << outputResultCells << endl;
    cout << "\t----------------------------" << endl;

    // Open file Result
    resultFile.open(outputResultCells, std::ifstream::out);
    if (!resultFile.is_open())
    {
        cerr << "Could not open output file:" << outputResultCells << endl;
        exit (EXIT_FAILURE);
    }

    // Create header of file
    // Label of center of mass coordinates
    for (unsigned i = 0; i < dimension; i++)
        resultFile << "CM-" << i << ",";

    resultFile << "cell-id,qty-points,";

    resultFile << "connected-cells,gCluster-label";
    resultFile << endl;

    // iterate all cells

    for (unsigned i = 0; i<(listCells.size()); i++)
    {
        Point cm = listCells[i]->getCenterMass();

        // Write center of mass coordinates on file
        for (unsigned j = 0; j < cm.coord.size(); j++)
            resultFile << cm.coord[j] << ",";

        // cell-id, quantity of points,
        resultFile << i << "," << listCells[i]->getQtyPoints() << ",";

        // connected-cells, gCluster-Label
        resultFile  <<  clusters[g.getClusterIndex(i)][1] << ","
                    << clusters[g.getClusterIndex(i)][0] ;

        // Adjacent cells

        for (unsigned a : g.getAdjacents(i))
            resultFile << "," << a ;

        resultFile << endl;

    }

    resultFile.close();
    infile.close();

    exit(EXIT_SUCCESS);

}
