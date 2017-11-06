### TEMPLATE

- Number of clusters: xx
- Number of Points: xx
- Row Data: CSV File: [xx.csv](../../data/xx/devices/xx.csv)
	- Size: xx bytes
- Configuration File: [config-xx.csv](../../data/xx/config/config-xx.csv)
	- Size: xx bytes

#### gCluster

##### Parameters

- Epsilon: xx
- Minimum Force: 0.xx

##### Summarization and Clustering
- command line: `complete.py -d ../data/xx -e xx -f 0.xx -p -x`

<img src = "images/xx/graph-xx-saved.svg" height="70%" >

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
    <td>xx.csv</td>
    <td>xx<br></td>
  </tr>
  <tr>
    <td colspan="2"><b>summarized data<br></td>
  </tr>
  <tr>
    <td>File Name<br></td>
    <td>Size (bytes)<br></td>
  </tr>
  <tr>
    <td>config-xx.csv</td>
    <td>34</td>
  </tr>
  <tr>
    <td>cell-xx-01.csv</td>
    <td>xx</td>
  </tr>
  <tr>
    <td><b>TOTAL<br></td>
    <td><b>xx</td>
  </tr>
  <tr>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td><b>Reduction (%)<br></td>
    <td><b>xx%</td>
  </tr>
</table>

#### DBSCAN 

##### cells

###### Parameters

- Epsilon: xx
- Minimum Points: x

###### Clusterization
- command line: `DBSCAN.py -d ..\data\xx -pr xxx -t c -e xxx -m x`

<img src = "images/xx/DBSCAN-cell-xx.png" height="60%" >

##### points

###### Parameters

- Epsilon: x
- Minimum Points: x

###### Clusterization
- command line: `DBSCAN.py -d ..\data\xx -pr exx -t p -e x -m x`

<img src = "images/xx/DBSCAN-points-xx.png" height="60%" >

#### Validation - gCluster

###### cells

- command line `validation.py -d ../data/xx -t c -pr xx`
- map file: [xx-cells-result-xx.csv](../../data/xx/config/xx-cells-result-xx.csv)
- **Result --> FM: 0.xxx**

###### points

- command line `validation.py -d ../data/xx -t p -pr xx.xx`
- map file: [xx.xx-points-map-xx.csv](../../data/xx/config/xx.xx-points-map-xx.csv)
- **Result --> FM: 0.xx**

#### Validation - DBSCAN

##### cells

- command line `validation.py -d ../data/xx -t c -pr e0.xx -b`
- map file: [xx-cells-map-DBSCAN-xx.csv](../../data/xx/config/e0.xx-cells-map-DBSCAN-xx.csv)
- **Result -->  FM: 0.xx**

##### points

- command line `validation.py -d ../data/xx -t p -pr e0.xx -b`
- map file: [xx.xx-points-map-DBSCAN-xx.csv](../../data/xx/config/e0.xx-points-map-DBSCAN-xx.csv)
- **Result --> FM: 0.xx**

#### xx - Summary

<table>
  <tr>
    <th>FM</th>
    <th>gCluster<br></th>
    <th>DBSCAN</th>
  </tr>
  <tr>
    <td><b>cells<br></td>
    <td>0.xx</td>
    <td>0.xx</td>
  </tr>
  <tr>
    <td><b>points</td>
    <td>0.xx</td>
    <td>0.xx</td>
  </tr>
</table>
