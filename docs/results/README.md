# Results

Experiments running gGluster and DBSCAN over several datasets provided in this [site](http://cs.uef.fi/sipu/datasets/), with most of them with Ground Truth.

The methodology of experiment:

1. Run `complete.py` script testing the parameters. It summarize data and run gGluster 
2. Run `DBSCAN.py` script
3. Run `validation.py` script to test:
	- gCluster running over cells
	- DBSCAN running over cells
	- gGluster running over points
	- DBSCAN running over points

## Datasets

### Agregation

- Number of clusters: 7
- Number of Points: 788
- CSV File: [Aggregation.csv](../../data/Agregation/devices/Aggregation.csv)
	- Size: 10,435 bytes


### gCluster 