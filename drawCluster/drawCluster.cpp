#include "drawCluster.h"

#define PRINT_DEBUG 0

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
                      unsigned cellID, int cellgC, int cellGT)
{
    stringstream svgText;

    string color;

    if (cellGT > -2)
        color = (cellgC==cellGT?"green":"red");
    else
        color = "black";

    double xSVG = graphMult * (double)x/epsilon + graphMult*.1;
    double ySVG =  graphMult*1.1 - graphMult * ((double)(y-1)/epsilon + (1./epsilon));

    svgText << "<text "
            << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.55)<< "cm\" "
            << "font-style=\"normal\" "
            << "font-family=\"Times New Roman\" "
            << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
            << "stroke=\"none\" fill=\"" << color << "\" > "
            << "Cell:" << cellID
            << "</text>" << endl;

    svgText << "<text "
            << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
            << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.30)<< "cm\" "
            << "font-style=\"normal\" "
            << "font-family=\"Times New Roman\" "
            << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
            << "stroke=\"none\" fill=\"" << color << "\" > "
            << "Cl:" << cellgC
            << "</text>" << endl;

    if (cellGT > -2)
    {
        svgText << "<text "
                << "x=\"" << xSVG + (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
                << "y=\"" << ySVG - (graphMult * (1/(double)epsilon) * 0.05)<< "cm\" "
                << "font-style=\"normal\" "
                << "font-family=\"Times New Roman\" "
                << "font-size=\"" << graphMult * (1/(double)epsilon) * 0.25 <<"cm\" "
                << "stroke=\"none\" fill=\"" << color << "\" > "
                << "GT:" << cellGT
                << "</text>" << endl;
    }
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
    // Files

    ifstream inPoints;
    ifstream inMapPoints;
    ifstream inCells;  // input file - cell

    ofstream svgFile;

    // Variables to deal with files
    int lineNo = 0;
    unsigned dimension;
    unsigned noFields;  // number of fields
    vector<string> lineCSV;
    vector<string> headerPointCSV;
    vector<string> headerCellCSV;

    vector<Cell> listCells; // Vector of Cells
    vector<rawPoint> listPoints; // Vector of Points

    int opt; // command line options

    // Parameters and theirs defaults
    string inputPoints = "";
    string inputCells = "";

    string svgOutput = "";
    string svgOutputDefault = "plot.svg";

    // Initialize draw options
    bool drawRects = true;
    bool drawEdges = true;
    bool drawPoints = false;
    bool drawNumbers = false;
    bool drawCells = false;

    string svgTitle = "";

    unsigned epsilon;

    cout << "gCluster algorithm - draw module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
    cout << "===============================================================================\n" << endl;

    // Initialize options

    epsilon = 0;

    svgOutput = svgOutputDefault;


    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "p:c:s:e:t:rgbh")) != -1)
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
            inputPoints = optarg;
            break;
        case 'c':
            inputCells = optarg;
            break;
        case 's':
            svgOutput = optarg;
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
        case 't':
            svgTitle = optarg;
            break;
        case 'h':
        default: /* '?' */
            cout << "OVERVIEW: Clustering data using gCluster algorithm" << endl;
            cout << "          The program output an SVG file with the two first dimensions" << endl;
            cout << endl;
            cout << "USAGE: drawCluster <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-e <value>\tEpsilon (required)" << endl;
            cout << "-p <file>\tinput points file (required)" << endl;
            cout << "-c <file>\tinput cells file (required)" << endl;
            cout << "-s <file>\tSVG output file (" << svgOutputDefault << ")" << endl;
            cout << "-r don't draw rectangles" << endl;
            cout << "-g don't draw edges" << endl;
            cout << "-b print numbers" << endl;
            cout << "-t <title> Title of graph" << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (epsilon == 0)
    {
        cerr << "Epsilon is required, please inform it on -e option" << endl;
        exit (EXIT_FAILURE);
    }

    if (inputPoints == "" && inputCells == "")
    {
        cerr << "Input Point File or Input Cell file are required" << endl;
        exit (EXIT_FAILURE);
    }

    drawPoints = (inputPoints.size() > 0);
    drawCells = (inputCells.size() > 0);

    cout << " Parameters" << endl;
    cout << "---------------------" << endl;
    cout << "Epsilon.............: " << epsilon << endl;
    cout << "Input Points........: " << inputPoints << endl;
    cout << "Input Cells.........: " << inputCells << endl;
    cout << "SVG file............: " << svgOutput << endl;
    cout << "Draw rects (cells)..: " << (drawRects?"Yes":"No") << endl;
    cout << "Draw edges .........: " << (drawEdges?"Yes":"No") << endl;
    cout << "Draw points ........: " << (drawPoints?"Yes":"No") << endl;
    cout << "Draw numbers........: " << (drawNumbers?"Yes":"No") << endl;
    cout << "Title...............: " << svgTitle  << endl;

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

    if (drawCells)
    {

        inCells.open(inputCells.c_str());
        if (!inCells)
        {
            cerr << "Error: Unable to open input cell file: " << inputCells << endl;
            exit(EXIT_FAILURE);
        }

        if (!getCSVLine(inCells, headerCellCSV))
        {
            cerr << "Error reading cell file (Header) on the first line: " << inputCells << endl;
            exit(EXIT_FAILURE);
        }

        noFields = headerCellCSV.size(); // Define number of fields

        cout << "Reading cell file header. Number of Fields:" << noFields << endl;

        for (unsigned i = 0; i < headerCellCSV.size(); i++)
            cout << (i?",": "") << headerCellCSV[i];

        cout << endl;

        // The header has dimensions plus 4 fields (cell-id, number-points, qty-cells, gCluster-label)
        dimension = headerCellCSV.size() - mapC_qtyFixedLabels;

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
                cm.coord[i] = stod(lineCSV[i+mapC_posDimension()]);

            vector<int> adjAux;
            int adjCellId;
            for (unsigned k = mapC_posAdjacent(); k < lineCSV.size(); k++ )
            {
                adjCellId = atoi(lineCSV[k].c_str());
                adjAux.push_back(adjCellId);
            }

            // Create a new cell read from file
            newCell.setCenterMass(cm);
            newCell.setLabelgC(atoi(lineCSV[mapC_posGCLabel()].c_str())); // Take last position of CSV
            newCell.setCellId(atoi(lineCSV[mapC_posCellId()].c_str()));
            newCell.setLabelGT(atoi(lineCSV[mapC_posGT()].c_str()));
            newCell.setAdjacent(adjAux);

            vector<int> cellCoord = getCellCoord(cm, epsilon);

            newCell.coord = getCellCoord(cm, epsilon);

#if PRINT_DEBUG
    Point xck = newCell.getCenterMass();
    cout << xck
        << "(";
    for (int xk : newCell.coord)
        cout << "," << xk;
    cout << ")" << newCell.getCellId()
        << "," << newCell.getLabelgC()
        << "," << newCell.getLabelGT() << endl ;
#endif // PRINT_DEBUG

            listCells.push_back(newCell);

            lineCSV.resize(0);
        }

        inCells.close();

    }
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

    if (drawPoints)
    {

        inPoints.open(inputPoints.c_str());
        if (!inPoints)
        {
            cerr << "Error: Unable to open input point file: "<< inputPoints << endl;
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
                    break;
                }
            }

            listPoints.push_back(newPoint);

            lineCSV.resize(0);
        }

        inPoints.close();

    }

