#
# This script run programs on all phases of process
#
# Run sumData processing all csv files inside <main>/devices directory where <main> is the
#       directory described by -d option
#
# The output cells files and point files (if -p option be present) will be stored on
#       <main>/central/devicesCells directory
#
# The cells and point files gathering from devices will be consolidate in the files allCells.csv and allPoints.csv on
#       <main>/central/consolidate directory
#
# Run getCluster to process cells and write all output on <main>/central/results
#
#
import sys
import os
import time
import validation
from subprocess import call

def cleanDirectory(dirName):
    if os.path.exists(dirName):
        # Remove and create directories
        for i in os.listdir(dirName):
            os.remove(os.path.join(dirName, i))
        os.removedirs(dirName)
    os.makedirs(dirName)


def isfloat(value):
    try:
        float(value)
        return True
    except ValueError:
        return False


def showError(msgErr):
    print("Error on script")
    print("---------------")
    print(msgErr)
    fLog.write("Error on script "+msgErr + "\n")
    exit(1)

def writeLog(msgLog):
    fLog.write(time.strftime('%d/%m/%Y %a %H:%M:%S') + ":" + msgLog)
    return;

def writeResultsHeader():
    fRes.write("time-stamp,epsilon,minForce,minCells,FM-points,FM-cells,notes" + "\n")

def writeResults(epsilon, force, cells, fmP, rand, jac, obs):
    fRes.write(time.strftime('%d/%m/%Y %a %H:%M:%S'))
    msg = ",%03d,%6.4f,%03d,%07.5f,%07.5f,%07.5f,%s\n" % (epsilon, force, cells, fmP, rand, jac, obs)
    fRes.write(msg)
    return;


def showHeader():
    print("gCluster algorithm - Script to all phases")
    print("Developed by Ricardo Brandao: https://github.com/programonauta/grid-clustering")
    print("------------------------------------------------------------------------------")


def showHelp():
    print("\tOptions\t\tDescription")
    print("\t-h\t\tShow this help")
    print("\t-d <dir>\tThe data files gathering by devices will be found on <dir>/devices directory")
    print("\t-e <epsilon>\tValue of epsilon: default = 10")
    print("\t-m <cells>\tMinimum Cells (default: 3)")
    print("\t-f <force>\tMinimum Force (default: 150)")
    print("\t-p Run with points")
    print("\t-n Draw Numbers")
    print("\t-x\t\tDon't validate")
    return


def parseOpt(opt, hasArgument):
    # verify if option is on the arg list
    mat = [i for i, x in enumerate(sys.argv) if x == opt]
    q = len(mat)

    if q == 0:
        return False, ""
    elif q > 1:
        showError("there is more than one " + opt + " option")
    else:
        ind = mat[0]
        arg = ""
        if hasArgument:
            if len(sys.argv) == ind + 1:
                arg = ""
            else:
                arg = sys.argv[ind + 1]
        return True, arg


# Print the header of script
showHeader()

# verify if have any -h option
hasHelp, opt = parseOpt("-h", False)

if (hasHelp > 0):
    showHelp()
    exit(1)

hasNumbers, opt = parseOpt("-n", False)

if hasNumbers:
    optNumbers = "-b"
else:
    optNumbers = ""

# verify if directory is defined
hasDir, nameDir = parseOpt("-d", True)

# replace "\\" by "/". In windows machines uses "\" for subdirectories. Python could handle with / in all OSs.
nameDir = nameDir.replace("\\", "/")

if hasDir:
    if nameDir == "":
        showError("Directory is not informed")
    if not os.path.isdir(nameDir):
        showError(nameDir + " is not a Directory")
    dirInput = nameDir + "/devices/"
    if not os.path.isdir(dirInput):
        showError(dirInput + " subdirectory not found on ")
else:
    showHelp()
    showError("-d option not found")

# verify Epsilon
hasEps, epsilon = parseOpt("-e", True)

if epsilon == "":
    epsilon = 10
else:
    if not epsilon.isdigit():
        showError("Epsilon: " + epsilon + " is not a valid integer")
    epsilon = int(epsilon)
    if epsilon <= 0:
        showError("Invalid value for epsilon:" + str(epsilon))

# verify MinCells
hasMinCell, minCell = parseOpt("-m", True)

if minCell == "":
    minCell = 3
else:
    if not minCell.isdigit():
        showError("Minimum number of Cells " + minCell + " is not a valid integer")
    minCell = int(minCell)
    if minCell <= 0:
        showError("Invalid value for minCell:" + str(minCell))

# verify force
hasForce, force = parseOpt("-f", True)

if force == "":
    force = 0.2
