/*
 * Program to test the results of gCluster
 *
 * Read Points file, that could have or not ground truth and prepare a CSV file
 * to be read by validation.py script
 *
 * Input files:  -p <point file>     sumData output (required)
 *                                   (with the Ground Truth on the last field)
 *               -c <cell file>      getCluster output
 *               -d <dbscan point file> DBSCAN.py output over sumData output
 *               -b <dbscan cell file>  DBSCAN.py output over getCluster output
 *
 * Output files: -1 <point gCluster with GT>
 *               -2 <cells gCluster with GT>
 *               -3 <point DBSCAN with GT>
 *               -4 <cells DBSCAN with GT>
 *
 * Epsilon       -e <epsilon>
 *
 */

#include "statCluster.h"

int main(int argc, char* argv[])
{
    int opt; // command line options
/*
 *     Files
 */
    ifstream inPoint; // input file - point
    ifstream inCell;  // input file - cell
    ifstream inPointDB; // input file - point DBSCAN
    ifstream inCellDB;  // input file - cell DBSCAN

    ofstream outPoint;
    ofstream outCell;
    ofstream outPointDB;
    ofstream outCellDB;
    ofstream mapFile;

    // Variables to deal with files
    int lineNo = 0;
    unsigned dimension;
    unsigned noFields;  // number of fields
    vector<string> lineCSV;
    vector<string> headerPointCSV;
    vector<string> headerCellCSV;

    vector<Cell> listCells; // Vector of Cells
    vector<rawPoint> listPoints; // Vector of Points

    // Parameters and theirs defaults
    string inputPoint = "";
    string inputCell = "";
    string inputPointDB = "";
    string inputCellDB = "";

    string outputPoint = "";
    string outputPointDefault = "map-point-gC.csv";
    string outputCell = "";
    string outputCellDefault = "map-cell-gC.csv";

    string outputPointDB = "";
    string outputPointDBDefault = "map-point-dbscan.csv";
    string outputCellDB = "";
    string outputCellDBDefault = "map-cell-dbscan.csv";

    unsigned epsilon;

    cout << "gCluster algorithm - Statistic module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/gCluster" << endl;
    cout << "========================================================================" << endl;

    // Initialize options

    epsilon = 0;

    outputPoint = outputPointDefault;
    outputCell = outputCellDefault;
    outputPointDB = outputPointDBDefault;
    outputCellDB = outputCellDBDefault;

    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "p:c:d:b:1:2:3:4:e:")) != -1)
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
        case 'p':
            inputPoint = optarg;
            break;
        case 'c':
            inputCell = optarg;
            break;
        case 'd':
            inputPointDB = optarg;
            break;
        case 'b':
            inputCellDB = optarg;
            break;
        case '1':
            outputPoint = optarg;
        case '2':
            outputCellDefault = optarg;
            break;
        case '3':
            outputPointDB = optarg;
            break;
        case '4':
            outputCellDB = false;
            break;
        case 'h':
        default: /* '?' */
            cout << "OVERVIEW: Clustering data using gCluster algorithm" << endl;
            cout << endl;
            cout << "USAGE: statCluster <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-h print this help" << endl;
            cout << "-e <value>\tEpsilon (required)" << endl;
            cout << "-p <file>\tinput points file (required)" << endl;
            cout << "-c <file>\tinput cells file (required)" << endl;
            cout << "-d <file>\tinput points DBSCAN file (optional)" << endl;
            cout << "-b <file>\tinput cells DBSCAN file (optional)" << endl;
            cout << "-1 <file>\toutput points file" << endl;
            cout << "-2 <file>\toutput cells file" << endl;
            cout << "-3 <file>\toutput points DBSCAN file" << endl;
            cout << "-4 <file>\toutput cells DBSCAN file" << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (epsilon == 0)
    {
        cerr << "Epsilon is required, please inform it on -e option" << endl;
        exit (EXIT_FAILURE);
    }

    if (inputPoint == "" || inputCell == "")
    {
        cerr << "Input Point File and Input Cell file are required" << endl;
        exit (EXIT_FAILURE);
    }

    cout << " Parameters" << endl;
    cout << "---------------------" << endl;
    cout << "Epsilon.............: " << epsilon << endl;
    cout << "Input Points........: " << inputPoint << endl;
    cout << "Input Cells.........: " << inputCell << endl;
    cout << "Input Points DBSCAN.: " << inputPointDB << endl;
    cout << "Input Cells DBSCAN..: " << inputCellDB << endl;
    cout << "Output Points.......: " << outputPoint << endl;
    cout << "Output Cells........: " << outputCell << endl;
    cout << "Output Points DBSCAN: " << outputPointDB << endl;
    cout << "Output Cells DBSCAN.: " << outputCellDB << endl;
    cout << endl <<"Running" << endl;
    cout << "-------" << endl;

    inCell.open(inputCell.c_str());
    if (!inCell)
    {
        cerr << "Error: Unable to open input cell file: " << inputCell << endl;
        exit(EXIT_FAILURE);
    }

