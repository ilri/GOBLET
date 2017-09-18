#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QCoreApplication>
#include <QPoint>
#include <QPointF>
#include <math.h>
#include <QTextStream>
#include <QtCore/QCoreApplication>
#include <QVector>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}


struct datasetInfo
{
  QString name;
  QString code;
};
typedef datasetInfo TdatasetInfo;

QString firstGrid;

bool datasetExists(QString dataset, QSqlDatabase db)
{
    QSqlQuery qry(db);
    QString sql;
    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
                return false;
            else
                return true;
        }
        else
            return false;
    }
    else
        return false;

}

int findDataset(QString dataset, QList <TdatasetInfo> list)
{
    for (int pos = 0; pos <= list.count()-1;pos++)
    {
        if (list[pos].name == dataset)
            return 1;
    }
    return 0;
}

QString getStrValue(int value)
{
  int pos;
  QString svalue;
  QString tvalue;

  svalue = QString::number(value);
  tvalue = svalue;
  for (pos = 1; pos <= 7-tvalue.length(); pos++)
  {
    svalue = "0" + svalue;
  }
  return svalue;
}

QString getWhereClauseFromExtent(QString extent,QSqlDatabase db, QString table)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (extent.count(" ") != 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count(",") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count("(") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count(")") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    int pos;
    pos = extent.indexOf(" ");
    QString from;
    QString to;
    from = extent.left(pos);
    to = extent.mid(pos+1,extent.length()-pos+1);
    from.replace("(","");
    from.replace(")","");
    to.replace("(","");
    to.replace(")","");

    //Get UpperLeft
    QPointF dupperLeft;
    pos = from.indexOf(",");
    dupperLeft.setX(from.left(pos).toDouble());
    dupperLeft.setY(from.mid(pos+1,from.length()-pos+1).toDouble());


    //Get lower right
    QPointF dlowerRight;
    pos = to.indexOf(",");
    dlowerRight.setX(to.left(pos).toDouble());
    dlowerRight.setY(to.mid(pos+1,to.length()-pos+1).toDouble());

    QSqlQuery qry(db);
    QString sql;
    double dbCellSize;
    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return QString();
    }
    else
        return QString();

    if (dbCellSize == 0.0)
        return QString();
    double xllCenter;
    double yllCenter;
    int NumberRows;
    sql = "SELECT xllCenter,yllCenter,nrows FROM datasetinfo WHERE dataset_id = '" + table + "'";
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            xllCenter = qry.value(0).toDouble();
            yllCenter = qry.value(1).toDouble();
            NumberRows = qry.value(2).toInt();
        }
        else
            return QString();
    }
    else
        return QString();

    QPoint upperLeft;
    QPoint lowerRight;

    int xpos;
    int ypos;

    if(dbCellSize != 0.0)
    {
        //Plots the bounds in the grid
        upperLeft.setX(round( ( dupperLeft.x() - xllCenter )/dbCellSize ));
        upperLeft.setY(NumberRows - round( ( dupperLeft.y() - yllCenter )/dbCellSize ) - 1);

        lowerRight.setX(round( ( dlowerRight.x() - xllCenter )/dbCellSize ));
        lowerRight.setY(NumberRows - round( ( dlowerRight.y() - yllCenter )/dbCellSize ) - 1);

        //Plots the bounds in the GOBLET coordinate system
        ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + NumberRows)) + 1;
        xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

        upperLeft.setX(xpos + upperLeft.x());
        upperLeft.setY(ypos + upperLeft.y());

        lowerRight.setX(xpos + lowerRight.x());
        lowerRight.setY(ypos + lowerRight.y());

        //Crete the where clause

        QString idsuit;
        idsuit = getStrValue(upperLeft.y()) + getStrValue(upperLeft.x());

        QString where;
        where = " TA.geokey BETWEEN '" + idsuit + "' AND ";

        idsuit = getStrValue(lowerRight.y()) + getStrValue(lowerRight.x());
        where = where + "'" + idsuit + "'";
        return where;

        //xpos = xpos + shapePixels[pos].col; //-1
        //ypos = ypos + shapePixels[pos].row; //-1

    }

    return QString();
}

