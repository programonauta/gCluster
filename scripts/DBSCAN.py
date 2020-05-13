import csv

import time
import validation
import matplotlib.pyplot as plt
import numpy as np
from sklearn.cluster import DBSCAN
# from sklearn.datasets.samples_generator import make_blobs
from sklearn.preprocessing import StandardScaler

import os
import sys

def writeLog(msgLog):
    fLog.write(time.strftime('%d/%m/%Y %a %H:%M:%S') + ":" + msgLog + "\n")
    return;

def writeResultsHeader():
    fRes.write("time-stamp,epsilon,minPts,clusters,type,FM" + "\n")

def writeResults(epsilon, minPts, clusters, type, FM, rand, jac):
    fRes.write(time.strftime('%d/%m/%Y %a %H:%M:%S'))
    msg = ",%6.4f,%03d,%d,%s,%07.5f,%07.5f,%07.5f\n" % (epsilon, minPts, clusters, type, FM, rand, jac)
    fRes.write(msg)
    return;



def isfloat(value):
    try:
        float(value)
        return True
    except ValueError:
        return False


def showHeader():
    print("DBSCAN algorithm")
    print("Developed by Ricardo Brandao: https://github.com/programonauta/grid-clustering")
    print("Based on scikit learn library")
    print("------------------------------------------------------------------------------")


def showError(msgErr):
    print("Error on script")
    print("---------------")
    print(msgErr)
    exit(1)


def showHelp():
    print("\tOptions\t\tDescription")
    print("\t-h\t\tShow this help")
    print("\t-d <dir>\tThe data files gathering by devices will be found on <dir>/devices directory")
    print("\t-t <type>\tType of input (c)ell or (p)oint")
    print("\t-e <value>\tEpsilon value")
    print("\t-m <value>\tMin points value")
    print("\t-l\t\tPrint legend")
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


def distPt(a, b):
    dist = ((b[0]-a[0])**2 + (b[1]-a[1])**2) ** 0.5
    return dist

def dbFun(_x, _original_vals, epsilon, minPts, hasLegend, gt):

    # Values salved for chamelon
    # epsilon = 0.09
    # minPts = 16
    db = DBSCAN(eps=epsilon, min_samples=minPts).fit(_original_vals)  # for cells_eps = 100
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True

    labels = db.labels_
    # print(labels)

    #create a map label DBSCAN x label Ground Truth

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
            finalMap.append([label,label,max])
        else:
            finalMap.append([label, labelGT, max])

    writeLog("   Map Matrix")
    print("Map Matrix")

    # sort Final Map


    #  correct wrong relations

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

    for m in finalMap:
        strMat = "   %d, %d, %d" % (m[0], m[1], m[2])
        writeLog(strMat)
        print(strMat)

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

    title = ('Número de Clusters: %d' % n_clusters_)
    subtitle = ('Epsilon = %f, minPts = %d' % (epsilon,minPts))
    print('Estimated number of clusters: %d' % n_clusters_)

    writeLog("   Clusters...: " + str(n_clusters_))

    print("Wait plotting clusters.....")
    plotCluster(_original_vals, labels, core_samples_mask, title, subtitle, hasLegend)
    return n_clusters_


def plotCluster(_x, labels, core_samples_mask, title, subtitle, legend):
    unique_labels = set(labels)
    colors = [plt.cm.Spectral(each)
              for each in np.linspace(0, 1, len(unique_labels))]

    for k, col in zip(unique_labels, colors):
        if k == -1:
            # Black used for noise.
            col = [0, 0, 0, 1]

        class_member_mask = (labels == k)

        xy = _x[class_member_mask & core_samples_mask]
        plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                 markeredgecolor='k', markersize=5)
        if legend:
            plt.legend(tuple(unique_labels), loc='best')

        # xy = _x[class_member_mask & ~core_samples_mask]
        # plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
        #       markeredgecolor='k', markersize=2)

    plt.suptitle(title)
    plt.title(subtitle)
    plt.show()
    return


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

nameDirAux = nameDir.split('/')
nameSingleDir = nameDirAux[len(nameDirAux) - 1]

hasType, optType = parseOpt("-t", True)

if not hasType:
    showError("Please inform type: -t c for cell or -t p for point")

if optType != "c" and  optType != "p":
    showError("Incorrect type. Valid types: (c)ell or (p)oint")

dirOutput = nameDir + "/central/6.resultsDBSCAN"

if optType == "c":
    inputFile = nameDir + "/central/4.mapClusterOutput/expanded-points-" + nameSingleDir + ".csv"
    outputFile = dirOutput + "/output-cells-DBSCAN-" + nameSingleDir + ".csv"
    strType = "Cell"
else:
    inputFile = nameDir + "/central/2.deviceConsolidation/points-" + nameSingleDir + ".csv"
    outputFile = dirOutput + "/output-points-DBSCAN-" + nameSingleDir + ".csv"
    strType = "Point"

if not os.path.exists(dirOutput):
    os.makedirs(dirOutput)

logFile = nameDir + "/" + nameSingleDir + "-DBSCAN.log"
logResults = nameDir + "/" + nameSingleDir + "-DBSCAN-results.csv"

hasFile = os.path.isfile(logResults)

fLog = open(logFile, "a")
fRes = open(logResults, "a")

if not hasFile:
    writeResultsHeader()

# Print the header of script
showHeader()

# verify if have any -h option
hasHelp, opt = parseOpt("-h", False)

if (hasHelp > 0):
    showHelp()
    exit(1)

# replace "\\" by "/". In windows machines uses "\" for subdirectories. Python could handle with / in all OSs.
outputFile = outputFile.replace("\\", "/")

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

hasLegend, opt = parseOpt("-l", False)

print("Input file: ", inputFile)
print("Output file: ", outputFile)
print("Epsilon: ", epsilon)
print("MinPts: ", minPts)

_val = []  # Values
_gt = []   # Ground truths



with open(inputFile, 'rU') as inp:
    rd = csv.reader(inp)
    qty = 0
    first = True
    for row in rd:
        if first:
            first = False
        else:
            qty += 1
            # _val receive: x, y
            _val.append([row[0], row[1]])
            _gt.append(row[-1])
    print("Total Points:", qty)

_val = np.asarray(_val)
_val_original = _val
_val_original = _val_original.astype('float32')
_val = StandardScaler().fit_transform(_val_original)


writeLog("/------ Begin of Script")

writeLog("epsilon: " + str(epsilon) + " minPts: " + str(minPts))
writeLog("   Type.......: " + strType)
writeLog("   Input File.: " + inputFile)
writeLog("   Output File: " + outputFile)

qtyClusters = dbFun(_val, _val_original, epsilon, minPts, hasLegend, _gt)

print("Calculating FM...")
ok, result, rand, jac = validation.validation(outputFile)

print("fm", result)
print("rand", rand)
print("jac", jac)

writeLog("   FM.........: " + str(result))
writeLog("   Rand.......: " + str(rand))
writeLog("   Jac........: " + str(jac))

writeResults(epsilon,minPts,qtyClusters,optType,result,rand,jac)

writeLog("End of Script ------/")


fLog.close()
fRes.close()

# _len = len(_center)