/*********************************************************
 *
 * 3. Create the SVG File
 *
 ********************************************************/

    float graphMult = 50.0;

    cout << endl;
    cout << "------------------------" << endl;
    cout << " Creating SVG File: " << svgOutput << endl;
    cout << "------------------------" << endl;

    svgFile.open(svgOutput, std::ifstream::out);
    if (!svgFile.is_open())
    {
        cerr << "Could not open output file:" << svgOutput << endl;
        exit (EXIT_FAILURE);
    }

    // Header of SVG file
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
            << "File Name: " << svgOutput
            << "</text>" << endl;
    svgFile << "<text x=\"" << graphMult * 0.1 << "cm\" "
            << "y=\"" << graphMult * 1.18 << "cm\" "
            << "font-family=\"Times New Roman\" font-size=\""
            << graphMult * 0.03 << "cm\" fill=\"black\">"
            << "Epsilon: " << epsilon
            << " " << svgTitle
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

    vector<string> colors;
    colors.push_back("#FF7F00");
    colors.push_back("#bf870f");
    colors.push_back("#7F0000");
    colors.push_back("#2d60ad");
    colors.push_back("#44ef0b");
    colors.push_back("#0000FF");
    colors.push_back("#007F00");
    colors.push_back("#7F007F");
    colors.push_back("#ef5d09");
    colors.push_back("#FF0000");
    colors.push_back("#000077");
    colors.push_back("#007F7F");
    colors.push_back("#6eb4ea");
    colors.push_back("#7F7F00");
    colors.push_back("#00007F");
    colors.push_back("#5f397a");
    colors.push_back("#000000");
    colors.push_back("#007777");
    colors.push_back("#ffbf00");
    colors.push_back("#7676ed");
    colors.push_back("#007700");
    colors.push_back("#c10f6e");

    if (drawCells)
    {

        // Group Begin
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
        svgFile << "</g>" << endl;

        for (unsigned i = 0; i<listCells.size(); i++)
        {

            // Draw transparent rect
            string opacity;
            opacity = (!drawEdges?"0.7":"0.6");

            // iterate all adj cells
            for (unsigned cellAdj : listCells[i].getAdjacent())
            {
                // hasCells inform if there is adjacent cells, if label = -1 there is no adjacent
                bool hasCells = (listCells[cellAdj].getLabelgC() >-1);
                // verify if must drawRects, cell is not drawn yet and is a labeled cluster
                if (drawRects && !rectDrawn[cellAdj] && hasCells)
                {
                    // first print the current cell
                    svgFile << strSVGRect(opacity,
                                          graphMult,
                                          epsilon,
                                          colors[listCells[cellAdj].getLabelgC()%colors.size()],
                                          listCells[cellAdj].coord[0],
                                          listCells[cellAdj].coord[1]);
                    rectDrawn[cellAdj] = true;
                }

                if (drawEdges && !edgeDraw[i][cellAdj] && hasCells)
                {
                    svgFile << strSVGEdge(graphMult,
                                          colors[listCells[i].getLabelgC()%colors.size()],
                                          listCells[i].getCenterMass().coord[0],
                                          listCells[i].getCenterMass().coord[1],
                                          listCells[cellAdj].getCenterMass().coord[0],
                                          listCells[cellAdj].getCenterMass().coord[1]);
                    edgeDraw[i][cellAdj] = true;
                    edgeDraw[cellAdj][i] = true;
                }
            }

            // if strSVGTextCell receives -2 as GT label, it didn't print it
            if (drawNumbers)
                svgFile << strSVGTextCell(graphMult, epsilon,
                                          listCells[i].coord[0],
                                          listCells[i].coord[1],
                                          listCells[i].getCellId(),
                                          listCells[i].getLabelgC(),
                                          listCells[i].getLabelGT()) << endl;

        }

    }


    if (drawPoints)
    {
        for (rawPoint p : listPoints)
        {
            int labelGT = p.classGT;
            string colorPoint = "none";
            string opacity = "1.0";
            string width = "0.03";
            double radio = 0.002;

            if (labelGT >= 0)
            {
                colorPoint = colors[(labelGT+5)%colors.size()];
                opacity = "0.4";
                width = "0.05";
                radio = 0.006;
            }

            double xCirc = (double)(p.rawData.coord[0]*graphMult)+graphMult*.1;
            double yCirc = (double) graphMult*1.1-(float)(p.rawData.coord[1]*graphMult);
            svgFile << "<circle fill=\""<< colorPoint<< "\""
                    << " style=\"opacity:" << opacity << "\" "
                    << " stroke=\"black\" "
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
                        << "stroke=\"none\" fill=\""<< "black" <<"\" > "
                        << labelGT
                        << "</text>" << endl;
            }

        }

//        svgFile << "</g>" << endl;

    }

    svgFile << "</svg>";
    svgFile.close();

    exit(EXIT_SUCCESS);

}