QString getShapeClause(QString shapeConstraint,QSqlDatabase db)
{
    //ShapeDataSet:shapeID,ShapeID,
    if (shapeConstraint.count(":") != 1)
    {
        gbtLog(QObject::tr("Error in shape constraint"));
        return QString();
    }

    int pos;
    pos = shapeConstraint.indexOf(":");

    QString dataset = shapeConstraint.left(pos);

    QSqlQuery qry(db);
    QString sql;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(dataset + QObject::tr(" is not a shape dataset"));
                return QString();
            }
        }
        else
            return QString();
    }
    else
        return QString();

    sql = "SELECT geokey FROM " + dataset + "_raster WHERE ";
    QString shapes = shapeConstraint.mid(pos+1,shapeConstraint.length()-pos+1);
    QString shape;

    pos =0;
    while (pos <= shapes.length()-1)
    {
        if (shapes[pos] != ',')
        {
            shape = shape + shapes[pos];
            pos++;
        }
        else
        {
            sql = sql + " shapeid = " + shape + " OR ";
            shape = "";
            shapes = shapes.mid(pos+1,shapes.length()-pos+1);
            pos = 0;
        }
    }
    sql = sql + "shapeid = " + shape;

    return sql;
}

int constructSQL(QString calculation, QString &resultSQL, QSqlDatabase db, QString extent, QString shapes)
{
    int openB;
    int closeB;

    openB = calculation.count("(");
    closeB = calculation.count(")");

    if (openB != closeB)
        return 1;


    QStringList letters;

    letters.append("A");
    letters.append("B");
    letters.append("C");
    letters.append("D");
    letters.append("E");
    letters.append("F");
    letters.append("G");
    letters.append("H");
    letters.append("I");
    letters.append("J");
    letters.append("K");
    letters.append("L");
    letters.append("M");
    letters.append("N");
    letters.append("O");
    letters.append("P");
    letters.append("Q");
    letters.append("R");
    letters.append("S");
    letters.append("T");
    letters.append("U");
    letters.append("V");
    letters.append("W");
    letters.append("X");
    letters.append("Y");
    letters.append("Z");

    QList <TdatasetInfo> datasets;
    QString calcstr;


    calcstr = calculation.replace(" ",""); //Remove all spaces
    //Replace all operands with spaces;
    calcstr = calcstr.replace("("," ");
    calcstr = calcstr.replace(")"," ");
    calcstr = calcstr.replace("+"," ");
    calcstr = calcstr.replace("-"," ");
    calcstr = calcstr.replace("*"," ");
    calcstr = calcstr.replace("/"," ");
    calcstr = calcstr.simplified(); //Remove spaces from start / end


    //Extract datasets
    int pos;
    QString dataset;
    int dsetcount;
    dsetcount = -1;
    for (pos = 0; pos <= calcstr.length()-1;pos++)
    {
        if (calcstr[pos] != ' ')
        {
            dataset = dataset + calcstr[pos];
        }
        else
        {
            dataset = dataset.simplified();
            if (!dataset.isEmpty())
            {
                if (datasetExists(dataset,db))
                {
                    if (!findDataset(dataset,datasets))
                    {
                        dsetcount++;
                        TdatasetInfo dset;
                        dset.name = dataset;
                        dset.code = letters[dsetcount];
                        datasets.append(dset);
                    }
                }
            }
            dataset = "";
        }
    }
    dataset = dataset.simplified();
    if (!dataset.isEmpty())
    {
        if (datasetExists(dataset,db))
        {
            if (!findDataset(dataset,datasets))
            {
                dsetcount++;
                TdatasetInfo dset;
                dset.name = dataset;
                dset.code = letters[dsetcount];
                datasets.append(dset);
            }
        }
    }

    if (datasets.count() == 0)
        return 1;

    firstGrid = datasets[0].name;

    //Replaces all datasets in the calculation to its cellValue
    calcstr = calculation.replace(" ",""); //Remove all spaces
    for (pos = 0; pos <= datasets.count()-1;pos++)
    {
        calcstr = calcstr.replace(datasets[pos].name,"T" + datasets[pos].code + ".cellvalue");
    }

    //Begin the construct of the result SQL

    QString resultSelect;
    resultSelect = "SELECT T" + datasets[0].code + ".geokey, T" + datasets[0].code + ".xpos,T" +  datasets[0].code + ".ypos," + calcstr;

    QString resultFrom;
    resultFrom = " FROM ";

    for (pos = 0; pos <= datasets.count()-1;pos++)
    {
        resultFrom = resultFrom + datasets[pos].name + " AS T" + datasets[pos].code + ",";
    }
    resultFrom = resultFrom.left(resultFrom.length()-1);

    QString resultWhere;


    //Link the tables
    if (datasets.count() > 1)
    {
        resultWhere = " WHERE ";
        for (pos = 1; pos <= datasets.count()-1;pos++)
        {
            resultWhere = resultWhere + "T" + datasets[pos-1].code + ".geokey = T" + datasets[pos].code + ".geokey AND ";
        }
        resultWhere = resultWhere.left(resultWhere.length()-5);
    }

    QString extentWhere;

    if (!extent.isEmpty())
    {
        extentWhere = getWhereClauseFromExtent(extent,db,datasets[0].name);
        if (!extentWhere.isEmpty())
        {
            if (resultWhere.contains(" WHERE "))
                resultWhere = resultWhere + " AND " + extentWhere;
            else
                resultWhere = resultWhere + " WHERE " + extentWhere;
        }
    }

    if (!shapes.isEmpty())
    {
        QString shapeClause;

        shapeClause = getShapeClause(shapes,db);


        if (!shapeClause.isEmpty())
        {
            //
            QSqlQuery qry(db);

            QString sqlcreate;
            sqlcreate = "CREATE TEMPORARY TABLE tmpshapes (";
            sqlcreate = sqlcreate + "geokey VARCHAR(14) NOT NULL ,";
            sqlcreate = sqlcreate + "PRIMARY KEY (geokey))";
            sqlcreate = sqlcreate + " ENGINE = MyISAM";
            if (qry.exec(sqlcreate))
            {
                QString extentClause;
                extentClause = extentWhere;
                extentClause.replace("TA.","");

                if (!extentClause.isEmpty())
                    sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                else
                    sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                gbtLog(QObject::tr("Preselecting shapes"));
                if (qry.exec(sqlcreate))
                {
                    dsetcount++;
                    resultFrom = resultFrom + ", tmpshapes T" + letters[dsetcount];

                    if (resultWhere.contains(" WHERE "))
                        resultWhere = resultWhere + " AND TA.geokey = T" + letters[dsetcount] + ".geokey";
                    else
                        resultWhere = resultWhere + " WHERE TA.geokey = T" + letters[dsetcount] + ".geokey";


                }
                else
                {
                    gbtLog(QObject::tr("Cannot insert temporary shapes."));
                    gbtLog(qry.lastError().databaseText());
                }
            }
            else
            {
                gbtLog(QObject::tr("Cannot shapes temporary table."));
                gbtLog(qry.lastError().databaseText());
            }
        }
    }


    resultSQL = resultSelect + resultFrom + resultWhere;

    return 0;

}

