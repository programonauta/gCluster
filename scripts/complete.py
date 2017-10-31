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
from datetime import datetime
from subprocess import call


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
    exit(1)


def showHeader():
    print("Grid Clustering algorithm - Script to all phases")
    print("Developed by Ricardo Brandao: https://github.com/programonauta/grid-clustering")
    print("------------------------------------------------------------------------------")


def showHelp():
    print("\tOptions\t\tDescription")
    print("\t-h\t\t\tShow this help")
    print("\t-d <dir>\tThe data files gathering by devices will be found on <dir>/devices directory")
    print("\t-e <epsilon>\tValue of epsilon: default = 10")
    print("\t-m <cells>\tMinimum Cells (default: 3)")
    print("\t-f <force>\tMinimum Force (default: 150)")
    print("\t-r\t\tDon't draw rectangles")
    print("\t-g\t\tDon't draw edges")
    print("\t-p\t\tPrint points")
    print("\t-t\t\tUse timestamp on files")
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
matHelp = [i for i, x in enumerate(sys.argv) if x == "-h"]
qtyHelp = len(matHelp)

if (qtyHelp > 0):
    showHelp()
    exit(0)

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
    if force <= 0:
        showError("Invalid value for force:" + str(force))

# Verify is don't draw rectangle
optRect, opt = parseOpt("-r", False)
if optRect:
    optRect = "-r"
else:
    optRect = ""

# Verify is don't draw edge
optEdge, opt = parseOpt("-g", False)
if optEdge:
    optEdge = "-g"
else:
    optEdge = ""

# Verify is don't draw edge
optPoint, opt = parseOpt("-p", False)
if optPoint:
    optPoint = "-p"
else:
    optPoint = ""

hasTimeStamp, opt = parseOpt("-t", False)

# Starting process
#
# First, let's verify if output directories are created and create them is not exist

nameDirAux = nameDir.split('/')
nameSingleDir = nameDirAux[len(nameDirAux) - 1]

dirSumDataOutput = nameDir + "/central/devicesCells"
dirConsolidateOutput = nameDir + "/central/consolidate"
dirGetClusterOutput = nameDir + "/central/results"

if not os.path.exists(dirSumDataOutput):
    os.makedirs(dirSumDataOutput)
if not os.path.exists(dirConsolidateOutput):
    os.makedirs(dirConsolidateOutput)
if not os.path.exists(dirGetClusterOutput):
    os.makedirs(dirGetClusterOutput)

if hasTimeStamp:
    now = datetime.now()
    prefix = now.strftime('%Y%m%d-%H%M%S-')
else:
    prefix = ""

qtyFiles = 0
listCellsFiles = []
listPointFiles = []

for file in os.listdir(dirInput):
    if file.endswith(".csv"):
        qtyFiles += 1;
        completeFileName = dirInput + file
        strN = "%02d" % qtyFiles
        # Deal Cell files
        cellOutput = dirSumDataOutput + "/" + prefix + "cell-" + nameSingleDir  + "-" + strN + ".csv"
        listCellsFiles.append(cellOutput)
        # Deal point files
        if (optPoint == ""):
            pointOutput = ""
        else:
            pointOutput = dirSumDataOutput + "/" + prefix + "point-" + nameSingleDir + "-" + strN + ".csv"
            listPointFiles.append(pointOutput)
        result = call(["../sumData/bin/Debug/sumData",
                       "-e", str(epsilon),
                       "-i", completeFileName,
                       "-c", cellOutput,
                       optPoint, pointOutput])
        if result > 0:
            showError("Script ended with error number: " + str(result) + \
                      "\nProcessing file " + completeFileName)

if qtyFiles == 0:
    showError("There is no csv files on directory " + dirInput)

cellOutput = dirConsolidateOutput + "/" + prefix + "cells-" + nameSingleDir + ".csv"
pointOutput = dirConsolidateOutput + "/" + prefix + "points-" + nameSingleDir + ".csv"

# Open cell output to write on it
fileCellOutput = open(cellOutput, "w")

for i in range(len(listCellsFiles)):
    fInd = open(listCellsFiles[i], "r")  # open first cell
    firstLine = True
    for line in fInd:
        lineWr = line
        if firstLine and i > 0: # test if Header
            lineWr = ""

        if len(lineWr) > 0:
            fileCellOutput.write(lineWr)

        firstLine = False

    fInd.close()

fileCellOutput.close()

# Open cell output to write on it
if len(listPointFiles) > 0:
    filePointOutput = open(pointOutput, "w")

for i in range(len(listPointFiles)):
    fInd = open(listPointFiles[i], "r")  # open first cell
    firstLine = True
    for line in fInd:
        lineWr = line
        if firstLine and i > 0:  # test if Header
            lineWr = ""

        if len(lineWr) > 0:
            filePointOutput.write(lineWr)
        firstLine = False
    fInd.close()

if len(listPointFiles) > 0:
    filePointOutput.close()

# Run getCluster

svgOutput = dirGetClusterOutput + "/" + prefix + "graph-" + nameSingleDir + ".svg"

result = call(["../getCluster/bin/Debug/getCluster",
               "-e", str(epsilon),
               "-m", str(minCell),
               "-f", str(force),
               optRect,
               optEdge,
               optPoint, pointOutput,
               "-s", svgOutput,
               "-i", cellOutput])
