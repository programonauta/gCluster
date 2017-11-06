# Results

Experiments running gGluster and DBSCAN over several datasets provided in this [site](http://cs.uef.fi/sipu/datasets/), with most of them with Ground Truth.

The methodology of experiment:

1. Go to `script` directory
2. Run `complete.py` script testing the parameters. It summarize data and run gGluster 
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
- Row Data: CSV File: [Agregation.csv](../../data/Agregation/devices/Aggregation.csv)
	- Size: 10,435 bytes
- Configuration File: [config-Agregation.csv](../../data/Agregation/config/config-Agregation.csv)
	- Size: 31 bytes


### gCluster

#### Parameters

- Epsilon: 12
- Minimum Force: 0.21

#### Summarization and Clustering
- command line: `complete.py -d ../data/Agregation -e 12 -f 0.21 -p`

<img src = "images/e012f0.2100-graph-Agregation-saved.svg" >

<table>
  <tr>
    <th colspan="2">Data Reduction<br></th>
  </tr>
  <tr>
    <td colspan="2"><b>raw data<br></td>
  </tr>
  <tr>
    <td>File Name<br></td>
    <td>Size (bytes)<br></td>
  </tr>
  <tr>
    <td>Agregation.csv</td>
    <td>10,435<br></td>
  </tr>
  <tr>
    <td colspan="2"><b>summarized data<br></td>
  </tr>
  <tr>
    <td>File Name<br></td>
    <td>Size (bytes)<br></td>
  </tr>
  <tr>
    <td>config-Agregation.csv</td>
    <td>31</td>
  </tr>
  <tr>
    <td>cell-Agregation-01.csv</td>
    <td>3,228</td>
  </tr>
  <tr>
    <td><b>TOTAL<br></td>
    <td><b>3,259</td>
  </tr>
  <tr>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td><b>Reduction (%)<br></td>
    <td><b>68.77%</td>
  </tr>
</table>

#### Validation

- command line `validation.py -d ../data/Agregation -t p -pr e012f0.2100`
- map file: [e012f0.2100-points-map-Agregation.csv](../../data/Agregation/config/e012f0.2100-points-map-Agregation.csv)
- Result: FM: 0.9127811966774019