//Latest and most updated version
int getGridValuesFromExtent(QString extent,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (extent.count(" ") != 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count(",") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count("(") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count(")") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    int pos;
    pos = extent.indexOf(" ");
    QString from;
    QString to;
    from = extent.left(pos);
    to = extent.mid(pos+1,extent.length()-pos+1);
    from.replace("(","");
    from.replace(")","");
    to.replace("(","");
    to.replace(")","");

    //Get UpperLeft
    QPointF dupperLeft;
    pos = from.indexOf(",");
    dupperLeft.setX(from.left(pos).toDouble());
    dupperLeft.setY(from.mid(pos+1,from.length()-pos+1).toDouble());


    //Get lower right
    QPointF dlowerRight;
    pos = to.indexOf(",");
    dlowerRight.setX(to.left(pos).toDouble());
    dlowerRight.setY(to.mid(pos+1,to.length()-pos+1).toDouble());

    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;

    if (dbCellSize == 0.0)
        return 1;



    //*************************
    double v_xMin;
    double v_xMax;
    double v_yMin;
    double v_yMax;
    double halfx;
    int v_cols;
    int v_rows;


    v_xMin = trunc(( dupperLeft.x() *1.0/ dbCellSize )) * dbCellSize;
    v_yMin = trunc(( dlowerRight.y() *1.0/ dbCellSize )) * dbCellSize;
    v_cols = trunc( ( dlowerRight.x() - dupperLeft.x() ) *1.0/ dbCellSize ) + 1;
    v_rows = trunc( ( dupperLeft.y() - dlowerRight.y() ) *1.0/ dbCellSize ) + 1;
    v_xMax = v_xMin + v_cols * dbCellSize;
    v_yMax = v_yMin + v_rows * dbCellSize;

    halfx = dbCellSize *1.0/ 2;

    xllCenter = v_xMin + halfx;
    yllCenter = (v_yMin + halfx) - 0.0200;
    nrows = trunc( fabs( v_yMax - v_yMin ) *1.0/ dbCellSize );
    ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);

    //*******************

    left = ceil((dupperLeft.x() + 180) / dbCellSize);
    bottom = ceil((90 - dupperLeft.y()) / dbCellSize);

    return 0;


}

