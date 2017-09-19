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
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QTextStream>

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

QString getWhereClauseFromPoint(QString point, QSqlDatabase db, QString table)
{

    //Get UpperLeft
    int pos;
    QPointF dupperLeft;
    pos = point.indexOf(",");
    dupperLeft.setX(point.left(pos).toDouble());
    dupperLeft.setY(point.mid(pos+1,point.length()-pos+1).toDouble());


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

    int xpos;
    int ypos;

    if(dbCellSize != 0.0)
    {
        //Plots the bounds in the grid
        upperLeft.setX(round( ( dupperLeft.x() - xllCenter )/dbCellSize ));
        upperLeft.setY(NumberRows - round( ( dupperLeft.y() - yllCenter )/dbCellSize ) - 1);

        //Plots the bounds in the GOBLET coordinate system
        ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + NumberRows)) + 1;
        xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

        upperLeft.setX(xpos + upperLeft.x());
        upperLeft.setY(ypos + upperLeft.y());

        //Crete the where clause

        QString idsuit;
        idsuit = getStrValue(upperLeft.y()) + getStrValue(upperLeft.x());

        QString where;
        where = table + ".geokey = '" + idsuit + "'";

        return where;

    }

    return QString();
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
        where = table + ".geokey BETWEEN '" + idsuit + "' AND ";

        idsuit = getStrValue(lowerRight.y()) + getStrValue(lowerRight.x());
        where = where + "'" + idsuit + "'";
        return where;

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

int constructSQL(QString calculation, QString &resultSQL, QSqlDatabase db, QString extent, QString shapes, QString point, QString tableName)
{

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



    int dsetcount;
    dsetcount = 1;

    //Replaces all datasets in the calculation to its cellValue
    QString calcstr;
    calcstr = calculation.replace(" ",""); //Remove all spaces


    //Begin the construct of the result SQL

    QString resultSelect;
    if (calcstr != "VAL")
        resultSelect = "SELECT " + calcstr + "("  + tableName + ".cellvalue)";
    else
        resultSelect = "SELECT " + tableName + ".cellvalue";

    QString resultFrom;
    resultFrom = " FROM " + tableName;

    QString resultWhere;


    if (!point.isEmpty())
    {
        QString pointWhere;
        pointWhere = getWhereClauseFromPoint(point,db,tableName);
        if (!pointWhere.isEmpty())
        {
            if (pointWhere.contains(" WHERE "))
                resultWhere = resultWhere + " AND " + pointWhere;
            else
                resultWhere = resultWhere + " WHERE " + pointWhere;
        }
    }
    else
    {
        QString extentWhere;
        if (!extent.isEmpty())
        {
            extentWhere = getWhereClauseFromExtent(extent,db,tableName);
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
                    extentClause.replace(tableName+".","");

                    if (!extentClause.isEmpty())
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                    else
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                    //gbtLog(QObject::tr("Preselecting shapes"));
                    //gbtLog(sqlcreate);
                    if (qry.exec(sqlcreate))
                    {
                        dsetcount++;
                        resultFrom = resultFrom + ",tmpshapes T" + letters[dsetcount];

                        if (resultWhere.contains(" WHERE "))
                            resultWhere = resultWhere + " AND " + tableName + ".geokey = T" + letters[dsetcount] + ".geokey";
                        else
                            resultWhere = resultWhere + " WHERE " + tableName + ".geokey = T" + letters[dsetcount] + ".geokey";
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
    TCLAP::CmdLine cmd("GOBLET (c) 2017, Bioversity International \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> calculationArg("c","calculation","Dataset calculation to perform. For example: 'VAL,MAX,MIN,AVG,SUM or STD' ",true,"","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name used",true,"","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");

    //Non required arguments
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> pointArg("n","point","Point: 'lat,log'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint classification using shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");
    TCLAP::ValueArg<std::string> formatArg("f","format","Format: (h)uman or (c)omputer",false,"h","string");


    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(calculationArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(pointArg);
    cmd.add(formatArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString format = QString::fromUtf8(formatArg.getValue().c_str());
    QString calculation = QString::fromUtf8(calculationArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    QString point = QString::fromUtf8(pointArg.getValue().c_str());

    calculation = calculation.toUpper().trimmed().simplified();

    if ((extent == "") && (shapes == "") && (point == ""))
    {
        gbtLog(QObject::tr("Invalid calculation. You need to indicate either an extend, a shape or a point for extraction"));
        return 1;
    }

    if (extent != "")
    {
        if ((calculation != "MAX") && (calculation != "MIN") && (calculation != "SUM") && (calculation != "STD") && (calculation != "AVG"))
        {
            gbtLog(QObject::tr("Invalid calculation. When using an extend you can only use MAX, MIN, SUM, STD OR AVG"));
            return 1;
        }
    }

    if (shapes != "")
    {
        if ((calculation != "MAX") && (calculation != "MIN") && (calculation != "SUM") && (calculation != "STD") && (calculation != "AVG"))
        {
            gbtLog(QObject::tr("Invalid calculation. When using an shape you can only use MAX, MIN, SUM, STD OR AVG"));
            return 1;
        }
    }

    if (point != "")
    {
        if (calculation != "VAL")
        {
            gbtLog(QObject::tr("Invalid calculation. When using a point you can only use VAL"));
            return 1;
        }
    }

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


        QString mainsql;
        QSqlQuery qry(mydb);

        if (constructSQL(calculation,mainsql,mydb,extent,shapes,point,tableName))
        {
            gbtLog(QObject::tr("Error in calculation"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }


        if (qry.exec(mainsql))
        {
            qry.first();
            if (format == "h")
                gbtLog("Result: " + qry.value(0).toString());
            else
            {
                QDomDocument doc;
                doc = QDomDocument("GOBLETXML");
                QDomElement root;
                root = doc.createElement("ResultXML");
                root.setAttribute("version", "1.0");
                doc.appendChild(root);

                QDomElement resultde;
                resultde = doc.createElement("result");
                QDomText varValue;
                varValue = doc.createTextNode(qry.value(0).toString());
                resultde.appendChild(varValue);
                root.appendChild(resultde);

                QTextStream out(stdout);
                out.setCodec("UTF-8");
                doc.save(out,1,QDomNode::EncodingFromTextStream);

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

        if (format == "h")
            gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();


        return 0;
    }

    return 0;

}
