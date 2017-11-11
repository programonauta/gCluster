#include "drawCluster.h"

string strSVGRect(string opacity, float graphMult, unsigned epsilon, string color, int x, int y)
{
    stringstream svgRect;

    double xSVG = graphMult * (double)x/epsilon + graphMult*.1;
    double ySVG = graphMult*1.1 - graphMult * ((double)y/epsilon + (1./epsilon));
    svgRect << "<rect style=\"opacity:"<<opacity<<";fill:"
            << color
            << ";stroke:none\" "
            << "width=\"" << graphMult * 1/(float)epsilon << "cm\" "
            << "height=\"" << graphMult * 1/(float)epsilon << "cm\" "
            << "x=\"" << xSVG << "cm\" "
            << "y=\"" << ySVG << "cm\" />"
            << endl;
    return svgRect.str();
}

string strSVGTextCell(float graphMult, unsigned epsilon, int x, int y,
                      unsigned cellID, int cluster, int cellGT)
{
    stringstream svgText;

    double xSVG = graphMult * (double)x/epsilon + graphMult*.1;
    double ySVG =  graphMult*1.1 - graphMult * ((double)(y-1)/epsilon + (1./epsilon));

    svgText << "<text "
            << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.55)<< "cm\" "
            << "font-style=\"normal\" "
            << "font-family=\"Times New Roman\" "
            << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
            << "stroke=\"none\" fill=\"black\" > "
            << "Cell:" << cellID
            << "</text>" << endl;

    svgText << "<text "
            << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.30)<< "cm\" "
            << "font-style=\"normal\" "
            << "font-family=\"Times New Roman\" "
            << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
            << "stroke=\"none\" fill=\"black\" > "
            << "Cl:" << cluster
            << "</text>" << endl;

    svgText << "<text "
            << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "font-style=\"normal\" "
            << "font-family=\"Times New Roman\" "
            << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
            << "stroke=\"none\" fill=\"black\" > "
            << "GT:" << cellGT
            << "</text>" << endl;
    return svgText.str();
}

string strSVGEdge(float graphMult, string color, double x1, double y1, double x2, double y2)
{
    stringstream svgEdge;
    svgEdge << "<line stroke=\"";
    svgEdge << color << "\" ";
    svgEdge << "x1=\""
            << (float)(x1*graphMult)+graphMult*.1
            <<"cm\" " << "y1=\""
            << graphMult*1.1 -
            (float)(y1*graphMult)
            << "cm\" " << "x2=\""
            << (float)(x2*graphMult)+graphMult*.1
            << "cm\" " << "y2=\""
            << graphMult*1.1 -
            (float)(y2*graphMult)
            << "cm\" />" << endl;
    return svgEdge.str();
}