//Latest and most updated version
int calcBoundFromShapes(QString shapeConstraint,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    //ShapeDataSet:shapeID,ShapeID,
    if (shapeConstraint.count(":") != 1)
    {
        gbtLog(QObject::tr("Error in shape constraint"));
        return 1;
    }

    int pos;
    pos = shapeConstraint.indexOf(":");

    QString dataset = shapeConstraint.left(pos);

    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr(" is not a shape dataset"));
                return 1;
            }
        }
        else
            return 1;
    }
    else
        return 1;

    sql = "SELECT shapeid,AsText(max(envelope(ogc_geom))) as boundbox FROM " + dataset + " WHERE ";
    QString shapes = shapeConstraint.mid(pos+1,shapeConstraint.length()-pos+1);
    QString shape;

    pos =0;
    while (pos <= shapes.length()-1)
    {
        if (shapes[pos] != ',')
        {
            shape = shape + shapes[pos];
            pos++;
        }
        else
        {
            sql = sql + " shapeid = " + shape + " OR ";
            shape = "";
            shapes = shapes.mid(pos+1,shapes.length()-pos+1);
            pos = 0;
        }
    }
    sql = sql + "shapeid = " + shape;
    sql = sql + " GROUP BY shapeid";

    double maxX;
    double maxY;
    double minX;
    double minY;

    maxX = -3000;
    maxY = -3000;
    minX = 3000;
    minY = 3000;


    QString cadena;
    QString geopos;

    double temp;
    if (qry.exec(sql))
    {
        while (qry.next())
        {
            cadena = qry.value(1).toString();
            pos = cadena.indexOf("(");
            cadena = cadena.mid(pos+1,cadena.length()-pos+1); //Remove the type of shape
            cadena = cadena.replace("(","");
            cadena = cadena.replace(")","");
            pos = 0;
            while (pos <= cadena.length()-1)
            {
                if (cadena[pos] != ',')
                {
                    geopos = geopos + cadena[pos];
                    pos++;
                }
                else
                {
                    cadena = cadena.mid(pos+1,cadena.length()-pos+1);

                    pos = geopos.indexOf(" ");
                    temp = geopos.left(pos).toDouble();
                    if (temp > maxX)
                        maxX = temp;
                    if (temp < minX)
                        minX = temp;

                    temp = geopos.mid(pos+1,geopos.length()-pos+1).toDouble();

                    if (temp > maxY)
                        maxY = temp;
                    if (temp < minY)
                        minY = temp;

                    pos = 0;
                    geopos = "";
                }
            }

            //Final string
            pos = geopos.indexOf(" ");
            temp = geopos.left(pos).toDouble();
            if (temp > maxX)
                maxX = temp;
            if (temp < minX)
                minX = temp;

            temp = geopos.mid(pos+1,geopos.length()-pos+1).toDouble();

            if (temp > maxY)
                maxY = temp;
            if (temp < minY)
                minY = temp;

        }
        QPointF UL;
        QPointF LR;

        UL.setX(minX);
        UL.setY(maxY);

        LR.setX(maxX);
        LR.setY(minY);


        double v_xMin,v_xMax,v_yMin,v_yMax;
        double halfx;
        int v_cols,v_rows;

        v_xMin = trunc(UL.x() / dbCellSize) * dbCellSize;
        v_yMin = trunc(LR.y() / dbCellSize) * dbCellSize;
        v_cols = trunc((LR.x() -  UL.x()) / dbCellSize) + 1;
        v_rows = trunc((UL.y() -  LR.y()) / dbCellSize) + 1;
        v_xMax = v_xMin + v_cols * dbCellSize;
        v_yMax = v_yMin + v_rows * dbCellSize;

        halfx = dbCellSize / 2;
        xllCenter = v_xMin + halfx;
        yllCenter = (v_yMin + halfx) - 0.0200;
        ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);
        nrows = trunc(fabs(v_yMax - v_yMin) / dbCellSize);

        left = ceil((UL.x() + 180) / dbCellSize);
        bottom = ceil((90 - UL.y()) / dbCellSize);


        return 0;

    }
    else
        return 1;
}

