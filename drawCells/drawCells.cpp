#include "drawCells.h"

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
                      unsigned qtyPoints)
{
    stringstream svgText;

    string color;

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
            << qtyPoints
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

    unsigned maxPts = 0; // Variable to get max number of points, if provided, don't calculate numbers

    // Initialize draw options
    bool drawRects = true;
    bool drawPoints = false;
    bool drawNumbers = false;
    bool drawCells = false;
    bool maxPtsProvided = false;

    string svgTitle = "";

    unsigned epsilon;

    cout << "gCluster algorithm - draw cells module" << endl;
    cout << "developed by ricardo brandao - https://github.com/programonauta/grid-clustering" << endl;
    cout << "===============================================================================\n" << endl;

    // Initialize options

    epsilon = 0;

    svgOutput = svgOutputDefault;


    // Verify command line options
    //
    // Show help message if -h or there is no command options

    while ((opt = getopt(argc, argv, "p:c:s:e:x:t:rbh")) != -1)
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
        case 'x':
            maxPts = atoi(optarg);
            maxPtsProvided = true;
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
            cout << "USAGE: drawCells <options>" << endl;
            cout << endl;
            cout << "OPTIONS: " << endl;
            cout << "-e <value>\tEpsilon (required)" << endl;
            cout << "-p <file>\tinput points file - Consolidate" << endl;
            cout << "-c <file>\tinput cells file (required) - Output of getCluster file" << endl;
            cout << "-s <file>\tSVG output file (" << svgOutputDefault << ")" << endl;
            cout << "-r don't draw rectangles" << endl;
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

            // Create a new cell read from file
            newCell.setCenterMass(cm);
            newCell.setCellId(atoi(lineCSV[resultgC_posCellId()].c_str()));
            newCell.setQtyPoints(atoi(lineCSV[resultgC_posQtyPts()].c_str()));

            if (newCell.getQtyPoints() > maxPts and !maxPtsProvided)
                maxPts = newCell.getQtyPoints();

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

            if (lineCSV.size() < noFields)
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

    unsigned maxColor = 90;
    unsigned minColor = 0;

    if (drawCells)
    {

        // Group Begin
        svgFile << "<g stroke=\"black\" stroke-width=\"" <<
                graphMult * 0.09 / epsilon <<
                "cm\">" << endl;

        vector<bool> rectDrawn(listCells.size());

        for (unsigned i = 0; i < rectDrawn.size(); i++)
            rectDrawn[i] = false;

        svgFile << "</g>" << endl;

        for (unsigned i = 0; i<listCells.size(); i++)
        {

            // Draw transparent rect
            string opacity;
            opacity = ".9";

            unsigned qtyPts = listCells[i].getQtyPoints();
            if (qtyPts > maxPts)
                qtyPts = maxPts;

            unsigned color = (unsigned)((1-((double)qtyPts/(double)maxPts)) * (maxColor-minColor) + minColor);

            char buffColor[8];

            sprintf(buffColor, "#%0x%0x%0x", color, color, color);
            string strColor = buffColor;

            svgFile << strSVGRect(opacity,
                                  graphMult,
                                  epsilon,
                                  strColor,
                                  listCells[i].coord[0],
                                  listCells[i].coord[1]);

            // if strSVGTextCell receives -2 as GT label, it didn't print it
            if (drawNumbers)
                svgFile << strSVGTextCell(graphMult, epsilon,
                                          listCells[i].coord[0],
                                          listCells[i].coord[1],
                                          listCells[i].getQtyPoints()) << endl;

        }

    }

    if (drawPoints)
    {
        for (rawPoint p : listPoints)
        {
            string colorPoint = "black";
            string opacity = "1.0";
            string width = "0.05";
            double radius = 0.002;

            double xCirc = (double)(p.rawData.coord[0]*graphMult)+graphMult*.1;
            double yCirc = (double) graphMult*1.1-(float)(p.rawData.coord[1]*graphMult);
            svgFile << "<circle fill=\""<< colorPoint<< "\""
                    << " style=\"opacity:" << opacity << "\" "
                    << " stroke=\"black\" "
                    << " stroke-width=\""<< width << "cm\" "
                    << " cx=\"" << xCirc  << "cm\" "
                    << " cy=\"" << yCirc  << "cm\" "
                    << " r=\"" << graphMult * radius << "cm\""
                    << " />" << endl;
            if (drawNumbers)
            {
                svgFile << "<text "
                        << "x=\"" << xCirc - ( 0.4 * graphMult * radius) << "cm\" "
                        << "y=\"" << yCirc + ( 0.4 * graphMult * radius) << "cm\" "
                        << "font-style=\"normal\" "
                        << "font-family=\"Times New Roman\" "
                        << "font-size=\"" << graphMult * 0.005 <<"cm\" "
                        << "stroke=\"none\" fill=\""<< "black" <<"\" > "
                        << "</text>" << endl;
            }

        }

//        svgFile << "</g>" << endl;

    }

    svgFile << "</svg>";
    svgFile.close();

    exit(EXIT_SUCCESS);

}
