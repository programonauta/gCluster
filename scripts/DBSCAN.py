import csv

import matplotlib.pyplot as plt
import numpy as np
from sklearn.cluster import DBSCAN
# from sklearn.datasets.samples_generator import make_blobs
from sklearn.preprocessing import StandardScaler

import os
import sys


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
    print("\t-d <dir>\tDirectory of files")
    print("\t-pr <pre>\tPrefix of files (e<epsilon>f<force (with 4 decimals)> - Ex. e014f0.1500)")
    print("\t-t <opt>\t<opt> = c or p (for cells or points respectively)")
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

def dbFun(_x, _original_vals, epsilon, minPts, hasLegend):

    # Values salved for chamelon
    # epsilon = 0.09
    # minPts = 16
    db = DBSCAN(eps=epsilon, min_samples=minPts).fit(_x)  # for cells_eps = 100
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True

    labels = db.labels_
    # print(labels)

    fileOutput = open(DBSCANOutput, "w")


    if fileType == "p":
        j = 0
        for i in _original_vals:
            # write line (x, y, label found by DBSCAN, ground truth
            strCSV = str(i[0]) + "," + str(i[1]) + "," + str(labels[j]) + "," + str(int(i[2])) + "\n"
            j += 1
            fileOutput.write(strCSV)

        fileOutput.close()
    else:
        # Create a file to compare

        totPts = 0
        fileOutput = open(DBSCANOutput, "w")
        with open(inputGT, 'rU') as inp:
            rd = csv.reader(inp)
            qty = 0
            first = True
            for row in rd:
                if first:
                    first = False
                    continue
                totPts += 1
                print("Line: ", totPts, end='\r', flush=True)
                p1 = [float(row[0]), float(row[1])]
                min = 99999
                labelDB = -1
                j = 0
                for i in _original_vals:
                    d = distPt(i, p1)
                    if d < min and d <= minPts:
                        min = d
                        labelDB = labels[j]
                    j += 1

                # Now j has the label found on DBSCAN to point row
                strCSV = str(row[0]) + "," + str(row[1]) + "," + str(labelDB) + "," + row[-1] + "\n"
                fileOutput.write(strCSV)

    n_clusters_ = len(set(labels)) - (1 if -1 else 0)
    title = ('Estimated number of clusters: %d' % n_clusters_)
    subtitle = ('Epsilon = %f, minPts = %d' % (epsilon,minPts))
    print('Estimated number of clusters: %d' % n_clusters_)
    print("Output file: ", DBSCANOutput)
    print("Wait plotting clusters.....")
    plotCluster(_original_vals, labels, core_samples_mask, title, subtitle, hasLegend)
    return


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
                 markeredgecolor='k', markersize=9)
        if legend:
            plt.legend(tuple(unique_labels), loc='best')

        # xy = _x[class_member_mask & ~core_samples_mask]
        # plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
        #       markeredgecolor='k', markersize=2)

    plt.suptitle(title)
    plt.title(subtitle)
    plt.show()
    return


_val = []

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

if hasDir:
    if nameDir == "":
        showError("Directory is not informed")
    if not os.path.isdir(nameDir):
        showError(nameDir + " is not a Directory")
    dirInput = nameDir + "/central/results"
    if not os.path.isdir(dirInput):
        showError(dirInput + " subdirectory not found on ")
else:
    showHelp()
    showError("-d option not found")

hasPrefix, prefix = parseOpt("-pr", True)


if prefix == "":
    print("--------------------------------")
    print("Prefix of file was not informed!")
    print("Have any doubt? Run this with -h")
    print("--------------------------------")
    exit(1)
else:
    prefix = prefix + "-"

hasType, fileType = parseOpt("-t", True)

if hasType:
    if fileType == "":
        showError("File type not informed")
    if fileType != "c" and fileType != "p":
        showError("File type (" + fileType + ") wrong. Enter c or p (cell or point)")
else:
    showError("File type not informed. Please use -t <c> or <p> option")

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

nameDirAux = nameDir.split('/')
nameSingleDir = nameDirAux[len(nameDirAux) - 1]
dirDBSCANOutput = nameDir + "/central/DBSCAN"

prefixDBSCAN = "e%06.4fm%03d-" % (epsilon, minPts)

if fileType == "c":
    prefixDBSCAN += "cells-"
    inputFile = dirInput + "/" + prefix + "cells-result-" + nameSingleDir + ".csv"
else:
    prefixDBSCAN += "points-"
    inputFile = dirInput + "/" + prefix + "points-result-" + nameSingleDir + ".csv"

if not os.path.exists(dirDBSCANOutput):
    os.makedirs(dirDBSCANOutput)

print("Input file: ", inputFile)

DBSCANOutput = dirDBSCANOutput + "/" + prefixDBSCAN + "DBSCAN-" + nameSingleDir + ".csv"

inputGT = nameDir + "/central/consolidate/points-" + nameSingleDir + ".csv"
outputGT = dirDBSCANOutput + "/" + prefixDBSCAN + "DBSCAN-Compare-" + nameSingleDir + ".csv"

with open(inputFile, 'rU') as inp:
    rd = csv.reader(inp)
    qty = 0
    first = True
    for row in rd:
        if first:
            first = False
        else:
            if fileType == "c":
                repeat = int(row[2])
                pos = [3, 4]
                gt = -1  # last position
            else:
                repeat = 1
                pos = [0, 1]
                gt = -1  # last position
            for i in (range(repeat)):
                qty += 1
                # _val receive: x, y and ground truth
                _val.append([row[pos[0]], row[pos[1]], row[gt]])
    print("Total Points:", qty)
    # print(_center)
_val = np.asarray(_val)
_val_original = _val
_val_original = _val_original.astype('float32')
_val = StandardScaler().fit_transform(_val_original)

dbFun(_val, _val_original, epsilon, minPts, hasLegend)

# _len = len(_center)