//Most updated version
int calcExtendFromGrid(QString gridName,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;



    sql = "SELECT xllcenter,yllcenter,ncols,nrows FROM datasetinfo WHERE dataset_id = '" + gridName + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            xllCenter = qry.value(0).toDouble();
            yllCenter = qry.value(1).toDouble() - 0.0200;
            ncols = qry.value(2).toInt();
            nrows = qry.value(3).toInt();
        }
        else
            return 1;
    }
    else
        return 1;

    int xpos;
    int ypos;
    ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + nrows)) + 1;
    xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

    sql = "SELECT max(xpos),max(ypos),min(xpos),min(ypos) FROM " + gridName;
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            //double utx;
            //double uty;

            //utx = ((qry.value(0).toInt() - xpos) * dbCellSize) + xllCenter;
            //uty = (((((qry.value(1).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            double ltx;
            double lty;

            ltx = ((qry.value(2).toInt() - xpos) * dbCellSize) + xllCenter;
            lty = (((((qry.value(3).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            QPointF UL;

            UL.setX(ltx);
            UL.setY(lty);


            left = ceil((UL.x() + 180) / dbCellSize);
            bottom = ceil((90 - UL.y()) / dbCellSize);

            return 0;

        }
        else
            return 1;
    }
    else
        return 1;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> calculationArg("c","calculation","Dataset calculation to perform. For example: '(DatasetA + DatasetB) / DatasetC' ",true,"","string");
    TCLAP::ValueArg<std::string> ouputArg("o","output","Output type: (f)ile or (d)ataset",true,"","string");

    //Non required arguments
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name if output is dataset",false,"datasetCalcOutput","string");
    TCLAP::ValueArg<std::string> datasetDescArg("s","datasetDescription","Dataset description if output is dataset",false,"Output of datasetCalc","string");
    TCLAP::ValueArg<std::string> gridFileArg("g","gridfile","Output grid file if output is file",false,"datasetcalcoutput.asc","string");



    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint classification using shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");

    //Switches
    TCLAP::SwitchArg overwriteSwitch("O","overwrite","Overwrite dataset if exists", cmd, false);

    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(datasetDescArg);
    cmd.add(gridFileArg);
    cmd.add(calculationArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(ouputArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());

    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString calculation = QString::fromUtf8(calculationArg.getValue().c_str());
    QString outputType = QString::fromUtf8(ouputArg.getValue().c_str());
    QString tableDesc = QString::fromUtf8(datasetDescArg.getValue().c_str());
    QString outputFile = QString::fromUtf8(gridFileArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    bool replace = overwriteSwitch.getValue();


    QSqlDatabase mydb;

    mydb = QSqlDatabase::addDatabase("QMYSQL","connection1");
    mydb.setHostName(host);
    mydb.setPort(port.toInt());
    if (!userName.isEmpty())
        mydb.setUserName(userName);
    if (!password.isEmpty())
        mydb.setPassword(password);


    mydb.setDatabaseName(dbName);

    if (!mydb.open())
    {
        gbtLog(QObject::tr("Cannot open database"));

        return 1;
    }
    else
    {
        QTime procTime;
        procTime.start();

        QString sql;
        QString mainsql;
        QSqlQuery qry(mydb);

        if (constructSQL(calculation,mainsql,mydb,extent,shapes))
        {
            gbtLog(QObject::tr("Error in calculation"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }

        //gbtLog(mainsql);



        double dbCellSize;
        int ncols,nrows;
        int left,bottom;
        double xllCenter,yllCenter;

        if (!extent.isEmpty())
        {
            if (getGridValuesFromExtent(extent,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
            {
                gbtLog(QObject::tr("Error in extent"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
        }
        else
        {
            if (!shapes.isEmpty())
            {
                if (calcBoundFromShapes(shapes,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
                {
                    gbtLog(QObject::tr("Error in constraining shapes"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }
            }
            else
            {
                if (calcExtendFromGrid(firstGrid,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
                {
                    gbtLog(QObject::tr("Error in grid extent"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }                
            }
        }
        //Create the grid

        if (outputType == "g")
        {

            int c;
            int r;

            QVector<QVector<float> >  grid;
            grid.resize(nrows);
            for(r=0; r<nrows; r++)
            {
                grid[r].resize(ncols);
            }

            for (r=0;r<=nrows-1;r++)
                for (c=0;c<= ncols-1;c++)
                    grid[r][c] = -9999.000;

            //Populating the grid

            int x;
            int y;

            gbtLog(mainsql);

            gbtLog(QObject::tr("Calculating values"));

            if (qry.exec(mainsql))
            {
                gbtLog(QObject::tr("Rasterizing"));
                int leftout;
                leftout = 0;
                while (qry.next())
                {
                    x = qry.value(1).toInt();
                    y = qry.value(2).toInt();

                    if (((x-left-1) >= 0) && ((x-left-1) <= (ncols-1)))
                    {
                        if (((y-bottom-1) >= 0) && ((y-bottom-1) <= (nrows -1)))
                        {
                            if (grid[y-bottom-1][x-left-1] != -9999)
                                leftout++;
                            grid[y-bottom-1][x-left-1] = qry.value(3).toFloat();
                        }
                        else
                            leftout++;
                    }
                    else
                        leftout++;
                }

                if (leftout > 0)
                     gbtLog("Cells left out: " + QString::number(leftout));


                QFile file(outputFile);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                    return -1;

                QTextStream out(&file);
                out << "ncols         " << ncols << "\n";
                out << "nrows         " << nrows << "\n";
                out << "xllcorner     " << xllCenter << "\n";
                out << "yllcorner     " << yllCenter << "\n";
                out << "cellsize      " << dbCellSize << "\n";
                out << "NODATA_value  " << -9999.000 << "\n";

                for (r=0;r<=nrows-1;r++)
                {
                    for (c=0;c<= ncols-1;c++)
                    {
                        out << QString::number(grid[r][c],'f',3) + " ";
                    }
                    out << "\n";
                }
                file.close();
            }
            else
            {
                gbtLog(QObject::tr("Cannot calculate"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
        }
        else
        {
            if (outputType == "d")
            {
                if (replace)
                {
                    sql = "DROP TABLE IF EXISTS " + tableName;
                    qry.exec(sql);

                    sql = "DELETE FROM datasetinfo WHERE dataset_id = '" + tableName + "'";
                    if (!qry.exec(sql))
                    {
                        gbtLog(QObject::tr("Cannot remove previous dataset."));
                        gbtLog(qry.lastError().databaseText());
                        mydb.close();

                        return 1;
                    }
                }

                sql = "CREATE TABLE " + tableName + " (";
                sql = sql + "geokey VARCHAR(14) NOT NULL ,";
                sql = sql + "xpos DECIMAL(7) NULL ,";
                sql = sql + "ypos DECIMAL(7) NULL ,";
                sql = sql + "cellvalue DECIMAL(14,5) NULL DEFAULT -9999 ,";
                sql = sql + "classCode BIGINT  NULL DEFAULT NULL ,";
                //sql = sql + "classColour VARCHAR(9) NULL DEFAULT NULL ,"; //,
                sql = sql + "PRIMARY KEY (geokey) ,";
                sql = sql + "INDEX " + tableName + "_IDXVALUE (cellvalue ASC)) ENGINE = MyISAM";


                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot create dataset. It might already exists"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }


                sql = "ALTER TABLE " + tableName + " DISABLE KEYS";
                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot disable keys"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }

                //mainsql = mainsql + " AND (TA.xpos - " + QString::number(left) + " -1) BETWEEN 0 AND (" + QString::number(ncols) + "-1)";

                //mainsql = mainsql + " AND (TA.ypos - " + QString::number(bottom) + "-1) BETWEEN 0 AND (" + QString::number(nrows) + "-1)";

                //gbtLog(mainsql);

                mainsql = "INSERT INTO " + tableName + "(geokey,xpos,ypos,cellvalue) " + mainsql;

                gbtLog(QObject::tr("Calculating values"));

                if (!qry.exec(mainsql))
                {
                    gbtLog(QObject::tr("Cannot enable keys"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }

                sql = "ALTER TABLE " + tableName + " ENABLE KEYS";
                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot enable keys"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }

                sql = "select count(distinct xpos),count(distinct ypos) from " + tableName;
                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot read dataset."));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }
                qry.first();

                ncols = qry.value(0).toInt();
                nrows = qry.value(1).toInt();

                sql = "select min(xpos),max(ypos) from " + tableName;
                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot read dataset."));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }
                qry.first();

                int xpos;
                int ypos;

                xpos = qry.value(0).toInt() -1;
                ypos = qry.value(1).toInt() -1;

                sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_type,ncols,nrows,xllcenter,yllcenter)";
                sql = sql + " VALUES ('" + tableName +"',";
                sql = sql + "'" + tableDesc +"',1,";
                sql = sql + QString::number(ncols) + ",";
                sql = sql + QString::number(nrows) + ",";
                sql = sql + QString::number(((xpos * dbCellSize) - 180),'f',15) + ",";
                sql = sql + QString::number((90 - (ypos * dbCellSize)),'f',15) + ")"; // + 0.0200 so its reduce

                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot insert dataset."));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }


            }
            else
            {
                gbtLog(QObject::tr("Invalid ouput format"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
        }





        int Hours;
        int Minutes;
        int Seconds;
        int Milliseconds;

        Milliseconds = procTime.elapsed();

        Hours = Milliseconds / (1000*60*60);
        Minutes = (Milliseconds % (1000*60*60)) / (1000*60);
        Seconds = ((Milliseconds % (1000*60*60)) % (1000*60)) / 1000;

        gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();


        return 0;
    }

    return 0;

}
