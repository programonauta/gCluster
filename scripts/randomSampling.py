#
# This script run programs that create samples from raw datasets
#
#
import sys
import os
import time
import validation
import csv
import math
import random
import numpy as np
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import StandardScaler
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
    if 'fLog' in vars():
        fLog.write("Error on script " + msgErr + "\n")
    exit(1)


def writeLog(msgLog):
    fLog.write(time.strftime('%d/%m/%Y %a %H:%M:%S') + ":" + msgLog)
    return;

def writeResultsHeader():
    fRes.write("time-stamp,experiments,epsilon,minPts,clusters,avg FM, stdDev" + "\n")

def writeResults(experiments, epsilon, minPts, FM, rand, jac, stdDev):
    fRes.write(time.strftime('%d/%m/%Y %a %H:%M:%S'))
    msg = ",%d,%6.4f,%03d,%07.5f,%07.5f\n" % (experiments, epsilon, minPts, FM, stdDev)
    fRes.write(msg)
    return;

def showHeader():
    print("Random Sampling algorithm")
    print("Developed by Ricardo Brandao: https://github.com/programonauta/grid-clustering")
    print("------------------------------------------------------------------------------")


def showHelp():
    print("\tOptions\t\tDescription")
    print("\t-h\t\tShow this help")
    print("\t-d <dir>\tThe data files gathering by devices will be found on <dir>/devices directory")
    print("\t-r <value>\tValue of reduction")
    print("\t-e <value>\tEpsilon value")
    print("\t-m <value>\tMin points value")
    print("\t-q <value>\tQty of experiments (default: 40)")
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


def dbFun(_x, _original_vals, epsilon, minPts, gt, outputFile):
    db = DBSCAN(eps=epsilon, min_samples=minPts).fit(_original_vals)  # for cells_eps = 100
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True

    labels = db.labels_
    # print(labels)

    # create a map label DBSCAN x label Ground Truth

    maps = []  # Initalize map vector
    finalMap = []

    j = 0
    for i in _original_vals:
        labelDB = labels[j]
        labelGT = int(gt[j])
        j += 1
        found = False
        for k in maps:
            if k[0] == labelDB and k[1] == labelGT:
                found = True
                k[2] += 1
                break;
        if not found:
            maps.append([labelDB, labelGT, 1])

    for label in set(labels):
        max = 0;
        labelGT = -2
        for k in maps:
            if k[0] == label:
                if k[2] > max:
                    max = k[2]
                    labelGT = k[1]
        if labelGT == -1 or label == -1:  # Don't let a lost cluster be mapped to -1
            finalMap.append([label, label, max])
        else:
            finalMap.append([label, labelGT, max])

    # sort Final Map

    # correct wrong relations

    correctMap = True

    while correctMap:
        finalMap = sorted(finalMap, key=lambda x: (x[1], -x[2]))
        mapToPrev = finalMap[0][1]
        correctMap = False
        for m in finalMap[1:]:
            if m[1] == mapToPrev:
                mapToPrev = m[1]
                if (m[1] != m[0]):
                    correctMap = True
                m[1] = m[0]  # mapTo receives mapFrom
            else:
                mapToPrev = m[1]

    # for m in finalMap:
    #     strMat = "   %d, %d, %d" % (m[0], m[1], m[2])
    #     writeLog(strMat)
    #     print(strMat)

    fOut = open(outputFile, "w")

    j = 0
    for i in _original_vals:
        # write line (x, y, label found by DBSCAN, ground truth
        # find correspondent label

        labelDB = labels[j]
        newLabel = labelDB
        for k in finalMap:
            if labelDB == k[0]:
                newLabel = k[1]
                break

        strCSV = str(i[0]) + "," + str(i[1]) + "," + str(labels[j]) + "," + str(newLabel) + "," + str(gt[j]) + "\n"
        j += 1
        fOut.write(strCSV)

    fOut.close()

    n_clusters_ = 0
    for c in set(labels):
        if c != -1:
            n_clusters_ += 1
    return n_clusters_


# Print the header of script
showHeader()

# verify if have any -h option
hasHelp, opt = parseOpt("-h", False)

if (hasHelp > 0):
    showHelp()
    exit(1)

# verify if directory is defined
hasDir, nameDir = parseOpt("-d", True)

# replace "\\" by "/". In windows machines uses "\" for subdirectories. Python could handle with / in all OSs.
nameDir = nameDir.replace("\\", "/")
nameDirAux = nameDir.split('/')
nameSingleDir = nameDirAux[len(nameDirAux) - 1]

logFile = nameDir + "/" + nameSingleDir + ".log"
logResults = nameDir + "/" + nameSingleDir + "-results.csv"