else:
    if not isfloat(force):
        showError("Force " + force + " is not a valid value")
    force = float(force)
    if force < 0:
        showError("Invalid value for force:" + str(force))

# Verify if validate
hasNotValid, opt = parseOpt("-x", False)

# Verify if don't draw edge
optEdge, opt = parseOpt("-g", False)
if optEdge:
    optEdge = "-g"
else:
    optEdge = ""

# Verify if draw points
optPoint, opt = parseOpt("-p", False)
if optPoint:
    optPoint = "-p"
else:
    optPoint = ""

#hasTimeStamp, opt = parseOpt("-t", False)

# Starting process
#
# First, let's verify if output directories are created and create them is not exist

nameDirAux = nameDir.split('/')
nameSingleDir = nameDirAux[len(nameDirAux) - 1]

dirSumDataOutput = nameDir + "/central/1.sumDataOutput"
dirConsolidateOutput = nameDir + "/central/2.deviceConsolidation"
dirGetClusterOutput = nameDir + "/central/3.getClusterOutput"
dirMapClusterOutput = nameDir + "/central/4.mapClusterOutput"
dirDrawOuput = nameDir + "/central/5.drawOutput"

logFile = nameDir + "/" + nameSingleDir + ".log"
logResults = nameDir + "/" + nameSingleDir + "-results.csv"

cleanDirectory(dirSumDataOutput)
cleanDirectory(dirConsolidateOutput)
cleanDirectory(dirGetClusterOutput)
cleanDirectory(dirMapClusterOutput)
cleanDirectory(dirDrawOuput)

configFile = nameDir + "/config/config-" + nameSingleDir + ".csv"
mapDirectory = nameDir + "/config"

qtyFiles = 0
listCellsFiles = []
listPointFiles = []

hasFile = os.path.isfile(logResults)

fLog = open(logFile, "a")
fRes = open(logResults, "a")

if not hasFile:
    writeResultsHeader()

writeLog("/------ Begin of Script ------" + "\n")
writeLog("Directory: " + nameDir + "\n")
writeLog("   Config File: " + configFile + "\n")
writeLog("   Epsilon..: " + str(epsilon) + "\n")
writeLog("   sumData" + "\n")

for file in os.listdir(dirInput):
    if file.endswith(".csv"):
        qtyFiles += 1
        completeFileName = dirInput + file
        strN = "%02d" % qtyFiles
        # Deal Cell files
        cellDataOutput = dirSumDataOutput + "/cell-" + nameSingleDir  + "-" + strN + ".csv"
        listCellsFiles.append(cellDataOutput)
        # Deal point files

        writeLog("   File Input:......" + completeFileName + "\n")
        writeLog("   File Cell Output.:" + cellDataOutput + "\n")

        if (optPoint == ""):
            pointConsOutput = ""
        else:
            pointConsOutput = dirSumDataOutput + "/point-" + nameSingleDir + "-" + strN + ".csv"
            writeLog("   File Point Output:" + pointConsOutput + "\n")
            listPointFiles.append(pointConsOutput)

        result = call(["../sumData/bin/sumData",
                       "-e", str(epsilon),
                       "-i", completeFileName,
                       "-c", cellDataOutput,
                       "-x", configFile,
                       optPoint, pointConsOutput])
        if result > 0:
            showError("sumData: Script ended with error number: " + str(result) + \
                      "\nProcessing file " + completeFileName)

if qtyFiles == 0:
    showError("There is no csv files on directory " + dirInput)

cellConsOutput = dirConsolidateOutput + "/cells-" + nameSingleDir + ".csv"
pointConsOutput = dirConsolidateOutput + "/points-" + nameSingleDir + ".csv"

# Open cell output to write on it
fileCellConsOutput = open(cellConsOutput, "w")

for i in range(len(listCellsFiles)):
    fInd = open(listCellsFiles[i], "r")  # open first cell
    firstLine = True
    for line in fInd:
        lineWr = line
        if firstLine and i > 0: # test if Header
            lineWr = ""

        if len(lineWr) > 0:
            fileCellConsOutput.write(lineWr)

        firstLine = False

    fInd.close()

fileCellConsOutput.close()

writeLog("Cells consolidation to file: " + cellConsOutput + " OK"  + "\n")

# Open point output to write on it
if len(listPointFiles) > 0:
    filePointConsOutput = open(pointConsOutput, "w")

for i in range(len(listPointFiles)):
    fInd = open(listPointFiles[i], "r")  # open first point
    firstLine = True
    for line in fInd:
        lineWr = line
        if firstLine and i > 0:  # test if Header
            lineWr = ""

        if len(lineWr) > 0:
            filePointConsOutput.write(lineWr)
        firstLine = False
    fInd.close()