/************************************************************
 *
 * 1. Process input cell file, filling listCells up
 *
 * Fields:
 *
 * CenterMass

 * The only unfilled field in that moment is Ground truth,
 * due lack of information at this time.
 *
 ************************************************************/
    int r = 0;

    if (!getCSVLine(inCell, headerCellCSV))
    {
        cerr << "Error reading cell file (Header) on the first line: " << inputCell << endl;
        exit(EXIT_FAILURE);
    }

    noFields = headerCellCSV.size(); // Define number of fields

    cout << "Reading cell file header. Number of Fields:" << noFields << endl;

    for (unsigned i = 0; i < headerCellCSV.size(); i++)
        cout << (i?",": "") << headerCellCSV[i];

    cout << endl;

    // The header has dimensions plus 4 fields (cell-id, number-points, qty-cells, gCluster-label)
    dimension = headerCellCSV.size() - resultgC_qtyFixedLabels;

    cout << "cell file - Reading Data" << endl;

    lineNo = 0;

    lineCSV.resize(0);

    while (inCell)
    {
        r = getCSVLine(inCell, lineCSV);
        ++lineNo;
        if (!r)
        {
            cerr << "Error reading input cell file (Data) on Line " << lineNo + 2  << "\n";
            exit(EXIT_FAILURE);
        }
        if (lineCSV.size() != noFields)
        {
            cerr << "Error reading input cell file (Data) on Line " << lineNo + 2 << ": number of "
                 << "fields doesn't match" << endl;
            cerr << "\tNumber of Fields expected: " << noFields << endl;
            cerr << "\tNumber of Fields read: " << lineCSV.size() << endl;

            exit(EXIT_FAILURE);
        }

        // Create a aux variable to store point
        Point cm(dimension);
        Cell newCell;

        // Read the point's coordinates from CSV line

        for (unsigned i = 0; i < dimension; i++)
            cm.coord[i] = stod(lineCSV[i+resultgC_posDimension()]);

        // Create a new point read from file
        newCell.setCenterMass(cm);
        newCell.setLabelgC(atoi(lineCSV[resultgC_posGCLabel()].c_str())); // Take last position of CSV
        newCell.setLabelGT(-1);

        vector<int> cellCoord = getCellCoord(cm, epsilon);

        newCell.coord = getCellCoord(cm, epsilon);

        listCells.push_back(newCell);

        lineCSV.resize(0);
    }

    inCell.close();

