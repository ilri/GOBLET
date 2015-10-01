# GOBLET
GOBLET is a series of command line tools to easily perform spatial operations on GIS data.  Each tool performs a specific spatial operation. The main purpose of having a set of command line tools is to facilitate the use of GOBLET in scripts like R or Python or in 3rd party applications like the Feed Resources Framework and the Development Domains. GOBLET is open source and cross platform. It can be build on Windows, Linux and Mac.

Each GOBLET command line tool can be execute in a local database (created with goblet-createdb) or in a remote server. The remote option allows GOBLET to perform spatial operations in high performing MySQL servers or MySQL Cluster servers.

GOBLET does not display data (on screen maps). To display any raster or shape output you can use [QGIS](http://www.qgis.org/).

GOBLET only imports/exports ArcInfo ASCII grids and shapefiles. If you need to use any other raster format like GeoTIFF, you can use [GDAL](http://www.gdal.org/gdal_translate.html) to convert raster files across different formats.

## The toolbox

### GOBLET-CreateDB
Creates a new GOBLET database.
#### *Parameters*
  - r - Create the database in a remote server.
  - d - Database name.
  - c - Cell size.
  - a - Local path for the database. "." by default. If remote (**-r**) is not used.
  - H - Remote MySQL server. If remote (**-r**) is used.
  - P - Remote MySQL port. If remote (**-r**) is used.
  - u - Remote MySQL user. If remote (**-r**) is used. The user account must have create database permissions.
  - p - Remote MySQL password. If remote (**-r**) is used.

#### *Examples*
Create a new GOBLET database called “my1kmdb” in the directory ./mygobletdbs **(The directory must exist)** with a cell size of 0.00833333 (roughly 1km cell size).
  ```sh
   $ goblet-createdb -a ./mygobletdbs -d my1kmdb -c 0.008333333
```
Create a new GOBLET database called “my1kmdb” in a remote server with a cell size of 0.00833333 (roughly 1km cell size).
  ```sh
   $ goblet-createdb -r -H MyRemoteServer -u myUserAccount -p MyPassword -d my1kmdb -c 0.008333333
```

---
### GOBLET-ImportDataset
Imports an ArcInfo ASCII grid as a GOBLET grid dataset. The ASCII grid must have the **exact** cell size of the target GOBLET database.
#### *Parameters*
  - r - Connect to a remote server.
  - a - Local path for the database. If remote (**-r**) is not used.
  - d - Database name.
  - t - Dataset name.
  - s - Dataset description.
  - g - Input grid file.
  - H - Remote MySQL server. If remote (**-r**) is used.
  - P - Remote MySQL port. If remote (**-r**) is used.
  - u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
  - p - Remote MySQL password. If remote (**-r**) is used.

#### *Examples*
Import a grid called lgp001.asc into a dataset called "lgp" with description "Length of Growing Period - Kenya, Tanzania and Uganda".
```sh
   $ goblet-importdataset -a ./mygobletdbs -d my1kmdb -g ./lgp001.asc -t lgp -s 'Length of Growing Period - Kenya, Tanzania and Uganda'
```

Import a grid called pop00den1.asc into a dataset called "popdens" with description "Population density - Kenya, Tanzania and Uganda".
```sh
   $ goblet-importdataset -a ./mygobletdbs -d my1kmdb -g ./pop00den1.asc -t popdens -s 'Population density - Kenya, Tanzania and Uganda'
```

Import a grid called time1.asc into a dataset called "traveltime" with description "Travel time in hours - Kenya, Tanzania and Uganda".
```sh
   $ goblet-importdataset -a ./mygobletdbs -d my1kmdb -g ./time1.asc -t traveltime -s 'Travel time in hours - Kenya, Tanzania and Uganda'
```

---
### GOBLET-ImportShape
Imports a shape file as a GOBLET shape dataset. The shape file will be rasterized using the database cell size and the shapes will be imported as MySQL spatial geometries with an unique ID called *"shapeID"*.  To view the contents of a shape dataset use *GOBLET-DescribeShape*
#### *Parameters*
  - r - Connect to a remote server.
  - a - Local path for the database. If remote (**-r**) is not used.
  - d - Database name.
  - t - Dataset name.
  - s - Dataset description.
  - S - Input shape file.
  - H - Remote MySQL server. If remote (**-r**) is used.
  - P - Remote MySQL port. If remote (**-r**) is used.
  - u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
  - p - Remote MySQL password. If remote (**-r**) is used.

#### *Examples*
Import  the shape file called kentanuga.shp into a dataset called "kentanuga" with description "Kenya, Tanzania and Uganda".
  ```sh
   $ goblet-importshape -a ./mygobletdbs -d my1kmdb -S ./kentanuga.shp -t kentanuga -s 'Kenya, Tanzania and Uganda'
```
Import  the shape file called kenya_provinces.shp into a dataset called "kenprov" with description "Kenya Provinces".
```sh
   $ goblet-importshape -a ./mygobletdbs -d my1kmdb -S ./kenya_provinces.shp -t kenprov -s 'Kenya Provinces'
```

---
### GOBLET-OutputRaster
Creates a ArcInfo ASCII grid from a GOBLET dataset. The GOBLET dataset can be a grid, a classified grid or the result of a combination. The tool can also constraint the output to a spatial extent or to a shape dataset.

#### An Extent
In the image below an extent represents a box covering from the blue point **(UpperLeft)** to the green point **(LowerRight)** in latitude and longitude degrees. For Kenya for example the extent is roughly (33.76,4.87) (42.07,-4.99).

![Image](/extent.png?raw=true "Example of an extent")

#### *Parameters*
  - r - Connect to a remote server.
  - a - Local path for the database. If remote (**-r**) is not used.
  - d - Database name.
  - t - Dataset name.
  - o - Output ASCII grid file.
  - i - Dataset type to export. A normal (**g**)rid, a (**c**)lassifified grid, or the result of a com(**b**)ination.
  - H - Remote MySQL server. If remote (**-r**) is used.
  - P - Remote MySQL port. If remote (**-r**) is used.
  - u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
  - p - Remote MySQL password. If remote (**-r**) is used.
  - c - Combinations to show. Only if the dataset type is **b**. Specify each combination in the following way: *'CombinationCode,CombinationCode,...'*. See *GOBLET-CombineDataset* for more information.
  - S - Constraint the output to a shape dataset. Specify a shape dataset and a series of shapes in the following way: *'ShapeDataSetName:shapeID,ShapeID,...'*
  - e - Constraint the output to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*


#### *Examples*
Output the grid dataset called “lgp” into an ASCII grid called “lgpout.asc” for an extent covering Kenya.
  ```sh
   $ goblet-outputraster -a ./mygobletdbs -d my1kmdb -t lgp -i g -e '(33.76,4.87) (42.07,-4.99)' -o ./lgpout.asc
```
Output the grid dataset called “lgp” into an ASCII grid called “lgpout.asc” constraining the result into the shape of Kenya.
```sh
 $ goblet-outputraster -a ./mygobletdbs -d my1kmdb -t lgp -i g -S 'kentanuga:0' -o ./lgpout.asc
```

---
### GOBLET-OutputShape
Generates a shape file from a GOBLET shape dataset. The tool can output the result of classifications or combinations and can also be constraint to a spatial extent.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Dataset name.
- o - Output shape file.
- i - Dataset type to export. A normal (**s**)hape file, a (**c**)lassifified shape, or the result of a com(**b**)ination.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- b - Combinations to show. Only if the dataset type is **b**. Specify each combination in the following way: *'CombinationCode,CombinationCode,...'*. See *GOBLET-CombineShape* for more information.
- c - Classifications to show. Only if the dataset type is **c**. Specify each class in the following way: *'ClassCode,ClassCode,...'*. See *GOBLET-ClassifyShape* for more information.
- e - Constraint the output to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Example*
Output the shape dataset called "kentanuga" into a shape file called "testout.shp".
  ```sh
   $ goblet-outputshape -a ./mygobletdbs -d my1kmdb -s kentanuga -i s -o ./testout.shp
```

---
### GOBLET-AggregateDataset
Aggregates the data of a grid dataset into a shape dataset using aggregation functions like avg, sum, max, min and count. For binary grids (1/0 values) GOBLET-AggregateDataset can use functions like and, or & xor. The tool can also constraint the aggregate to a spatial extent or to a shape dataset.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Grid dataset to aggregate.
- s - Target shape dataset.
- f - Aggregation function avg, sum, max, min, and, or, xor, count. The default is avg.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- S - Constraint the aggregate to a different shape dataset. Specify a shape dataset and a series of shapes in the following way: *'ShapeDataSetName:shapeID,ShapeID,...'*
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Examples*
Aggregate the grid "lgp" using an average function into the shape of "kenprov"
  ```sh
   $ goblet-aggregatedataset -a ./mygobletdbs -d my1kmdb -t lgp -s kenprov -f avg
```
Aggregate the grid "popdens" using an average function into the shape of "kenprov"
  ```sh
   $ goblet-aggregatedataset -a ./mygobletdbs -d my1kmdb -t popdens -s kenprov -f avg
```
Aggregate the grid "traveltime" using an average function into the shape of "kenprov"
  ```sh
   $ goblet-aggregatedataset -a ./mygobletdbs -d my1kmdb -t traveltime -s kenprov -f avg
```

---
### GOBLET-ClassifyDataset
Classifies a grid dataset using a classification code and values *from* and *to*. The tool can also constraint the aggregate to a spatial extent or to a shape dataset.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Dataset name.
- c - Class definition. Specify the classes in the following way: '**ClassNumber**:*valueFrom* *ValueTo*,**ClassNumber**:*valueFrom* *ValueTo*,....'
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- S - Constraint the aggregate to a different shape dataset. Specify a shape dataset and a series of shapes in the following way: *'ShapeDataSetName:shapeID,ShapeID,...'*
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Examples*
Classify the grid dataset "lgp" in three classes:
  1. Class 10 with values ranging from 0 to 114;
  2. Class 20 with values ranging from 114 to 228 and;
  3. Class 30 with values ranging from 228 to 344.

```sh
   $ goblet-classifydataset -a ./mygobletdbs -d my1kmdb -t lgp -c '10:0 114,20:114 228,30:228 344'
```
Classify the grid dataset "popdens" in three classes:
  1. Class 100 with values ranging from 0 to 80;
  2. Class 200 with values ranging from 80 to 160 and;
  3. Class 300 with values ranging from 160 to 400.

```sh
   $ goblet-classifydataset -a ./mygobletdbs -d my1kmdb -t -t popdens -c '100:0 80,200:80 160,300:160 400'
```
Classify the grid dataset "traveltime" in three classes:
  1. Class 1000 with values ranging from 0 to 4;
  2. Class 2000 with values ranging from 4 to 8 and;
  3. Class 3000 with values ranging from 8 to 47.

```sh
   $ goblet-classifydataset -a ./mygobletdbs -d my1kmdb -t -t traveltime -c '1000:0 4,2000:4 8,3000:8 47'
```

---
### GOBLET-ClassifyAggregate
Classifies an aggregation (see *GOBLET-AggregateDataset*) using a classification code and values *from* and *to*. The tool can also constraint the aggregate to a spatial extent.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Grid dataset that was aggregated.
- s - Shape dataset that was used in the aggregation.
- c - Class definition. Specify the classes in the following way: '**ClassNumber**:*valueFrom* *ValueTo*,**ClassNumber**:*valueFrom* *ValueTo*,....'
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Examples*
Classifies the aggregated "lgp" in the shape dataset "kenprov" with three classes:
  1. Class 10 with values ranging from 0 to 114;
  2. Class 20 with values ranging from 114 to 228 and;
  3. Class 30 with values ranging from 228 to 344.

```sh
   $ goblet-classifyaggregate -a ./mygobletdbs -d my1kmdb -t lgp -s kenprov -c '10:0 114,20:114 228,30:228 344'
```

Classifies the aggregated "popdens" in the shape dataset "kenprov" with three classes:
  1. Class 100 with values ranging from 0 to 80;
  2. Class 200 with values ranging from 80 to 160 and;
  3. Class 300 with values ranging from 160 to 400.

```sh
   $ goblet-classifyaggregate -a ./mygobletdbs -d my1kmdb -t popdens -s kenprov -c '100:0 80,200:80 160,300:160 400'
```

Classifies the aggregated "traveltime" in the shape dataset "kenprov" with three classes:
  1. Class 1000 with values ranging from 0 to 4;
  2. Class 2000 with values ranging from 4 to 8 and;
  3. Class 3000 with values ranging from 8 to 11.

```sh
   $ goblet-classifyaggregate -a ./mygobletdbs -d my1kmdb -t traveltime -s kenprov -c '1000:0 4,2000:4 8,3000:8 11'
```

---
### GOBLET-CombineDatasets
Combines a series of classified grid datasets into a result grid dataset. The tool can also constraint the aggregate to a spatial extent or to a shape dataset.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Classified datasets to combine. Specify the datasets in the following way: '*dataset*,*dataset*,*dataset*,...'
- f - Combination function that can either be **sum** or (**mul**)tiplication. The default is sum.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- S - Constraint the aggregate to a different shape dataset. Specify a shape dataset and a series of shapes in the following way: *'ShapeDataSetName:shapeID,ShapeID,...'*
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Example*
Combine the grids "lgp", "popdens" and "traveltime" into one result.
```sh
   $ goblet-combinedatasets -a ./mygobletdbs -d my1kmdb -t 'lgp,popdens,traveltime'
```

---
### GOBLET-CombineAggregate
Combines the classified grid datasets that are aggregated in shape dataset.  (see *GOBLET-AggregateDataset* and *GOBLET-ClassifyAggregate*). The tool can also constraint the aggregate to a spatial extent.
#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- s - Shape dataset that was used in the aggregation and classification.
- f - Combination function that can either be sum, and, or & xor. The default is sum.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*

#### *Example*
Combine aggregated grids in kenprov ("lgp", "popdens" and "traveltime") into a result
```sh
   $ goblet-combineaggregate -a ./mygobletdbs -d my1kmdb -s kenprov
```

---
### GOBLET-GenDatasetStats
Generates the following statistics from a grid dataset:

1. Number of cells.
2. Maximum value.
3. Minimum value.
4. Average value.
5. Standard deviation.


#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Dataset name.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.

#### *Example*
Generate basic statistics from the grid dataset "traveltime"
```sh
   $ goblet-gendatasetstats -a ./mygobletdbs -d my1kmdb -t traveltime
```

---
### GOBLET-GenaAgregateStats
Generates the following statistics from an aggregated grid dataset in a shape dataset:

1. Number of shapes.
2. Maximum value.
3. Minimum value.
4. Average value.
5. Standard deviation.


#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Aggregated dataset name.
- s - Shape dataset that was used in the aggregation.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.

#### *Example*
Generate basic statistics from the aggregated grid dataset "traveltime" in the shape dataset "kenprov"
```sh
   $ goblet-genaggregatestats -a ./mygobletdbs -d my1kmdb -t traveltime -s kenprov
```

---
### GOBLET-DescribeShape
Display information about a shape dataset. The output can be easily readable for humans or represented in XML for its usage by other computer programs. The human readable output looks like this:
```
ID: kenprov
Description: Kenya Provinces
Units:
Shape type: POLYGON
Has metadata: No
No. Colums in raster: 953
No. Rows in raster: 1165
Raster X center: 33.954166412353516
Raster Y center: -4.687500000000000

+---------+--------+-----------+----------+------------+---------------+-----------+
+ shapeid + AREA   + PERIMETER + KENPROV_ + KENPROV_ID + PROV_NAME     + num_cells +
+---------+--------+-----------+----------+------------+---------------+-----------+
| 0       | 14.302 | 32.323    | 2.000    | 3.000      | Rift valley   | 205980    |
| 1       | 13.103 | 28.620    | 3.000    | 6.000      | Eastern       | 188685    |
| 2       | 10.215 | 18.885    | 4.000    | 7.000      | North Eastern | 147007    |
| 3       | 0.000  | 0.067     | 5.000    | 6.000      | Coast         | 3         |
| 4       | 0.000  | 0.047     | 6.000    | 3.000      | Coast         | 2         |
| 5       | 0.003  | 0.268     | 7.000    | 6.000      | Coast         | 42        |
| 6       | 0.665  | 5.497     | 8.000    | 2.000      | Western       | 9571      |
| 7       | 1.028  | 10.284    | 9.000    | 1.000      | Nyanzs        | 14815     |
| 8       | 1.060  | 6.851     | 10.000   | 4.000      | Central       | 15264     |
| 9       | 6.787  | 22.678    | 11.000   | 8.000      | Coast         | 97756     |
| 10      | 0.001  | 0.140     | 12.000   | 1.000      | Coast         | 8         |
| 11      | 0.000  | 0.080     | 13.000   | 1.000      | Coast         | 4         |
| 12      | 0.003  | 0.359     | 14.000   | 1.000      | Coast         | 50        |
| 13      | 0.005  | 0.323     | 15.000   | 1.000      | Coast         | 75        |
| 14      | 0.055  | 1.529     | 16.000   | 5.000      | Nairobi       | 797       |
| 15      | 0.004  | 0.504     | 17.000   | 8.000      | Coast         | 69        |
| 16      | 0.002  | 0.273     | 18.000   | 8.000      | Coast         | 28        |
| 17      | 0.004  | 0.495     | 19.000   | 8.000      | Coast         | 61        |
| 18      | 0.004  | 0.332     | 20.000   | 8.000      | Coast         | 58        |
| 19      | 0.009  | 0.605     | 21.000   | 8.000      | Coast         | 128       |
| 20      | 0.006  | 0.533     | 22.000   | 8.000      | Coast         | 91        |
| 21      | 0.004  | 0.362     | 23.000   | 8.000      | Coast         | 51        |
| 22      | 0.001  | 0.168     | 24.000   | 8.000      | Coast         | 10        |
| 23      | 0.001  | 0.129     | 25.000   | 8.000      | Coast         | 7         |
+---------+--------+-----------+----------+------------+---------------+-----------+
```

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Shape dataset.
- f - Format. (**h**)uman or (**c**)omputer. The default is human.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.

#### *Example*
Describe the shape dataset "kenprov"
```sh
   $ goblet-describeshape -a ./mygobletdbs -d my1kmdb -t kenprov
```

---
### GOBLET-ResetDataset
Clears a grid dataset from any previous classification or combination.

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Dataset name.
- f - Format. (**h**)uman or (**c**)omputer. The default is human.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.

#### *Example*
Reset the grid dataset "lgp"
```sh
   $ goblet-resetdataset -a ./mygobletdbs -d my1kmdb -t lgp
```

---
### GOBLET-ResetAggregate
Clears a shape dataset from any previous aggregations, classification or combinations.

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- s - Shape dataset that was used in the aggregation.
- f - Format. (**h**)uman or (**c**)omputer. The default is human.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.

#### *Example*
Reset the shape dataset "kenyaprov"
```sh
   $ goblet-resetaggregate -a ./mygobletdbs -d my1kmdb -s kenprov
```

---
### GOBLET-DatasetCalc
Calculate operations in and between grid datasets. The result can be exported into an ASCII grid file or as a new GOBLET grid dataset. The tool can also constraint the results to a spatial extent or to a shape dataset

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- c - Calculation to perform. For example: '(DatasetA + DatasetB) / DatasetC'
- o - Output type: (**f**)ile or (**d**)ataset.
- g - Output grid file. Required if output type is **f**.
- t - Target dataset name. Required if output type is **d**.
- s - Target dataset description. Required if output type is **d**.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.
- S - Constraint the aggregate to a different shape dataset. Specify a shape dataset and a series of shapes in the following way: *'ShapeDataSetName:shapeID,ShapeID,...'*
- e - Constraint the aggregate to a spatial extend. Specify the extent in the following way: *'(UpperLeft latitude degrees,UpperLeft longitude degrees) (LowerRight latitude degrees,LowerRight longitude degrees)'*
- O - Overwrite the file or dataset if exists.


#### *Example*
Calculate 2 X traveltime and store the result into the file "output.asc"
```sh
   $ goblet-datasetcalc -a ./mygobletdbs -d my1kmdb -c 'traveltime * 2' -o f -g ./output.asc
```


---
### GOBLET-CalcInCombination
Calculate operations in grid datasets aggregated by combination codes (Requires an existing combination). See *GOBLET-CombineDatasets*. The output can be easily readable for humans or represented in XML for its usage by other computer programs. The human readable output looks like this:
```
+------------+-------------------+
+ Class code + Total travel time +
+------------+-------------------+
| 1110       | 625100.000        |
| 1120       | 1394700.000       |
| 1130       | 64800.000         |
| 1210       | 1025100.000       |
| 1220       | 1716200.000       |
| 1230       | 79000.000         |
| 1310       | 4042800.000       |
| 1320       | 10638300.000      |
| 1330       | 125800.000        |
| 2110       | 4400.000          |
| 2120       | 126400.000        |
| 2130       | 21000.000         |
| 2220       | 31100.000         |
| 2230       | 5900.000          |
| 2310       | 3000.000          |
| 2320       | 8300.000          |
| 2330       | 3500.000          |
| 3110       | 400.000           |
| 3120       | 23000.000         |
| 3130       | 4100.000          |
| 3220       | 3200.000          |
| 3230       | 600.000           |
| 3320       | 1900.000          |
+------------+-------------------+
```

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- c - Calculation to perform. For example: 'sum(DatasetA),sum(DatasetB)'
- o - Output type: (**h**)uman readable or (**c**)omputer readable.
- s - Descriptions for the calculations separated by coma. Default value is the calculation string.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.


#### *Example*
Calculate the sum of travel time for each combination.
```sh
   $ golet-calcincombination -a ./mygobletdbs -d my1kmdb –c 'sum(traveltime)' –o h -s 'Total travel time'
```

---
### GOBLET-ModifyDataset
Modify the description and units of a GOBLET dataset.

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- t - Dataset name.
- s - New description for the dataset.
- U - New unit for the dataset.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.


#### *Example*
Modify the dataset "traveltime" to have the description "East Africa travel time"
```sh
   $ goblet-modifydataset -a ./mygobletdbs -d my1kmdb -t traveltime -s "East Africa travel time"
```

---
### GOBLET-ListDatasets
List the dataset in a GOBLET database. The output can be easily readable for humans or represented in XML for its usage by other computer programs. The human readable output looks like this:
```
+-------------------+--------------------------+-------+----------+--------------+-------------------+----------------+--------------------+---------------------+-------------------------------+
+ Dataset code      + Description              + Units + Metadata + Dataset type + Number of columns + Number of rows + X center           + Y center            + Shape type (if type is shape) +
+-------------------+--------------------------+-------+----------+--------------+-------------------+----------------+--------------------+---------------------+-------------------------------+
| datasetCalcOutput | Nairobi lgp *2           | class |          | grid         | 53                | 34             | 36.674991333000008 | -1.441663008999996  | NA                            |
| kenyaprov         | Kenya provinces          |       |          | shape        | 953               | 1166           | 33.954165308499995 | -4.687499812500000  | POLYGON                       |
| lgp               | Length of growing period |       |          | grid         | 1510              | 1970           | 29.327170000001001 | -11.745699999998999 | NA                            |
| popdens           | Population density       |       |          | grid         | 1510              | 1970           | 29.327170000001001 | -11.745699999998999 | NA                            |
| traveltime        | East Africa travel time  |       |          | grid         | 1510              | 1970           | 29.327170000001001 | -11.745699999998999 | NA                            |
+-------------------+--------------------------+-------+----------+--------------+-------------------+----------------+--------------------+---------------------+-------------------------------+
```

#### *Parameters*
- r - Connect to a remote server.
- a - Local path for the database. If remote (**-r**) is not used.
- d - Database name.
- f - Format: (h)uman or (c)omputer.
- H - Remote MySQL server. If remote (**-r**) is used.
- P - Remote MySQL port. If remote (**-r**) is used.
- u - Remote MySQL user. If remote (**-r**) is used. The user account must have create table permissions.
- p - Remote MySQL password. If remote (**-r**) is used.


#### *Example*
Modify the dataset "traveltime" to have the description "East Africa travel time"
```sh
   $ goblet-listdatasets -a ./mygobletdbs -d my1kmdb
```

## Technology
GOBLET was built using:

- [C++](https://isocpp.org/), a general-purpose programming language.
- [Qt](https://www.qt.io/), a cross-platform application framework.
- [MySQL](http://www.mysql.com/), an open-source relational database management system.
- [TClap](http://tclap.sourceforge.net/), a small, flexible library that provides a simple interface for defining and accessing command line arguments. *(Included in source code)*
- [QuaZIP](http://quazip.sourceforge.net/),  a simple C++ wrapper over Gilles Vollant's ZIP/UNZIP package that can be used to access ZIP archives.
- [Shapefile](http://shapelib.maptools.org/), a C Library provides the ability to write simple C programs for reading, writing and updating (to a limited extent) ESRI Shapefiles, and the associated attribute file (.dbf).
- [CMake] (http://www.cmake.org/), a cross-platform free and open-source software for managing the build process of software using a compiler-independent method.


## Building and testing
To build GOBLET on Linux do:

    $ git clone https://github.com/ilri/GOBLET.git    
    $ mkdir build
    $ cd build
    $ cmake ..
    $ sudo make install    

## Author
Carlos Quiros (cquiros_at_qlands.com / c.f.quiros_at_cgiar.org)


## License
This repository contains the code of:

- [TClap](http://tclap.sourceforge.net/) which is licensed under the [MIT license](https://raw.githubusercontent.com/twbs/bootstrap/master/LICENSE).
- [Shapefile To Grid](http://www.mapwindow.org/apps/wiki/doku.php?id=shapefile_to_grid), a application that converts a shapefile to a grid. It is licensed under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/).


Otherwise, GOBLET is licensed under [LGPL V3](http://www.gnu.org/licenses/lgpl-3.0.html).