int main(int argc, char* argv[])
{
    ifstream infile;
    ofstream svgFile;
    int opt;
    ifstream inPoint;
    string value;
    int lineNo = 0;
    unsigned dimension;
    unsigned noFields;  // number of fields
    vector<string> lineCSV, headerCSV, cellCSV;
    vector<Cell*> listCells; // Vector of Cells
    vector<Edge> listEdges; // Vector of Edges

    // Parameters and theirs defaults
    string input;
    string inputDefault = "cells.csv";
    string inputPoints;
    string inputPointsDefault = "";
    string outputSVG;
    string outputSVGDefault = "graph.svg";
    string outputResultPoints;
    string outputMapPoints;
    string outputResultPointsDefault = "resultPoints.csv";
    string outputResultCells;
    string outputResultCellsDefault = "resultCells.csv";
    string outputMapCells;
    string outputMapDirectory = ".";
    unsigned epsilon;
    unsigned epsilonDefault = 10;
    unsigned minCells;
    unsigned minCellsDefault = 3; // Minimum number of cells
    double minForce;
    double minForceDefault = 150.;

    cout << "Grid Clustering algorithm - getCluster module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
    cout << "===============================================================================\n" << endl;

    // Initialize draw options
    bool drawRects = true;
    bool drawEdges = true;
    bool drawPoints = false;
    bool drawNumbers = false;
    string prefix = "";

    // Initialize options

    epsilon = epsilonDefault;
    input = inputDefault;
    outputSVG = outputSVGDefault;
    outputResultPoints = outputResultPointsDefault;
    outputResultCells = outputResultCellsDefault;
    minCells = minCellsDefault;
    minForce = minForceDefault;

    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "e:m:f:i:p:s:t:k:l:x:rghb")) != -1)
    {
        switch (opt)
        {
        case 'e':
            epsilon = atoi(optarg);
            if (epsilon > 10000)
            {
                cout << "epsilon value invalid: " << epsilon << endl;
                cout << "Must be less than or equal 10000" << endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            minCells = atoi(optarg);
            break;
        case 'f':
            minForce = stod(optarg);
            break;
        case 'i':
            input = optarg;
            break;
        case 'p':
            inputPoints = optarg;
            break;
        case 's':
            outputSVG = optarg;
            break;
        case 't':
            outputResultPoints = optarg;
        case 'l':
            outputResultCells = optarg;
            break;
        case 'k':
            outputMapDirectory = optarg;
            break;
        case 'r':
            drawRects = false;
            break;
        case 'g':
            drawEdges = false;
            break;
        case 'b':
            drawNumbers = true;
            break;
        case 'x':
            prefix = optarg;
            break;
        case 'h':
        default: /* '?' */
            cout << "OVERVIEW: Clustering data using grid-based algorithm" << endl;
            cout << "          The program output an SVG file if dimension = 2" << endl;
            cout << endl;
            cout << "USAGE: getCluster <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-e <value>\tEpsilon (defaut: " << epsilonDefault << ")" << endl;
            cout << "-m <value>\tMinimum Cells (default: " << minCellsDefault << ")" << endl;
            cout << "-f <value>\tMinimum Force (default: " << minForceDefault << ")" << endl;
            cout << "-i <file>\tinput file (default: " << inputDefault <<")" << endl;
            cout << "-p <file>\tpoints file (there is no default)" << endl;
            cout << "-s <file>\tSVG output file (" << outputSVGDefault<< ")" << endl;
            cout << "-t <file>\tResult points file (" << outputResultPointsDefault << ")" << endl;
            cout << "-l <file>\tResult cells file (" << outputResultCellsDefault << ")" << endl;
            cout << "-r don't draw rectangles" << endl;
            cout << "-g don't draw edges" << endl;
            cout << "-h print this help" << endl;
            cout << "-k output directory for map files (default: current directory)" << endl;
            cout << "-x <prefix> Prefix to create maps files" << endl;
            exit(EXIT_FAILURE);
        }
    }

    outputMapCells = outputMapDirectory + "/" + prefix + "cells-map.csv";
    outputMapPoints = outputMapDirectory + "/" + prefix + "points-map.csv";

    cout << "Parameters" << endl;
    cout << "----------" << endl;
    cout << "Epsilon:    " << epsilon << endl;
    cout << "Min Cells:  " << minCells << endl;
    cout << "Min Force:  " << minForce << endl;
    cout << "Input file: " << input << endl;
    if (inputPoints.size() > 0)
        cout << "Point file: " << inputPoints << endl;
    cout << "Result Points file: " << outputResultPoints << endl;
    cout << "Result Cells file : " << outputResultCells << endl;
    cout << "Map Cells file    : " << outputMapCells << endl;
    cout << "Map Points file   : " << outputMapPoints << endl;
    cout << "SVG file          : " << outputSVG << endl;
    cout << endl <<"Running" << endl;
    cout << "-------" << endl;

    drawPoints = (inputPoints.size() > 0);

    infile.open(input.c_str());
    if (!infile)
    {
        cout << "Error: Unable to open input file: "<< input << endl;
        exit(EXIT_FAILURE);
    }

    // Start to read CSV file
    int r = 0;

    if (!getCSVLine(infile, headerCSV))
    {
        cout << "Error reader CSV File (Header) on the first line" << endl;
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

    // Define max number of points
    unsigned maxPoints = 0;

    unsigned totPoints = 0;

    while (infile)
    {
        r = getCSVLine(infile, lineCSV);
        ++lineNo;
        if (!r)
        {
            cout << "Error reading CSV File (Data) on Line " << lineNo + 2  << "\n";
            exit(EXIT_FAILURE);
        }
        if (lineCSV.size() != noFields)
        {
            cout << "Error reading CSV File (Data) on Line " << lineNo + 2 << ": number of "
                 << "fields doesn't match" << endl;
            cout << "Number of Fields expected: " << noFields << endl;
            cout << "Number of Fields read: " << lineCSV.size() << endl;

            exit(EXIT_FAILURE);
        }

        // Define a Cell pointer
        Cell* pNewCell;
        // a vector Sample and a vector to coordinate
        vector<int> coordCell; // Cells coordinates
        Point centerMass(dimension); // Center of Mass coordinates
        unsigned qtyPoints;
        // Define a dimension
        coordCell.resize(dimension);
        // And initialize the pointer
        pNewCell = NULL;

        // Read the cell's coordinates from CSV line

        for (unsigned i = 0; i < dimension; i++)
        {
            centerMass.coord[i] = stod(lineCSV[i+dimPosition]);
            coordCell[i] = (int)(centerMass.coord[i] * epsilon);
        }

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
            cout << "Error on Merge Points on Cell - Line " << lineNo << endl;
            return 1;
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
        return 1;
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
                if (gravForce > minForce)
                {
                    nodesToBeConnected.push_back(j);
                }
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

    vector<unsigned> clusters = g.getClusters();

    cout << endl
         << "\t--------------------" << endl
         << "\tCluster\tQty Cells"<< endl
         << "\t--------------------" << endl;

    unsigned totClusters = 0;
    for (unsigned i = 0; i < clusters.size(); i++)
    {
        if (clusters[i] >= minCells)
        {
            cout << "\t" << i << "\t" << clusters[i] << endl;
            totClusters++;
        }
    }
    cout << "\t----------------------" << endl;
    cout << "\t Total Clusters: " << totClusters << endl;
    cout << "\t----------------------" << endl << endl;


    bool pointFileOk = true;
    bool hasGT = true;  // Has Ground truth?

    vector<pointGT> rawPoints;

    /*
    *  2-dimensional vectors to make correspondence between clusters found out by gCluster and Ground Truths
    *
    *  mapClusters[idxGC][idxGT][0..1] where idxGC = 0 to qty Clusters found - 1
    *                                        idxGT = 0 to qty of GT found on idxGC - 1
    *
    *  mapClusters[idxGC][idxGT][0] = label Ground Truth
    * mapClusters[idxGC][idxGT][1] = Qty of occurrences of label Ground Truth on label gCluster
    */

    vector< vector< vector<int> > >  mapClustersPoints(clusters.size());
    vector< vector< vector<int> > >  mapClustersCells(clusters.size());

    if (drawPoints)
    {

        bool procInPoints = true;
        headerCSV.resize(0);

        while (procInPoints)
        {

            inPoint.open(inputPoints.c_str());
            if (!inPoint)
            {
                cerr << "Error: Unable to open input file Points: " << endl;
                cerr << "\tFile: " << inputPoints << endl;
                procInPoints = false;
                pointFileOk = false;
                continue;
            }

            if (!getCSVLine(inPoint, headerCSV))
            {
                cerr << "Error reader Points File (Header) on the first line" << endl;
                procInPoints = false;
                continue;
            }

            noFields = headerCSV.size(); // Define number of fields

            if (noFields != (dimension + 1)) // Dimension + 1 if has
            {
                cerr << "Error reading Points File (Header). Number of dimensions doesn't match" << endl;
                cerr << "\tFile: " << inputPoints << endl;
                procInPoints = false;
                pointFileOk = false;
                continue;
            }

            // Start to read CSV file
            int r = 0;

            lineNo = 1;

            // Open file Result
            resultFile.open(outputResultPoints, std::ifstream::out);


            // Open Map Point file
            mapFile.open(outputMapPoints, std::ifstream::out);

            for (unsigned i = 0; i < dimension; i++)
                resultFile << "Coord-" << i << ",";

            resultFile << "class-label,gCluster-label";

            if (headerCSV.back() == "_#NO_CLASS#_")
            {
                resultFile << ",_#NO_CLASS#_";
                hasGT = false;
            }
            else
                resultFile << ",ground-truth-label";

            resultFile << endl;

            while (inPoint && procInPoints)
            {
                r = getCSVLine(inPoint, lineCSV);
                ++lineNo;
                if (!r)
                {
                    cerr << "Error reading CSV File (Data) on Line " << lineNo + 2  << "\n";
                    cerr << "\tFile: " << inputPoints << endl;
                    procInPoints = false;
                    pointFileOk = false;
                    continue;
                }
                if (lineCSV.size() != noFields)
                {
                    cerr << "Error reading CSV File (Data) on Line " << lineNo + 2 << endl
                         << "\tFile: " << inputPoints << endl
                         << "\tNumber of fields doesn't match" << endl;
                    cerr << "\tNumber of Fields expected: " << noFields << endl;
                    cerr << "\tNumber of Fields read: " << lineCSV.size() << endl;
                    procInPoints = false;
                    pointFileOk = false;
                    continue;
                }

                Point p(dimension); // Center of Mass coordinates

                // Read the cell's coordinates from CSV line
                for (unsigned i = 0; i < dimension; i++)
                    p.coord[i] = stod(lineCSV[i]);

                // Verify the cells coordinate, to find out the cluster

                vector<int> coordCell; // Cells coordinates

                coordCell.resize(dimension);

                // Determine the cell's coordinates

                for (unsigned i = 0; i < dimension; i++)
                    coordCell[i] = (int)(p.coord[i] * epsilon);

                int clusterFound = -1;
                int labelCell = -1;
                for (unsigned i=0; i<listCells.size(); i++) // Search cells
                {
                    // Compare listCells and current cell
                    if (coordCell == listCells[i]->coord) // all coordinates are equal
                    {
                        // Verify
                        labelCell = i;
                        if (clusters[g.getClusterIndex(i)] >= minCells)
                            clusterFound = g.getClusterIndex(i);
                        break;
                    }
                }

                //
                // Insert into rawPoints vector

                pointGT pRP;

                pRP.rawPoint = p;
                pRP.classAlgo = clusterFound;
                pRP.classGT = atoi(lineCSV.back().c_str());

                rawPoints.push_back(pRP);

                resultFile << p.coord[0];
                for (unsigned i = 1; i < dimension; i++)
                    resultFile << "," << p.coord[i];

                resultFile << ","<< labelCell;
                resultFile << "," << clusterFound;
                resultFile << "," << lineCSV.back() << endl;

                if (clusterFound >= 0) // is not -1
                {
                    int idxGC = clusterFound;   // Index of gCluster on the matrix
                    int idxGT = -1;             // Index of Ground Truth on the matrix
                    // Try to find out GT on matrix of cluster Found
                    for (unsigned j = 0; j < mapClustersPoints[idxGC].size(); j++ )
                    {
                        if (mapClustersPoints[idxGC][j][0] == pRP.classGT)
                            idxGT = j;
                    }

                    // If don't find Ground Truth, create a new matrix
                    if (idxGT == -1)
                    {
                        // Create a new Ground Truth on Map
                        vector<int> matAux(2);
                        matAux[0] = pRP.classGT;
                        matAux[1] = 1;
                        mapClustersPoints[idxGC].push_back(matAux);
                    }
                    else
                    {
                        mapClustersPoints[idxGC][idxGT][1]++;
                    }
                }
                lineCSV.resize(0);
            }

            procInPoints = false;
        }
        inPoint.close();

        for (unsigned i = 0; i < clusters.size(); i++)
        {
            int maxPts = 0;
            int labelGT = -1;
            if (clusters[i] >= minCells)
            {
                // Iterate map for cluster i
                for (unsigned j = 0; j < mapClustersPoints[i].size(); j++)
                {
                    if (mapClustersPoints[i][j][1] > maxPts)
                    {
                        maxPts = mapClustersPoints[i][j][1];
                        labelGT = mapClustersPoints[i][j][0];
                    }
                }
                mapFile << i << "," << labelGT << endl;
            }
        }
        resultFile.close();
        mapFile.close();
    }

    // Now rowPoitns has a copy of row result file.

    // Creating map to make a relation between cluster labels (label Algo --> label GT)

    cout << "\t----------------------------" << endl;
    cout << "\t Creating result cells file " << outputResultCells << endl;
    cout << "\t----------------------------" << endl;

    // Open file Result
    resultFile.open(outputResultCells, std::ifstream::out);

    // Create header of file
    resultFile << "cell-id,cell-label,number-points,";

    // Label of center of mass coordinates
    for (unsigned i = 0; i < dimension; i++)
        resultFile << "CM-" << i << ",";

    resultFile << "qty-cells-cluster-greater-equal-" << minCells << ",gCluster-label";

    if (rawPoints.size() > 0)
    {
        if (hasGT)
            resultFile << ", ground-truth-cell-label";
        else
            resultFile << ", _#NO_CLASS#_";
    }

    resultFile << endl;

    // iterate all cells

    for (unsigned i = 0; i<(listCells.size()-1); i++)
    {
        resultFile << i << "," << i << "," << listCells[i]->getQtyPoints() << ",";

        Point cm = listCells[i]->getCenterMass();

        // Write center of mass coordinates on file
        for (unsigned j = 0; j < cm.coord.size(); j++)
            resultFile << cm.coord[j] << ",";

        int cellClusterIdx;

        if (g.getQtyAdjacents(i) <= minCells)
            cellClusterIdx = -1;
        else
            cellClusterIdx = g.getClusterIndex(i);

        resultFile  << clusters[g.getClusterIndex(i)]
                    << "," << cellClusterIdx;

        // Try to find out the closest raw pointer if there is rawPoints

        if (rawPoints.size() > 0)
        {
            double minDist = cm.dist(rawPoints[0].rawPoint);
            unsigned posMat = 0;
            for (unsigned j = 1; j < rawPoints.size(); j++)
            {
                double curDist = cm.dist(rawPoints[j].rawPoint);
                if (curDist < minDist)
                {
                    minDist = curDist;
                    posMat = j;
                }
            }

            // Found the ground truth for the current cell
            if (cellClusterIdx >= 0) // is not -1
            {
                int idxGC = cellClusterIdx;   // Index of gCluster on the matrix
                int idxGT = -1;             // Index of Ground Truth on the matrix
                // Try to find out GT on matrix of cluster Found
                for (unsigned j = 0; j < mapClustersCells[idxGC].size(); j++ )
                {
                    if (mapClustersCells[idxGC][j][0] == rawPoints[posMat].classGT)
                        idxGT = j;
                }

                // If don't find Ground Truth, create a new matrix
                if (idxGT == -1)
                {
                    // Create a new Ground Truth on Map
                    vector<int> matAux(2);
                    matAux[0] = rawPoints[posMat].classGT;
                    matAux[1] = 1;
                    mapClustersCells[idxGC].push_back(matAux);
                }
                else
                {
                    mapClustersCells[idxGC][idxGT][1]++;
                }
            }

            listCells[i]->cellGT = rawPoints[posMat].classGT;
            resultFile << "," << rawPoints[posMat].classGT;

        }
        resultFile << endl;
    }

    resultFile.close();

    if (rawPoints.size() > 0)
    {
        // Open map result
        mapFile.open(outputMapCells, std::ifstream::out);

        for (unsigned i = 0; i < clusters.size(); i++)
        {
            int maxPts = 0;
            int labelGT = -1;
            if (clusters[i] >= minCells)
            {
                // Iterate map for cluster i
                for (unsigned j = 0; j < mapClustersCells[i].size(); j++)
                {
                    if (mapClustersCells[i][j][1] > maxPts)
                    {
                        maxPts = mapClustersCells[i][j][1];
                        labelGT = mapClustersCells[i][j][0];
                    }
                }
                mapFile <<  i << "," <<  labelGT << endl;
            }
        }
        mapFile.close();
    }

    // If dimension == 2, create a svg file
    if (dimension == 2)
    {
        float graphMult = 50.0;

        cout << endl;
        cout << "\t------------------------" << endl;
        cout << "\t Creating SVG File: " << outputSVG << endl;
        cout << "\t------------------------" << endl;

        svgFile.open(outputSVG, std::ifstream::out);
        svgFile << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
                << "<svg width=\"" << graphMult*1.2 << "cm\" height=\""
                << graphMult * 1.2 << "cm\" version=\"1.1\"" << endl
                << "xmlns=\"http://www.w3.org/2000/svg\">" << endl
                << "<desc>Graph bi-dimensional </desc>" << endl
                // Display External Rectangle
                << "<rect x=\"" << graphMult * 0.1 << "cm\" "
                << "y=\"" << graphMult * 0.1 << "cm\" "
                << "width=\"" << graphMult << "cm\" "
                << "height=\"" << graphMult << "cm\" "
                << "fill=\"none\" stroke=\"black\" stroke-width=\""
                << graphMult * 0.002 << "cm\" />" << endl;
        // Display Text with parameters
        svgFile << "<text x=\"" << graphMult * 0.1 << "cm\" "
                << "y=\"" << graphMult * 1.14 << "cm\" "
                << "font-family=\"Times New Roman\" font-size=\""
                << graphMult * 0.03 << "cm\" fill=\"black\">"
                << "File Name: " << outputSVG
                << "</text>" << endl;
        svgFile << "<text x=\"" << graphMult * 0.1 << "cm\" "
                << "y=\"" << graphMult * 1.18 << "cm\" "
                << "font-family=\"Times New Roman\" font-size=\""
                << graphMult * 0.03 << "cm\" fill=\"black\">"
                << "Epsilon: " << epsilon
                << ", Min Force: " << minForce
                << ", Min Cells: " << minCells
                << ", Number Clusters: " << totClusters
                << "</text>" << endl;

        // Draw Horizontal and Vertical Grids
        svgFile << "<g stroke=\"gray\" stroke-width=\""
                << graphMult * 0.025 / epsilon << "cm\">" << endl;
        for (unsigned i = 1; i < epsilon; i++)
        {
            // Vertical Grids
            svgFile << "<line x1=\""
                    << (graphMult * 1/(float)epsilon * i) + graphMult*0.1<< "cm\" "
                    << "y1=\"" << graphMult * .1 << "cm\" "
                    << "x2=\"" << (graphMult * 1/(float)epsilon * i) + graphMult*0.1<< "cm\" "
                    << "y2=\"" << graphMult * 1.1 << "cm\" />" << endl;
            // Horizontal Grids
            svgFile << "<line y1=\""
                    << (graphMult * 1/(float)epsilon * i) + graphMult*0.1<< "cm\" "
                    << "x1=\"" << graphMult * .1 << "cm\" "
                    << "y2=\"" << (graphMult * 1/(float)epsilon * i) + graphMult*0.1<< "cm\" "
                    << "x2=\"" << graphMult * 1.1 << "cm\" />" << endl;
        }
        svgFile <<"</g>" << endl;

        // Draw Edges
        vector<string> colors;
        colors.push_back("#000000");
        colors.push_back("#FF7F00");
        colors.push_back("#000077");
        colors.push_back("#bf870f");
        colors.push_back("#7F0000");
        colors.push_back("#0000FF");
        colors.push_back("#ef5d09");
        colors.push_back("#FF0000");
        colors.push_back("#00007F");
        colors.push_back("#2d60ad");
        colors.push_back("#44ef0b");
        colors.push_back("#007F00");
        colors.push_back("#7F007F");
        colors.push_back("#007F7F");
        colors.push_back("#6eb4ea");
        colors.push_back("#7F7F00");
        colors.push_back("#5f397a");
        colors.push_back("#007777");
        colors.push_back("#ffbf00");
        colors.push_back("#7676ed");
        colors.push_back("#007700");
        colors.push_back("#c10f6e");


        svgFile << "<g stroke=\"black\" stroke-width=\"" <<
                graphMult * 0.09 / epsilon <<
                "cm\">" << endl;

        vector<bool> rectDrawn(listCells.size());
        vector< vector<bool> > edgeDraw(listCells.size());

        for (unsigned i = 0; i < rectDrawn.size(); i++)
            rectDrawn[i] = false;

        for (unsigned i = 0; i < edgeDraw.size(); i++)
            for (unsigned j = 0; j < edgeDraw.size(); j++)
                edgeDraw[i].push_back(false);

        for (unsigned i = 0; i<listCells.size(); i++)
        {

            // Draw transparent rect
            string opacity;
            opacity = (!drawEdges?"0.7":"0.6");

            // Find out the connected cells

            unsigned cellClusterIdx = clusters[g.getClusterIndex(i)];

            if (cellClusterIdx <= minCells)
                cellClusterIdx = -1;

            if (drawNumbers)
                svgFile << strSVGTextCell(graphMult, epsilon,
                                          listCells[i]->coord[0],
                                          listCells[i]->coord[1],
                                          i, cellClusterIdx,
                                          listCells[i]->cellGT) << endl;

            vector<unsigned> adj = g.getAdjacents(i);

            for (unsigned j = 0; j < adj.size(); j++)
            {
                unsigned cellAdj = adj[j];
                bool hasCells = (clusters[g.getClusterIndex(cellAdj)] >= minCells);
                if (drawRects && !rectDrawn[cellAdj] && hasCells)
                {
                    // first print the current cell
                    svgFile << strSVGRect(opacity,
                                          graphMult,
                                          epsilon,
                                          colors[g.getClusterIndex(cellAdj)%colors.size()],
                                          listCells[cellAdj]->coord[0],
                                          listCells[cellAdj]->coord[1]);
                    rectDrawn[cellAdj] = true;
                }

                if (drawEdges && !edgeDraw[i][cellAdj] && hasCells)
                {
                    svgFile << strSVGEdge(graphMult,
                                          colors[g.getClusterIndex(i)%colors.size()],
                                          listCells[i]->getCenterMass().coord[0],
                                          listCells[i]->getCenterMass().coord[1],
                                          listCells[cellAdj]->getCenterMass().coord[0],
                                          listCells[cellAdj]->getCenterMass().coord[1]);
                    edgeDraw[i][cellAdj] = true;
                    edgeDraw[cellAdj][i] = true;

                }
            }
        }

        if (drawPoints && pointFileOk)
        {

            cout << endl;
            cout << "\t Drawing Points " << endl;
            cout << "\t----------------" << endl;

            bool procInPoints = true;
            headerCSV.resize(0);

            while (procInPoints)
            {
                inPoint.open(inputPoints.c_str());
                if (!inPoint)
                {
                    cout << "Error: Unable to open input file Points: " << inputPoints << endl;
                    procInPoints = false;
                    continue;
                }

                if (!getCSVLine(inPoint, headerCSV))
                {
                    cout << "Error reader Points File (Header) on the first line" << endl;
                    procInPoints = false;
                    continue;
                }

                noFields = headerCSV.size(); // Define number of fields

                if (noFields != (dimension + 1)) // Dimension + 1 if has
                {
                    cout << "Error reading Points File (Header). Number of dimensions doesn't match" << endl;
                    procInPoints = false;
                    continue;
                }
                cout << endl;

                // Start to read CSV file
                int r = 0;

                lineNo = 1;

                while (inPoint && procInPoints)
                {
                    r = getCSVLine(inPoint, lineCSV);
                    ++lineNo;
                    if (!r)
                    {
                        cout << "Error reading CSV File (Data) on Line " << lineNo + 2  << "\n";
                        procInPoints = false;
                        continue;
                    }
                    if (lineCSV.size() != noFields)
                    {
                        cout << "Error reading CSV File (Data) on Line " << lineNo + 2 << endl
                             << "\tNumber of fields doesn't match" << endl;
                        cout << "\tNumber of Fields expected: " << noFields << endl;
                        cout << "\tNumber of Fields read: " << lineCSV.size() << endl;
                        procInPoints = false;
                        continue;
                    }

                    Point p(dimension); // Center of Mass coordinates

                    // Read the cell's coordinates from CSV line
                    for (unsigned i = 0; i < dimension; i++)
                        p.coord[i] = stod(lineCSV[i]);

                    // Verify the cells coordinate, to find out the cluster

                    vector<int> coordCell; // Cells coordinates

                    coordCell.resize(dimension);

                    // Determine the cell's coordinates

                    for (unsigned i = 0; i < dimension; i++)
                        coordCell[i] = (int)(p.coord[i] * epsilon);

                    int clusterFound = -1;
                    for (unsigned i=0; i<listCells.size(); i++) // Search cells
                    {
                        // Compare listCells and current cell
                        if (coordCell == listCells[i]->coord) // all coordinates are equal
                        {
                            // Verify
                            if (clusters[g.getClusterIndex(i)] >= minCells)
                                clusterFound = g.getClusterIndex(i);
                            break;
                        }
                    }

                    int indClass = atoi(lineCSV.back().c_str());
                    string colorPoint = "none";
                    string opacity = "1.0";
                    string width = "0.03";
                    double radio = 0.002;

                    if (indClass >= 0)
                    {
                        colorPoint = colors[indClass%colors.size()];
                        opacity = "0.4";
                        width = "0.05";
                        radio = 0.006;
                    }

                    double xCirc = (double)(p.coord[0]*graphMult)+graphMult*.1;
                    double yCirc = (double) graphMult*1.1-(float)(p.coord[1]*graphMult);
                    svgFile << "<circle fill=\""<< colorPoint<< "\""
                            << " style=\"opacity:" << opacity << "\" "
                            << " stroke-width=\""<< width << "cm\"  cx=\""
                            << xCirc << "cm\" "
                            << "cy=\"" << yCirc
                            << "cm\" r=\"" << graphMult * radio << "cm\""
                            << " />" << endl;
                    if (drawNumbers)
                    {
                        svgFile << "<text "
                                << "x=\"" << xCirc - 2 * radio << "cm\" "
                                << "y=\"" << yCirc + radio << "cm\" "
                                << "font-style=\"normal\" "
                                << "font-family=\"Times New Roman\" "
                                << "font-size=\"" << graphMult * 0.01 <<"cm\" "
                                << "stroke=\"none\" fill=\""<< colorPoint <<"\" > "
                                << lineCSV[lineCSV.size()-1]
                                << "</text>" << endl;
                    }
                    lineCSV.resize(0);
                }

                procInPoints = false;
            }

            inPoint.close();
        }

        if (dimension == 2)
            svgFile << "</g>" << endl;

    }

    svgFile << "</svg>";
    svgFile.close();

    exit(EXIT_SUCCESS);

    infile.close();
}