/*********************************************************
 *
 * 2. Process input points file and fill the listCells up
 *
 * Read points and look at listCells to find out the cell
 * it belongs to.
 *
 * Also update listCells
 *
 ********************************************************/
    inPoint.open(inputPoint.c_str());
    if (!inPoint)
    {
        cerr << "Error: Unable to open input point file: "<< inputPoint << endl;
        exit(EXIT_FAILURE);
    }

    r = 0;

    if (!getCSVLine(inPoint, headerPointCSV))
    {
        cerr << "Error reading point file (Header) on the first line" << endl;
        exit(EXIT_FAILURE);
    }

    noFields = headerPointCSV.size(); // Define number of fields

    cout << "Reading CSV Point File header. Number of Fields:" << noFields << endl;

    for (unsigned i = 0; i < headerPointCSV.size(); i++)
        cout << (i?",": "") << headerPointCSV[i+consolidPts_posDimension()];

    cout << endl;

    // The header has dimensions plus 1 field (Ground Truth)
    dimension = headerPointCSV.size() - 1;

    cout << "CSV File - Reading Data" << endl;

    lineNo = 0;

    lineCSV.resize(0);

    while (inPoint)
    {
        r = getCSVLine(inPoint, lineCSV);
        ++lineNo;
        if (!r)
        {
            cerr << "Error reading input point file (Data) on Line " << lineNo + 2  << "\n";
            exit(EXIT_FAILURE);
        }

        if (lineCSV.size() != noFields)
        {
            cerr << "Error reading input point file (Data) on Line " << lineNo + 2 << ": number of "
                 << "fields doesn't match" << endl;
            cerr << "\tNumber of Fields expected: " << noFields << endl;
            cerr << "\tNumber of Fields read: " << lineCSV.size() << endl;

            exit(EXIT_FAILURE);
        }

        // Create a aux variable to store point
        Point p(dimension);
        rawPoint newPoint;

        // Read the point's coordinates from CSV line

        for (unsigned i = 0; i < dimension; i++)
        {
            p.coord[i] = stod(lineCSV[i]);
        }

        // Create a new point read from file
        newPoint.rawData = p;
        newPoint.classAlgo = -1;
        newPoint.classGT = atoi(lineCSV[dimension].c_str());

        vector<int> cellCoord = getCellCoord(p, epsilon);

        for (unsigned i=0; i<listCells.size(); i++) // Search cells to find out cluster label on cells
        {
            // Compare listCells and current cell

            if (cellCoord == listCells[i].coord) // all coordinates are equal
            {
                newPoint.classAlgo = listCells[i].getLabelgC();
                // Need to update cell with ground truth to find out the ground truth of cell

                break;
            }
        }

        listPoints.push_back(newPoint);

        lineCSV.resize(0);
    }

    inPoint.close();


/*********************************************************
 *
 * 3. Now we have listPoints and listCells filled up
 *
 * Read points and look at listCells to find out the cell
 * it belongs to.
 *
 *
 ********************************************************/

/*********************************************************
 *
 * 4. Store listCells and listPoints on the disk
 *
 * Read points and look at listCells to find out the cell
 * it belongs to.
 *
 *
 ********************************************************/


    outPoint.open(outputPoint, std::ifstream::out);
    if (!outPoint.is_open())
    {
        cerr << "Could not open output file:" << outputPoint << endl;
        exit (EXIT_FAILURE);
    }

    // Write header

    for (unsigned i = 0; i < dimension; i++)
        outPoint << headerCellCSV[i] << ",";

    outPoint << "label-cell,label-gC,label-GT" << endl;

    for (rawPoint line : listPoints)
    {
        for (unsigned i = 0; i < dimension; i++)
            outPoint << line.rawData.coord[i] << ",";

        outPoint << line.classAlgo << "," << line.classGT << endl;

    }

    outPoint.close();

