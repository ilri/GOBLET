shp2mysql - Convert Shape file to MySQL geometric tables
________________________________________________

VERSION: 0.4 (July 2004) 

IMPORTANT:
This is a quick hack by Barend Kobben <kobben@itc.nl> of the original
shp2pgsql loader 0.7 for PostGIS by  http://postgis.refractions.net
*** ALMOST NO SERIOUS TESTING DONE, use at own risk!
*** NOTE: requires [included] cygwin1.dll (part of the windows-unix shell (http://www.cygwin.com/))
Released under GPL, http://www.gnu.org/copyleft/gpl.html

INTRODUCTION:
This program takes in ESRI shape files and outputs SQL command files suitable
for use in the MySQL spatial database version 4.1.x , and should also work
in future 5.0 versions (not tested).


This application uses functionality from shapelib 1.2.9
by Frank Warmerdam <warmerda@gdal.velocet.ca> to read from ESRI
Shape files.


INSTALLATION:

either:
- Use the readymade binary "shp2mysql.exe" in conjunction with cygwin1.dll
- (re-)build shp2mysql: just run 'make' in the 'src' directory 
(USING THE CYGWIN SHELL if you are in MS Windows!);
Copy the binary wherever you like.


USAGE:

shp2mysql [<options>] <shapefile> <tablename> <database name>

The <shapefile> is the name of the shape file, without any extension
information. For example, 'roads' would be the name of the shapefile 
comprising the 'roads.shp', 'roads.shx', and 'roads.dbf' files.

The <tablename> is the name of the database table you want the data stored
in in the database. Within that table, the geometry will be placed in 
the 'geometry' column by default.

The <database name> is the name of the database you are going to put the
the data into.

The options are as follows:

 -s <srid>  Set the SRID field. If not specified it defaults to -1.
  
(-a || -c || -d) these options are mutually exclusive.

  -a    Append mode. Do not delete the target table or try to create
        a new table, simple insert the data into the existing table.
        A table will have to exist for this to work, it is usually
        used after a create mode as been run once.(mutually exclusive
	with -c and -d)
  -c    Create mode. This is the default mode is no other is specified.
	Create a new table and upload the data into that table.
	(mutually exclusive with -a and -d)
  -d    Delete mode. Delete the database table named <tablename>, then
	create a new one with that name before uploading the data into
	the new empty database table.(mutually exclusive with -a and -c)


EXAMPLES:

Saving to an intermediate sql file to load in mysql later:

  shp2mysql roads1 roads_table my_db > roads.sql

Loading directly:

  shp2mysql -c roads1 roads_table my_db | mysql my_db -u user -p password
  shp2mysql -a roads2 roads_table my_db | mysql my_db

