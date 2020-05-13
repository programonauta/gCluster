import numpy as np
from scipy.spatial import distance
from sklearn.cluster import DBSCAN
from sklearn import metrics
from sklearn.datasets.samples_generator import make_blobs
import matplotlib.pyplot as plt

import os
import sys

import csv

def showHeader():
    print("DBSCAN algorithm")
    print("Developed by Ricardo Brandao: https://github.com/programonauta/grid-clustering")
    print("Based on scikit learn library")
    print("------------------------------------------------------------------------------")

def showHelp():
    print("\tOptions\t\tDescription")
    print("\t-h\t\tShow this help")
    print("\t-i <file>\tInput File (csv)")
    print("\t-e <value>\tEpsilon value")
    print("\t-m <value>\tMin points value")
    print("\t-l\t\tPrint legend")
    return

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
	
hasFile, inputFile = parseOpt("-i", True)

showHeader()

if inputFile == "":
	showHelp()
	showError("Input File not informed")

# verify minPts
hasMinPts, minPts = parseOpt("-m", True)

if minPts == "":
    showError("minPts not informed")
else:
    minPts = int(minPts)
    if minPts <= 0:
        showError("Invalid value for minPts:" + str(minPts))

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
		
hasLegend, opt = parseOpt("-l", False)

print("Input file: ", inputFile)
print("Epsilon: ", epsilon)
print("MinPts: ", minPts)

_val = []  # Values

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
    print("Total Points:", qty)

_val = np.asarray(_val)
_val_original = _val
_val_original = _val_original.astype('float32')
#_val = StandardScaler().fit_transform(_val_original)
	

##############################################################################
# Compute DBSCAN

db = DBSCAN(eps=epsilon, min_samples=minPts).fit(_val_original)  # for cells_eps = 100
core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
core_samples_mask[db.core_sample_indices_] = True
core_samples = db.core_sample_indices_


labels = db.labels_

# Number of clusters in labels, ignoring noise if present.
n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
title = ('Número de Clusters: %d' % n_clusters_)
subtitle = ('Epsilon = %f, minPts = %d' % (epsilon,minPts))

print ('Estimated number of clusters: %d' % n_clusters_)
#print ("Homogeneity: %0.3f" % metrics.homogeneity_score(labels_true, labels))
#print ("Completeness: %0.3f" % metrics.completeness_score(labels_true, labels))
#print ("V-measure: %0.3f" % metrics.v_measure_score(labels_true, labels))
#print ("Adjusted Rand Index: %0.3f" % \
#    metrics.adjusted_rand_score(labels_true, labels))
#print ("Adjusted Mutual Information: %0.3f" % \
#	    metrics.adjusted_mutual_info_score(labels_true, labels))
#print ("Silhouette Coefficient: %0.3f" %
#       metrics.silhouette_score(D, labels, metric='precomputed'))

print("Wait plotting clusters.....")
plotCluster(_val_original, labels, core_samples_mask, title, subtitle, hasLegend)

