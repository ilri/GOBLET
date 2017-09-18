#include "writeshapefile.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QVariant>
#include <stdlib.h>
#include <string.h>
#include <QStringList>

void gbtLog2(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

writeShapefile::writeShapefile(QObject *parent) :
    QObject(parent)
{
}

int writeShapefile::getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

void writeShapefile::getFieldSize(QString fieldDef, int &size, int &prec)
{
    /*

      This function return the size and precision of mysql fields

    */


    QString temp;
    temp = fieldDef;
    int pos;
    pos = fieldDef.indexOf("(");
    temp = fieldDef.mid(pos,fieldDef.length()-pos);
    temp = temp.replace("(","");
    temp = temp.replace(")","");
    pos = temp.indexOf(",");
    if (pos >= 0)
    {
        size = temp.left(pos).toInt();
        prec = temp.mid(pos+1,temp.length()-pos+1).toInt();
    }
    else
    {
        size = temp.toInt();
        prec = 0;
    }
}

int writeShapefile::shpadd( int argc, char ** argv )
{
    /*
      *****************************************************************************
      $Id: shpadd.c,v 1.16 2010-06-21 20:41:52 fwarmerdam Exp $

      Project:  Shapelib
      Purpose:  Sample application for adding a shape to a shapefile.
      Author:   Frank Warmerdam, warmerdam@pobox.com

     ******************************************************************************
      Copyright (c) 1999, Frank Warmerdam

      This software is available under the following "MIT Style" license,
      or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
      option is discussed in more detail in shapelib.html.
     ******************************************************************************
    */

    /*

      GOBLET uses Frank Warmerdam's shpadd as it is. No need to create new code.

    */

    SHPHandle	hSHP;
    int		nShapeType, nVertices, nParts, *panParts, nVMax, i;
    double	*padfX, *padfY, *padfZ = NULL, *padfM = NULL;
    SHPObject	*psObject;
    const char  *tuple = "";
    const char  *filename;


    if( argc < 2 )
    {
        return 1;
    }

    filename = argv[1];
    argv++;
    argc--;

    /* -------------------------------------------------------------------- */
    /*      Check for tuple description options.                            */
    /* -------------------------------------------------------------------- */
    if( argc > 1
            && (strcmp(argv[1],"-z") == 0
                || strcmp(argv[1],"-m") == 0
                || strcmp(argv[1],"-zm") == 0) )
    {
        tuple = argv[1] + 1;
        argv++;
        argc--;
    }

    /* -------------------------------------------------------------------- */
    /*      Open the passed shapefile.                                      */
    /* -------------------------------------------------------------------- */
    hSHP = SHPOpen( filename, "r+b" );

    if( hSHP == NULL )
    {
        printf( "Unable to open:%s\n", filename );
        return 1;
    }

    SHPGetInfo( hSHP, NULL, &nShapeType, NULL, NULL );

    if( argc == 1 )
        nShapeType = SHPT_NULL;

    /* -------------------------------------------------------------------- */
    /*	Build a vertex/part list from the command line arguments.	*/
    /* -------------------------------------------------------------------- */
    nVMax = 1000;
    padfX = (double *) malloc(sizeof(double) * nVMax);
    padfY = (double *) malloc(sizeof(double) * nVMax);

    if( strchr(tuple,'z') )
        padfZ = (double *) malloc(sizeof(double) * nVMax);
    if( strchr(tuple,'m') )
        padfM = (double *) malloc(sizeof(double) * nVMax);

    nVertices = 0;

    if( (panParts = (int *) malloc(sizeof(int) * 1000 )) == NULL )
    {
        printf( "Out of memory\n" );
        return 1;
    }

    nParts = 1;
    panParts[0] = 0;

    for( i = 1; i < argc;  )
    {
        if( argv[i][0] == '+' )
        {
            panParts[nParts++] = nVertices;
            i++;
        }
        else if( i < static_cast<int>(argc-1-strlen(tuple)) )
        {
            if( nVertices == nVMax )
            {
                nVMax = nVMax * 2;
                padfX = (double *) realloc(padfX,sizeof(double)*nVMax);
                padfY = (double *) realloc(padfY,sizeof(double)*nVMax);
                if( padfZ )
                    padfZ = (double *) realloc(padfZ,sizeof(double)*nVMax);
                if( padfM )
                    padfM = (double *) realloc(padfM,sizeof(double)*nVMax);
            }

            sscanf( argv[i++], "%lg", padfX+nVertices );
            sscanf( argv[i++], "%lg", padfY+nVertices );
            if( padfZ )
                sscanf( argv[i++], "%lg", padfZ+nVertices );
            if( padfM )
                sscanf( argv[i++], "%lg", padfM+nVertices );

            nVertices += 1;
        }
    }

    /* -------------------------------------------------------------------- */
    /*      Write the new entity to the shape file.                         */
    /* -------------------------------------------------------------------- */
    psObject = SHPCreateObject( nShapeType, -1, nParts, panParts, NULL,
                                nVertices, padfX, padfY, padfZ, padfM );
    SHPWriteObject( hSHP, -1, psObject );
    SHPDestroyObject( psObject );

    SHPClose( hSHP );

    free( panParts );
    free( padfX );
    free( padfY );
    free( padfZ );
    free( padfM );

    return 0;
}

QStringList writeShapefile::separateCoords(QString data)
{
    /*
      This function separate the coordinates into string items in a list.
    */

    QStringList res;

    QString temp;
    int pos;
    QString coord;

    temp = data;
    pos =0;
    while (pos <= temp.length()-1)
    {
        if (temp[pos] != ' ')
        {
            coord = coord + temp[pos];
            pos++;
        }
        else
        {
            res.append(coord);
            coord = "";
            temp = temp.mid(pos+1,temp.length()-pos+1);
            pos = 0;
        }
    }
    res.append(coord);


    return res;
}

QStringList writeShapefile::getGeometry(QString mysqlGeo)
{
    // This fuction separated the MysqlGeometry string into
    // separated x y coordinates in a StringList
    QString temp;
    temp = mysqlGeo;
    int pos;
    temp = temp.left(temp.length()-2);
    pos = temp.indexOf("(");
    pos++;
    temp = temp.mid(pos+1,temp.length()-pos+1);
    pos = temp.indexOf(")");
    QString geodata;
    QStringList res;

    while (pos+1 < temp.length())
    {
        geodata = temp.left(pos+1);
        //Clean the string
        geodata.replace("(","");
        geodata.replace(")","");
        geodata.replace(","," ");
        res.append(separateCoords(geodata)); //Separate the coordinates into String items
        res.append("+"); //Appends a + so shpadd understand it as a new part

        pos = temp.indexOf("(",pos);
        temp = temp.mid(pos,temp.length()-pos);
        pos = temp.indexOf(")");
    }
    //Clean the string
    temp.replace("(","");
    temp.replace(")","");
    temp.replace(","," ");
    res.append(separateCoords(temp)); //Separate the coordinates into String items
    return res;
}

bool writeShapefile::createShapeFile()
{

    /*

      This function creates the shape file

    */
    bool success;
    success = true;
    QString dbfFile;
    if (m_shapeFile.indexOf(".shp") == 0)
    {
        m_shapeFile = m_shapeFile + ".shp";
    }

    dbfFile = m_shapeFile.replace(".shp",".dbf");

    if (QFile::exists(m_shapeFile))
    {
        if (!m_overWrite)
        {
            gbtLog2("Shape output file already exists. To overwrite use -O");
            return false;
        }
        else
        {
            QString temp;


            //Remove the primary files
            if (!QFile::remove(m_shapeFile))
                success = false;
            temp = m_shapeFile.replace(".shp",".dbf");
            if (!QFile::remove(temp))
                success = false;
            temp = m_shapeFile.replace(".shp",".shx");
            if (!QFile::remove(temp))
                success = false;
            //Remove secondary files
            temp = m_shapeFile.replace(".shp",".sbn");
            QFile::remove(temp);
            temp = m_shapeFile.replace(".shp",".sbx");
            QFile::remove(temp);
            temp = m_shapeFile.replace(".shp",".shp.xml");
            QFile::remove(temp);
        }
    }
    SHPHandle hSHP;
    DBFHandle hDBF;
    int	nShapeType;

    nShapeType = -1;

    QByteArray bshp = m_shapeFile.toLocal8Bit();
    char *pshapeFile = bshp.data();

    QSqlQuery query(m_database);
    QString sql;
    sql = "SELECT shapefiletype FROM datasetinfo WHERE dataset_id = '" + m_tableName + "'";
    if (query.exec(sql))
    {
        if (query.first())
        {
            nShapeType = query.value(0).toInt();
        }
        else
        {
            gbtLog2("Error selecting dataset");
            gbtLog2(query.lastError().databaseText());
            return false;
        }
    }
    else
    {
        gbtLog2("Error selecting dataset");
        gbtLog2(query.lastError().databaseText());
        return false;
    }


    if (nShapeType < 0 || nShapeType > 31)
    {
        gbtLog2("Shaoe type error");
        return false;
    }

    hSHP = SHPCreate( pshapeFile, nShapeType );
    if( hSHP == NULL )
    {
        gbtLog2("Unable to create: " + m_shapeFile);
        return false;
    }

    QByteArray bdbf = dbfFile.toLocal8Bit();
    char *pdbfFile = bdbf.data();

    hDBF = DBFCreate(pdbfFile);
    if( hDBF == NULL )
    {
        gbtLog2("Unable to create dbf: " + dbfFile);
        SHPClose( hSHP );
        return false;
    }

    //Create the structure of the DBF

    sql = "DESC " + m_tableName;
    query.exec(sql);
    QString fieldName;
    QString fieldType;

    DBFFieldType eType;
    int size;
    int prec;
    int fieldpos;
    fieldpos = -1;

    QList<TfieldDef> fields;

    while (query.next())
    {

        eType = FTString;
        size = 10;
        prec = 0;
        fieldName = query.value(getFieldIndex(query,"Field")).toString();
        fieldType = query.value(getFieldIndex(query,"Type")).toString();

        if (fieldName == "shapeid" || fieldName == "ogc_geom" || fieldName == "num_cells")
            continue;

        if (fieldType.contains("double",Qt::CaseInsensitive))
        {
            eType = FTDouble;
            size = 7;
            prec = 3;
        }
        if (fieldType.contains("float",Qt::CaseInsensitive))
        {
            eType = FTDouble;
            size = 7;
            prec = 3;
        }
        if (fieldType.contains("char",Qt::CaseInsensitive))
        {
            eType = FTString;
            getFieldSize(fieldType,size,prec);
        }
        if (fieldType.contains("bool",Qt::CaseInsensitive))
        {
            eType = FTLogical;
            size = 0;
            prec = 0;
        }
        if (fieldType.contains("int",Qt::CaseInsensitive))
        {
            eType = FTInteger;
            getFieldSize(fieldType,size,prec);
            prec = 0;
        }
        if (fieldType.contains("decimal",Qt::CaseInsensitive))
        {
            eType = FTDouble;
            getFieldSize(fieldType,size,prec);
        }

        if( DBFAddField( hDBF, fieldName.toLocal8Bit().data(), eType, size, prec ) == -1 )
        {
            gbtLog2("Unable to create dbf: " + dbfFile);
            DBFClose(hDBF);
            SHPClose(hSHP);
            return false;
        }
        else
        {
            fieldpos++;
            TfieldDef field;
            field.fieldName = fieldName;
            field.fieldType = eType;
            field.fieldPos = fieldpos;
            fields.append(field);
        }
    }

    TfieldDef cfield;
    if (m_outputType == "c")
    {
        fieldpos++;
        cfield.fieldName = "classCode";
        cfield.fieldType = FTInteger;
        cfield.fieldPos = fieldpos;
        fields.append(cfield);

        if( DBFAddField( hDBF, "classCode", FTInteger, 10, 0 ) == -1 )
        {
            gbtLog2("Unable to create dbf: " + dbfFile);
            DBFClose(hDBF);
            SHPClose(hSHP);
            return false;
        }
    }
    if (m_outputType == "b")
    {
        fieldpos++;
        cfield.fieldName = "comCode";
        cfield.fieldType = FTInteger;
        cfield.fieldPos = fieldpos;
        fields.append(cfield);

        if( DBFAddField( hDBF, "comCode", FTInteger, 10, 0 ) == -1 )
        {
            gbtLog2("Unable to create dbf: " + dbfFile);
            DBFClose(hDBF);
            SHPClose(hSHP);
            return false;
        }
    }

    //Insert the data into the DBF

    int recNo;
    int pos;

    if (query.exec(m_sql))
    {
        recNo = -1;
        while (query.next())
        {
            recNo++;
            for (pos = 0; pos <= fields.count()-1;pos++)
            {
                if (fields[pos].fieldType == FTDouble)
                {
                    DBFWriteDoubleAttribute(hDBF,recNo,fields[pos].fieldPos,query.value(getFieldIndex(query,fields[pos].fieldName)).toDouble());
                }
                if (fields[pos].fieldType == FTInteger)
                {
                    DBFWriteIntegerAttribute(hDBF,recNo,fields[pos].fieldPos,query.value(getFieldIndex(query,fields[pos].fieldName)).toInt());
                }
                if (fields[pos].fieldType == FTString)
                {
                    DBFWriteStringAttribute(hDBF,recNo,fields[pos].fieldPos,query.value(getFieldIndex(query,fields[pos].fieldName)).toString().toLocal8Bit().data());
                }
                if (fields[pos].fieldType == FTLogical)
                {
                    if (query.value(getFieldIndex(query,fields[pos].fieldName)).toInt() == 0)
                        DBFWriteLogicalAttribute(hDBF,recNo,fields[pos].fieldPos,'F');
                    else
                        DBFWriteLogicalAttribute(hDBF,recNo,fields[pos].fieldPos,'T');
                }
            }
        }
    }
    else
    {
        gbtLog2("Error while loading data");
        gbtLog2(query.lastError().databaseText());
    }

    //Closes the DBF and SHP handles.
    DBFClose(hDBF);
    SHPClose(hSHP);

    //Process the geometry and creates the shapes using Frank Warmerdam's shpadd program
    query.first();
    int nparams;

    QStringList cadena;
    char **output;
    while (query.isValid())
    {
        //Add the parameters to a string list
        if (!query.value(getFieldIndex(query,"shpgeometry")).toString().simplified().isEmpty())
        {
            cadena.append("goblet");
            cadena.append(m_shapeFile);
            cadena.append(getGeometry(query.value(getFieldIndex(query,"shpgeometry")).toString()));
        }


        //Convert the stringlist to a char**
        output = new char*[cadena.size() + 1];
        int i;
        for (i = 0; i < cadena.size(); i++)
        {
            output[i] = new char[strlen(cadena.at(i).toStdString().c_str())+1];
            memcpy(output[i], cadena.at(i).toStdString().c_str(), strlen(cadena.at(i).toStdString().c_str())+1);
        }
        output[cadena.size()] = ((char)NULL);
        //Get the number of parameters
        nparams = cadena.count();

        //Basically we run shpadd like in the command line.
        shpadd(nparams,output);

        //Clear the char** and the string list

        for (i = 0; i < cadena.size(); i++)
        {
            delete output[i];
        }
        delete output;

        cadena.clear();


        query.next();
    }


    return success;
}