if len(listPointFiles) > 0:
    filePointConsOutput.close()
    writeLog("Points consolidation to file: " + pointConsOutput + " OK" + "\n")

# Run getCluster

svgOutput = dirGetClusterOutput + "/graph-" + nameSingleDir + ".svg"
fileGetClusterOutput = dirGetClusterOutput + "/gCluster-result-" + nameSingleDir + ".csv"

writeLog("getCluster" + "\n")
writeLog("   Epsilon..: "+str(epsilon) + "\n")
writeLog("   Min Force: "+str(force) + "\n")
writeLog("   Min Cells: "+str(minCell) + "\n")
writeLog("   Input....: " + cellConsOutput + "\n")
writeLog("   Output...: " + fileGetClusterOutput + "\n")

result = call(["../getCluster/bin/getCluster",
               "-e", str(epsilon),
               "-m", str(minCell),
               "-f", str(force),
               "-i", cellConsOutput,
               "-o", fileGetClusterOutput, ])

if (optPoint == "-p"):

    fileOutputLabeledCells = dirMapClusterOutput + "/labeled-cells-" + nameSingleDir + ".csv"
    fileOutputLabeledPoints = dirMapClusterOutput + "/labeled-points-" + nameSingleDir + ".csv"
    fileOutputExpandedPoints = dirMapClusterOutput + "/expanded-points-" + nameSingleDir + ".csv"

    writeLog("mapCluster \n")
    writeLog("   Epsilon............: " + str(epsilon) + "\n")
    writeLog("   Input Cell.........: " + fileGetClusterOutput + "\n")
    writeLog("   Input Point........: " + pointConsOutput + "\n")
    writeLog("   Labeled Cells .....: " + fileOutputLabeledCells + "\n")
    writeLog("   Labeled Points ....: " + fileOutputLabeledPoints + "\n")
    writeLog("   Expanded Points....: " + fileOutputExpandedPoints + "\n")

    result = call(["../mapCluster/bin/mapCluster",
                   "-e", str(epsilon),
                   "-p", pointConsOutput,
                   "-c", fileGetClusterOutput,
                   "-m", fileOutputLabeledCells,
                   "-v", fileOutputLabeledPoints,
                   "-x", fileOutputExpandedPoints])

    if result > 0:
        showError("Error on mapCluster. Input file:", cellConsOutput)

    if not hasNotValid:
        print("Validating Points. Input File: " + fileOutputLabeledPoints)
        writeLog("Results-------\n")
        writeLog("   Input File" + fileOutputLabeledPoints + "\n")

        r, FMPoints, RandPoints, JacPoints = validation.validation(fileOutputLabeledPoints);
        if r:
            writeLog("   FM for gCluster Points..: " + str(FMPoints) + "\n")
            writeLog("   Rand for gCluster Points..: " + str(RandPoints) + "\n")
            writeLog("   Jac for gCluster Points..: " + str(JacPoints) + "\n")
        else:
            writeLog("Some problem on validation process\n")

        strPoints = " FM Points: %07.5f" % FMPoints

    else:
        writeLog("Choose not run validation process\n")
        strPoints = ""

    fileSVGOutput = dirDrawOuput + "/graph-" + nameSingleDir + ".svg"

    writeLog("drawCluster \n")
    writeLog("   Epsilon.......: " + str(epsilon) + "\n")
    writeLog("   Input Cell....: " + fileOutputLabeledCells + "\n")
    writeLog("   Input Point...: " + fileOutputLabeledPoints + "\n")
    writeLog("   Output SVG....: " + fileSVGOutput + "\n")
    if (hasNumbers):
        writeLog("    Draw Numbers..: Yes\n")
    else:
        writeLog("    Draw Numbers..: No\n")


    titleSVG = "MinForce: " + str(force) + " minCells: " + str(minCell) + strPoints

    result = call(["../drawCluster/bin/drawCluster",
                   "-e", str(epsilon),
                   "-p", fileOutputLabeledPoints,
                   "-c", fileOutputLabeledCells,
                   "-s", fileSVGOutput,
                   optNumbers,
                   "-t", titleSVG])

else:
    writeLog("mapCluster and drawCluster: didn't execute. Don't have -p option")

comment = input("Enter a comment about this experiment: ")

if comment != "":
    writeLog("Comment: " + comment+"\n")

if optPoint == "-p":
    writeResults(epsilon,force,minCell,FMPoints,RandPoints, JacPoints, comment)
else:
    writeResults(epsilon,force,minCell,0,comment)

writeLog("------ End of Script ------/" + "\n")

fLog.close()
fRes.close()