if hasDir:
    if nameDir == "":
        showError("Directory is not informed")
    if not os.path.isdir(nameDir):
        showError(nameDir + " is not a Directory")
    dirInput = nameDir + "/devices/"
    if not os.path.isdir(dirInput):
        showError(dirInput + " subdirectory not found.")
else:
    showHelp()
    showError("-d option not found")

# verify Reduction
hasReduction, reduction = parseOpt("-r", True)

if reduction == "":
    showError("Reduction not informed")
else:
    if not isfloat(reduction):
        showError("Reductoin " + reduction + " is not a valid value")
    reduction = float(reduction)
    if reduction <= 0 or reduction >= 1:
        showError("Invalid value for Reduction: " + str(reduction) + " must be between 0 and 1")

# verify Epsilon
hasEpsilon, epsilon = parseOpt("-e", True)

if epsilon == "":
    showError("Epsilon not informed")
else:
    if not isfloat(epsilon):
        showError("Epsilon " + epsilon + " is not a valid value")
    epsilon = float(epsilon)
    if epsilon <= 0:
        showError("Invalid value for Epsilon:" + str(epsilon))

# verify minPts
hasMinPts, minPts = parseOpt("-m", True)

if minPts == "":
    showError("minPts not informed")
else:
    minPts = int(minPts)
    if minPts <= 0:
        showError("Invalid value for minPts:" + str(minPts))

# verify qtyExperiments
hasQtyExp, qtyExperiments = parseOpt("-q", True)

if qtyExperiments == "":
    qtyExperiments = 40
else:
    qtyExperiments = int(qtyExperiments)
    if qtyExperiments <= 0:
        showError("Invalid value for quantity of Experiments:" + str(qtyExperiments))

dirOutput = nameDir + "/randomSampling"
inputFile = nameDir + "/central/2.deviceConsolidation/points-" + nameSingleDir + ".csv"
if not os.path.isfile(inputFile):
    showError(inputFile + ": file not found.")

cleanDirectory(dirOutput)

logFile = nameDir + "/" + nameSingleDir + "-randomSample.log"
logResults = nameDir + "/" + nameSingleDir + "-randomSample-results.csv"

hasFile = os.path.isfile(logResults)

fLog = open(logFile, "a")
fRes = open(logResults, "a")

if not hasFile:
    writeResultsHeader()

# Print the header of script
showHeader()

print("Input file     : ", inputFile)
print("Reduction      : ", reduction)
print("Epsilon        : ", epsilon)
print("MinPts         : ", minPts)
print("Qty Experiments: ", qtyExperiments)

# Starting process
#
qtyFiles = 0

writeLog("/------ Begin of Script Random Sampling------" + "\n")
writeLog("Directory: " + nameDir + "\n")
writeLog("   Input File....: " + inputFile + "\n")
writeLog("   Reduction.....: " + str(reduction) + "\n")
writeLog("   Epsilon.......: " + str(epsilon) + "\n")
writeLog("   minPts........: " + str(minPts) + "\n")
writeLog("   qtyExperiments: " + str(qtyExperiments) + "\n")

with open(inputFile, 'r') as f:
    reader = csv.reader(f)
    inputList = list(reader)

qtySamples = math.ceil(len(inputList) * (1 - reduction))

writeLog("Input file with " + str(len(inputList)) + " registers\n" )

print("Input file with " + str(len(inputList)) + " registers")

totalExp = []
for i in range(qtyExperiments):
    samples = []
    randomSamples = random.sample(range(1, len(inputList) - 1), qtySamples)
    for s in range(len(randomSamples)):
        samples.append(inputList[randomSamples[s]])
    totalExp.append(samples)

writeLog("Created: " + str(qtyExperiments) + " Experiments with " + str(qtySamples) + " each \n")

print("Created " ,qtyExperiments, " Experiments with " ,qtySamples, " each")

nExp = 0
fmResults = []

for s in totalExp:
    _val = []
    _gt = []
    for v in s:
        _val.append([v[0], v[1]])
        _gt.append(v[2])
    _val = np.asarray(_val)
    _val_original = _val
    _val_original = _val_original.astype('float32')
    _val = StandardScaler().fit_transform(_val_original)
    strN = "%02d" % nExp
    resultFile = dirOutput + "/DBSCAN-Result-" + strN + ".csv"
    nClusters = dbFun(_val, _val_original, epsilon, minPts, _gt, resultFile)
    nExp += 1
    ok, result, rand, jac = validation.validation(resultFile)
    fmResults.append(result)


meanFM = np.mean(np.array(fmResults))
stdDevFM = np.std(np.array(fmResults))

writeResults(qtyExperiments, epsilon, minPts, meanFM, rand, jac, stdDevFM)

print("Mean FM...: "+ str(meanFM))
print("StdDev FM.: "+ str(stdDevFM) )

writeLog("------ End of Script ------/" + "\n")

fLog.close()
fRes.close()