//    Point a(dimension);
//
//    cout << "CSV File - Total of " << lineNo << " lines of data read" << endl;
//    cout << "Number of Cells   : " << listCells.size() << endl;
//    cout << "Number of Points  : " << totPoints << endl;
//    cout << "Avg Points by Cell: " << (double)totPoints / (double)listCells.size() << endl;
//
//    if (listCells.size() == 0)
//    {
//        cerr << "Error on algorithm: There is no cells" << endl;
//        return 1;
//    }
//
//    // All cells were processed, now process the graph
//
//    // Create a Graph with number of nodes = number of cells
//    Graph g(listCells.size());
//
//    // iterate all cells
//    for (unsigned i = 0; i<(listCells.size()-1); i++)
//    {
//        vector<unsigned> nodesToBeConnected;
//        for (unsigned j = i+1; j<listCells.size(); j++)
//        {
//            if (areAdjacents(listCells[i], listCells[j]))
//            {
//                // Calculate gravitational force
//                double distance = listCells[i]->getCenterMass().dist(listCells[j]->getCenterMass());
//                distance /= 1/(double)epsilon;
//                // Gravitational force is relative Qty of Points[j] * relative Qty of Poinst[i] / distance ^ 2
//                double relativeQtyI = (double)listCells[i]->getQtyPoints() / maxPoints;
//                double relativeQtyJ = (double)listCells[j]->getQtyPoints() / maxPoints;
//                double gravForce = (relativeQtyI)*(relativeQtyJ)/(distance*distance);
//                if (gravForce > minForce)
//                {
//                    nodesToBeConnected.push_back(j);
//                }
//            }
//        }
//
//        // for this cell (index = i), include in the correspondent node (index = i)
//        // an adjacent matrix, where there is all nodes connected to this cell
//        // The last parameter is the weight of edge. It´s not being used at this implementation
//        for (unsigned j = 0; j < nodesToBeConnected.size(); j++)
//            g.addEdge(i, nodesToBeConnected[j], 1.); // Create an edge on Graph
//
//    }
//
//    // connected Components give labels for clusters and
//    // for each node store its cluster on g.[<nodeIndex>].clusterIndex
//    // Also, update an internal vector of graph that store the numbers of nodes each cluster has
//    // This vector could be gathered by getCluster method
//    g.connectedComponents(); // Verify the connected components of graph
//
//    vector<unsigned> clusters = g.getClusters();
//
//    cout << endl
//         << "\t--------------------" << endl
//         << "\tCluster\tQty Cells"<< endl
//         << "\t--------------------" << endl;
//
//    unsigned totClusters = 0;
//    for (unsigned i = 0; i < clusters.size(); i++)
//    {
//        if (clusters[i] >= minCells)
//        {
//            cout << "\t" << i << "\t" << clusters[i] << endl;
//            totClusters++;
//        }
//    }
//    cout << "\t----------------------" << endl;
//    cout << "\t Total Clusters: " << totClusters << endl;
//    cout << "\t----------------------" << endl << endl;
//
//
//    bool pointFileOk = true;
//    bool hasGT = true;  // Has Ground truth?
//
//    /*
//    *  2-dimensional vectors to make correspondence between clusters found out by gCluster and Ground Truths
//    *
//    *  mapClusters[idxGC][idxGT][0..1] where idxGC = 0 to qty Clusters found - 1
//    *                                        idxGT = 0 to qty of GT found on idxGC - 1
//    *
//    *  mapClusters[idxGC][idxGT][0] = label Ground Truth
//    * mapClusters[idxGC][idxGT][1] = Qty of occurrences of label Ground Truth on label gCluster
//    */
//
//    vector< vector< vector<int> > >  mapClustersPoints(clusters.size());
//    vector< vector< vector<int> > >  mapClustersCells(clusters.size());
//
//    if (drawPoints)
//    {
//
//        bool procInPoints = true;
//        headerCSV.resize(0);
//
//        while (procInPoints)
//        {
//
//            inPoint.open(inputPoints.c_str());
//            if (!inPoint)
//            {
//                cerr << "Error: Unable to open input file Points: " << endl;
//                cerr << "\tFile: " << inputPoints << endl;
//                procInPoints = false;
//                pointFileOk = false;
//                continue;
//            }
//
//            if (!getCSVLine(inPoint, headerCSV))
//            {
//                cerr << "Error reader Points File (Header) on the first line" << endl;
//                procInPoints = false;
//                continue;
//            }
//
//            noFields = headerCSV.size(); // Define number of fields
//
//            if (noFields != (dimension + 1)) // Dimension + 1 if has
//            {
//                cerr << "Error reading Points File (Header). Number of dimensions doesn't match" << endl;
//                cerr << "\tFile: " << inputPoints << endl;
//                procInPoints = false;
//                pointFileOk = false;
//                continue;
//            }
//
//            // Start to read CSV file
//            int r = 0;
//
//            lineNo = 1;
//
//            // Open file Result
//            resultFile.open(outputResultPoints, std::ifstream::out);
//
//
//            // Open Map Point file
//            mapFile.open(outputMapPoints, std::ifstream::out);
//
//            for (unsigned i = 0; i < dimension; i++)
//                resultFile << "Coord-" << i << ",";
//
//            resultFile << "class-label,gCluster-label";
//
//            if (headerCSV.back() == "_#NO_CLASS#_")
//            {
//                resultFile << ",_#NO_CLASS#_";
//                hasGT = false;
//            }
//            else
//                resultFile << ",ground-truth-label";
//
//            resultFile << endl;
//
//            while (inPoint && procInPoints)
//            {
//                r = getCSVLine(inPoint, lineCSV);
//                ++lineNo;
//                if (!r)
//                {
//                    cerr << "Error reading CSV File (Data) on Line " << lineNo + 2  << "\n";
//                    cerr << "\tFile: " << inputPoints << endl;
//                    procInPoints = false;
//                    pointFileOk = false;
//                    continue;
//                }
//                if (lineCSV.size() != noFields)
//                {
//                    cerr << "Error reading CSV File (Data) on Line " << lineNo + 2 << endl
//                         << "\tFile: " << inputPoints << endl
//                         << "\tNumber of fields doesn't match" << endl;
//                    cerr << "\tNumber of Fields expected: " << noFields << endl;
//                    cerr << "\tNumber of Fields read: " << lineCSV.size() << endl;
//                    procInPoints = false;
//                    pointFileOk = false;
//                    continue;
//                }
//
//                Point p(dimension); // Center of Mass coordinates
//
//                // Read the cell's coordinates from CSV line
//                for (unsigned i = 0; i < dimension; i++)
//                    p.coord[i] = stod(lineCSV[i]);
//
//                // Verify the cells coordinate, to find out the cluster
//
//                vector<int> coordCell; // Cells coordinates
//
//                coordCell.resize(dimension);
//
//                // Determine the cell's coordinates
//
//                for (unsigned i = 0; i < dimension; i++)
//                    coordCell[i] = (int)(p.coord[i] * epsilon);
//
//                int clusterFound = -1;
//                int labelCell = -1;
//                for (unsigned i=0; i<listCells.size(); i++) // Search cells
//                {
//                    // Compare listCells and current cell
//                    if (coordCell == listCells[i]->coord) // all coordinates are equal
//                    {
//                        // Verify
//                        labelCell = i;
//                        if (clusters[g.getClusterIndex(i)] >= minCells)
//                            clusterFound = g.getClusterIndex(i);
//                        break;
//                    }
//                }
//
//                //
//                // Insert into rawPoints vector
//
//                pointGT pRP;
//
//                pRP.rawPoint = p;
//                pRP.classAlgo = clusterFound;
//                pRP.classGT = atoi(lineCSV.back().c_str());
//
//                rawPoints.push_back(pRP);
//
//                resultFile << p.coord[0];
//                for (unsigned i = 1; i < dimension; i++)
//                    resultFile << "," << p.coord[i];
//
//                resultFile << ","<< labelCell;
//                resultFile << "," << clusterFound;
//                resultFile << "," << lineCSV.back() << endl;
//
//                if (clusterFound >= 0) // is not -1
//                {
//                    int idxGC = clusterFound;   // Index of gCluster on the matrix
//                    int idxGT = -1;             // Index of Ground Truth on the matrix
//                    // Try to find out GT on matrix of cluster Found
//                    for (unsigned j = 0; j < mapClustersPoints[idxGC].size(); j++ )
//                    {
//                        if (mapClustersPoints[idxGC][j][0] == pRP.classGT)
//                            idxGT = j;
//                    }
//
//                    // If don't find Ground Truth, create a new matrix
//                    if (idxGT == -1)
//                    {
//                        // Create a new Ground Truth on Map
//                        vector<int> matAux(2);
//                        matAux[0] = pRP.classGT;
//                        matAux[1] = 1;
//                        mapClustersPoints[idxGC].push_back(matAux);
//                    }
//                    else
//                    {
//                        mapClustersPoints[idxGC][idxGT][1]++;
//                    }
//                }
//                lineCSV.resize(0);
//            }
//
//            procInPoints = false;
//        }
//        inPoint.close();
//
//        for (unsigned i = 0; i < clusters.size(); i++)
//        {
//            int maxPts = 0;
//            int labelGT = -1;
//            if (clusters[i] >= minCells)
//            {
//                // Iterate map for cluster i
//                for (unsigned j = 0; j < mapClustersPoints[i].size(); j++)
//                {
//                    if (mapClustersPoints[i][j][1] > maxPts)
//                    {
//                        maxPts = mapClustersPoints[i][j][1];
//                        labelGT = mapClustersPoints[i][j][0];
//                    }
//                }
//                mapFile << i << "," << labelGT << endl;
//            }
//        }
//        resultFile.close();
//        mapFile.close();
//    }
//
//    // Now rowPoitns has a copy of row result file.
//
//    // Creating map to make a relation between cluster labels (label Algo --> label GT)
//
//    cout << "\t----------------------------" << endl;
//    cout << "\t Creating result cells file " << outputResultCells << endl;
//    cout << "\t----------------------------" << endl;
//
//    // Open file Result
//    resultFile.open(outputResultCells, std::ifstream::out);
//
//    // Create header of file
//    resultFile << "cell-id,cell-label,number-points,";
//
//    // Label of center of mass coordinates
//    for (unsigned i = 0; i < dimension; i++)
//        resultFile << "CM-" << i << ",";
//
//    resultFile << "qty-cells-cluster-greater-equal-" << minCells << ",gCluster-label";
//
//    if (rawPoints.size() > 0)
//    {
//        if (hasGT)
//            resultFile << ", ground-truth-cell-label";
//        else
//            resultFile << ", _#NO_CLASS#_";
//    }
//
//    resultFile << endl;
//
//    // iterate all cells
//
//    for (unsigned i = 0; i<(listCells.size()-1); i++)
//    {
//        resultFile << i << "," << i << "," << listCells[i]->getQtyPoints() << ",";
//
//        Point cm = listCells[i]->getCenterMass();
//
//        // Write center of mass coordinates on file
//        for (unsigned j = 0; j < cm.coord.size(); j++)
//            resultFile << cm.coord[j] << ",";
//
//        int cellClusterIdx;
//
//        if (g.getQtyAdjacents(i) <= minCells)
//            cellClusterIdx = -1;
//        else
//            cellClusterIdx = g.getClusterIndex(i);
//
//        resultFile  << clusters[g.getClusterIndex(i)]
//                    << "," << cellClusterIdx;
//
//        // Try to find out the closest raw pointer if there is rawPoints
//
//        if (rawPoints.size() > 0)
//        {
//            double minDist = cm.dist(rawPoints[0].rawPoint);
//            unsigned posMat = 0;
//            for (unsigned j = 1; j < rawPoints.size(); j++)
//            {
//                double curDist = cm.dist(rawPoints[j].rawPoint);
//                if (curDist < minDist)
//                {
//                    minDist = curDist;
//                    posMat = j;
//                }
//            }
//
//            // Found the ground truth for the current cell
//            if (cellClusterIdx >= 0) // is not -1
//            {
//                int idxGC = cellClusterIdx;   // Index of gCluster on the matrix
//                int idxGT = -1;             // Index of Ground Truth on the matrix
//                // Try to find out GT on matrix of cluster Found
//                for (unsigned j = 0; j < mapClustersCells[idxGC].size(); j++ )
//                {
//                    if (mapClustersCells[idxGC][j][0] == rawPoints[posMat].classGT)
//                        idxGT = j;
//                }
//
//                // If don't find Ground Truth, create a new matrix
//                if (idxGT == -1)
//                {
//                    // Create a new Ground Truth on Map
//                    vector<int> matAux(2);
//                    matAux[0] = rawPoints[posMat].classGT;
//                    matAux[1] = 1;
//                    mapClustersCells[idxGC].push_back(matAux);
//                }
//                else
//                {
//                    mapClustersCells[idxGC][idxGT][1]++;
//                }
//            }
//
//            listCells[i]->cellGT = rawPoints[posMat].classGT;
//            resultFile << "," << rawPoints[posMat].classGT;
//
//        }
//        resultFile << endl;
//    }
//
//    resultFile.close();
//
//    if (rawPoints.size() > 0)
//    {
//        // Open map result
//        mapFile.open(outputMapCells, std::ifstream::out);
//
//        for (unsigned i = 0; i < clusters.size(); i++)
//        {
//            int maxPts = 0;
//            int labelGT = -1;
//            if (clusters[i] >= minCells)
//            {
//                // Iterate map for cluster i
//                for (unsigned j = 0; j < mapClustersCells[i].size(); j++)
//                {
//                    if (mapClustersCells[i][j][1] > maxPts)
//                    {
//                        maxPts = mapClustersCells[i][j][1];
//                        labelGT = mapClustersCells[i][j][0];
//                    }
//                }
//                mapFile <<  i << "," <<  labelGT << endl;
//            }
//        }
//        mapFile.close();
//    }
//
    exit(EXIT_SUCCESS);

}
