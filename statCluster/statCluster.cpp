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
 * Output files: -1 <file> point gCluster with GT
 *               -2 <file> cells gCluster with GT
 *               -3 <file> cells DBSCAN with GT
 *
 * Validation files: -v <file>  points file with only coordinates, gCluster Label and ground truth
 *                   -w <file>  cells file with only cm coordinates, gCluster Label and ground truth
 *
 * Epsilon       -e <epsilon>
 *
 */
#include "statCluster.h"

#define PRINT_DEBUG 0

int main(int argc, char* argv[])
{
    int opt; // command line options
/*
 *     Files
 */
    ifstream inPoints; // input file - point
    ifstream inCells;  // input file - cell
    ifstream inCellsDB;  // input file - cell DBSCAN

    ofstream outPoints;
    ofstream outCells;
    ofstream outCellsDB;

    ofstream outValidPoints;
    ofstream outValidCells;

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

    string outputValidPoint = "";
    string outputValidPointDefault = "valid-point-gC.csv";
    string outputValidCells = "";
    string outputValidCellDefault = "valid-cell-gC.csv";

    string outputCellsDB = "";
    string outputCellsDBDefault = "map-cell-dbscan.csv";

    unsigned epsilon;

    cout << "gCluster algorithm - Statistic module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/gCluster" << endl;
    cout << "========================================================================" << endl;

    // Initialize options

    epsilon = 0;

    outputPoint = outputPointDefault;
    outputCell = outputCellDefault;
    outputValidPoint = outputValidPointDefault;
    outputValidCells = outputValidCellDefault;
    outputCellsDB = outputCellsDBDefault;

    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "e:p:c:d:b:v:w:1:2:3:h")) != -1)
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
            outputCell = optarg;
            break;
        case 'v':
            outputValidPoint = optarg;
            break;
        case 'w':
            outputValidCells = optarg;
            break;
        case '3':
            outputCellsDB = optarg;
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
            cout << "-3 <file>\toutput cells DBSCAN file" << endl;
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
    cout << "Output Cells DBSCAN.: " << outputCellsDB << endl;
    cout << endl <<"Running" << endl;
    cout << "-------" << endl;

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
    inCells.open(inputCell.c_str());
    if (!inCells)
    {
        cerr << "Error: Unable to open input cell file: " << inputCell << endl;
        exit(EXIT_FAILURE);
    }

    int r = 0;

    if (!getCSVLine(inCells, headerCellCSV))
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

    while (inCells)
    {
        r = getCSVLine(inCells, lineCSV);
        ++lineNo;
        if (!r)
        {
            cerr << "Error reading input cell file (Data) on Line " << lineNo + 2  << "\n";
            exit(EXIT_FAILURE);
        }
        if (lineCSV.size() < noFields)
        {
            cerr << "Error reading input cell file (Data) on Line " << lineNo + 2 << ": number of "
                 << "fields less then expected" << endl;
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

        vector<int> adjAux;
        int adjCellId;
        for (unsigned k = resultgC_posAdjacent(); k < lineCSV.size(); k++ )
        {
            adjCellId = atoi(lineCSV[k].c_str());
            adjAux.push_back(adjCellId);
        }

        // Create a new cell read from file
        newCell.setCenterMass(cm);
        newCell.setLabelgC(atoi(lineCSV[resultgC_posGCLabel()].c_str())); // Take last position of CSV
        newCell.setCellId(atoi(lineCSV[resultgC_posCellId()].c_str()));
        newCell.setQtyPoints((unsigned long)atoi(lineCSV[resultgC_posQtyPts()].c_str()));
        newCell.setLabelGT(-1);
        newCell.setAdjacent(adjAux);

        vector<int> cellCoord = getCellCoord(cm, epsilon);

        newCell.coord = getCellCoord(cm, epsilon);

        listCells.push_back(newCell);

        lineCSV.resize(0);
    }

    inCells.close();

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
    inPoints.open(inputPoint.c_str());
    if (!inPoints)
    {
        cerr << "Error: Unable to open input point file: "<< inputPoint << endl;
        exit(EXIT_FAILURE);
    }

    r = 0;

    if (!getCSVLine(inPoints, headerPointCSV))
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

    while (inPoints)
    {
        r = getCSVLine(inPoints, lineCSV);
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
        newPoint.classGT = atoi(lineCSV[consolidPts_posGT()].c_str());

        vector<int> cellCoord = getCellCoord(p, epsilon);

        for (unsigned i=0; i<listCells.size(); i++) // Search cells to find out cluster label on cells
        {
            // Compare listCells and current cell

            if (cellCoord == listCells[i].coord) // all coordinates are equal
            {
                newPoint.classAlgo = listCells[i].getLabelgC();
                newPoint.cellId = listCells[i].getCellId();
                // Need to update cell with ground truth to find out the ground truth of cell
                listCells[i].insertLabelGT(newPoint.classGT);
                break;
            }
        }

        listPoints.push_back(newPoint);

        lineCSV.resize(0);
    }

    inPoints.close();


 /********************************************************
 *
 *  3. Create maps to make a correspondence between labels of gCluster e Ground Truths
 *
 *  mapClusters[idxGC][idxGT][0..1] where idxGC = 0 to qty Clusters found - 1
 *                                        idxGT = 0 to qty of GT found on idxGC - 1
 *
 * mapClusters[idxGC][idxGT][0] = label Ground Truth
 * mapClusters[idxGC][idxGT][1] = Qty of occurrences of label Ground Truth on label gCluster
 ********************************************************/

    MapClusters mapAux;
    vector<MapClusters>  mapCells;
    vector<MapClusters>  mapPoints;

    // First let's verify list cells
    vector<int> aux(2); // Initial vector

    vector<vector <int> > finalMap;

    for (Cell c : listCells)
    {
        int labgC = c.getLabelgC(); // get the label found by gCluster
        int labGT = c.getLabelGT(); // get the Ground Truth label

        aux[0] = labGT;
        aux[1] = 1;

        int idxgC = -1; // try to find out the labels on the map
        int idxGT = -1; //

        for (unsigned i = 0; i < mapCells.size(); i++ )
        {
            if (mapCells[i].cGLabel == labgC)
            {
                idxgC = i;
                for (unsigned j = 0; j < mapCells[i].mapGT.size(); j++) // search GT label
                {
                    if(mapCells[i].mapGT[j][0] == labGT) // Found gCluster and GT, just increase
                    {
                        idxGT = j; // Indicate that found GT
                        mapCells[i].mapGT[j][1]++;
                        break;
                    }
                }
                if (idxGT == -1) // Didn't find Ground Truth label
                {
                    mapCells[i].mapGT.push_back(aux);
                    break;
                }
                break;
            }
        }
        if (idxgC == -1) // Didn't find gCluster label
        {
            mapAux.cGLabel = labgC;
            mapAux.mapGT.push_back(aux);
            mapCells.push_back(mapAux);
        }
    }

    for (MapClusters m : mapCells)
    {
        int maxGT = 0;
        int labelMaxGT = -1;
        for (vector<int> i : m.mapGT)
        {
            if (i[1] > maxGT)
            {
                maxGT = i[1];
                labelMaxGT = i[0];
            }
        }

        vector<int> nAux = {m.cGLabel, labelMaxGT};
        finalMap.push_back(nAux);
    }

#if PRINT_DEBUG
    cout << "again...." << endl;
    for (vector<int> m : finalMap)
    {
        cout << m[0] << "," << m[1] << endl;
    }
#endif // PRINT_DEBUG

/*********************************************************
 *
 * 4. Store listCells and listPoints on the disk (map and valid files)
 *
 * Read points and look at listCells to find out the cell
 * it belongs to.
 *
 *
 ********************************************************/

 /*********************************************************
 *
 * 4.1 Write Points
 *
 ********************************************************/
   cout << "Writing point output file: " << outputPoint << endl;
    cout << "Writing point validation file : " << outputValidPoint << endl;

    outPoints.open(outputPoint, std::ifstream::out);
    if (!outPoints.is_open())
    {
        cerr << "Could not open output file:" << outputPoint << endl;
        exit (EXIT_FAILURE);
    }

    outValidPoints.open(outputValidPoint, std::ifstream::out);
    if (!outValidPoints.is_open())
    {
        cerr << "Could not open valide point output file:" << outputValidPoint << endl;
        exit (EXIT_FAILURE);
    }

    // Write header

    for (unsigned i = 0; i < dimension; i++)
    {
        outPoints << headerPointCSV[consolidPts_posDimension()+i] << ",";
        outValidPoints << headerPointCSV[consolidPts_posDimension()+i] << ",";
    }

    outPoints << "cell-id,label-gC,label-GT" << endl;
    outValidPoints << "label-gC,label-GT" << endl;

    for (rawPoint line : listPoints)
    {
        for (unsigned i = 0; i < dimension; i++)
        {
            outPoints << line.rawData.coord[consolidPts_posDimension()+i] << ",";
            outValidPoints << line.rawData.coord[consolidPts_posDimension()+i] << ",";
        }

        int modLabel = line.classAlgo;
        for (vector<int> m : finalMap)
        {
            if (m[0] == modLabel)
            {
                modLabel = m[1];
                break;
            }
        }
        outPoints << line.cellId << "," << modLabel << "," << line.classGT << endl;
        outValidPoints << modLabel << "," << line.classGT << endl;
    }

    outPoints.close();
    outValidPoints.close();

 /*********************************************************
 *
 * 4.1 Write Cells
 *
 ********************************************************/

    cout << "Writing cell output file: " << outputCell << endl;
    cout << "Writing cell validation input file: " << outputValidCells << endl;
    cout << "Writing cell DBSCAN input file: " << outputCellsDB << endl;

    outCells.open(outputCell, std::ifstream::out);

    if (!outCells.is_open())
    {
        cerr << "Could not open output file:" << outputCell << endl;
        exit (EXIT_FAILURE);
    }

    outValidCells.open(outputValidCells, std::ifstream::out);
    if (!outValidCells.is_open())
    {
        cerr << "Could not open output valid cells file:" << outputValidCells << endl;
        exit (EXIT_FAILURE);
    }

    outCellsDB.open(outputCellsDB, std::ifstream::out);
    if (!outValidCells.is_open())
    {
        cerr << "Could not open output DBSCAN cells file:" << outputCellsDB << endl;
        exit (EXIT_FAILURE);
    }


    // Write header

    for (unsigned i = 0; i < dimension; i++)
    {
        outCells << headerCellCSV[resultgC_posDimension()+i] << ",";        outValidCells << headerCellCSV[resultgC_posDimension()+i] << ",";
        outCellsDB << headerCellCSV[resultgC_posDimension()+i] << ",";
    }

    outCells << "cell-id,label-gC,label-GT" << endl;
    outValidCells << "label-gC,label-GT" << endl;
    outCellsDB << "label-gC,label-GT" << endl;

    for (Cell c : listCells)
    {
        for (unsigned i = 0; i < dimension; i++)
        {
            outCells << c.getCenterMass().coord[resultgC_posDimension()+i] << ",";
            outValidCells << c.getCenterMass().coord[resultgC_posDimension()+i] << ",";
        }

        int modLabel = c.getLabelgC();
        for (vector<int> m : finalMap)
        {
            if (m[0] == modLabel)
            {
                modLabel = m[1];
                break;
            }
        }

        outCells << c.getCellId()
            << "," << modLabel
            << "," << c.getLabelGT();

        outValidCells << c.getCellId()
            << "," << modLabel
            << "," << c.getLabelGT();

        for (int a : c.getAdjacent())
            outCells << "," << a;

        outCells << endl;
        outValidCells << endl;

        // Write qty points of cells to DBSCAN
        for (unsigned j = 0; j < c.getQtyPoints(); j++)
        {
            for (unsigned i = 0; i < dimension; i++)
                outCellsDB << c.getCenterMass().coord[resultgC_posDimension()+i] << ",";

            outCellsDB << c.getCellId()
                << "," << modLabel
                << "," << c.getLabelGT() << endl;
        }

    }

    outCells.close();
    outValidCells.close();
    outCellsDB.close();

    exit(EXIT_SUCCESS);

}
