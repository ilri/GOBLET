#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QPoint>
#include <QPointF>
#include <math.h>
#include <QCoreApplication>
#include <cmath>
#include <QTextStream>

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

QString getWhereClauseFromExtent2(QString extent,QSqlDatabase db, QPointF &UL, QPointF &LR, double &dbCellSize)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (!extent.count(" ") == 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count(",") == 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count("(") == 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count(")") == 2)
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

    //*************************
    double v_xMin;
    //double v_xMax;
    double v_yMin;
    double v_yMax;
    double halfx;
    //int v_cols;
    int v_rows;


    v_xMin = trunc(( dupperLeft.x() *1.0/ dbCellSize )) * dbCellSize;
    v_yMin = trunc(( dlowerRight.y() *1.0/ dbCellSize )) * dbCellSize;
    //v_cols = trunc( ( dlowerRight.x() - dupperLeft.x() ) *1.0/ dbCellSize ) + 1;
    v_rows = trunc( ( dupperLeft.y() - dlowerRight.y() ) *1.0/ dbCellSize ) + 1;
    //v_xMax = v_xMin + v_cols * dbCellSize;
    v_yMax = v_yMin + v_rows * dbCellSize;

    halfx = dbCellSize *1.0/ 2;

    xllCenter = v_xMin + halfx;
    yllCenter = (v_yMin + halfx);
    NumberRows = trunc( fabs( v_yMax - v_yMin ) *1.0/ dbCellSize );

    //*******************
    QPoint upperLeft;
    QPoint lowerRight;

    int xpos;
    int ypos;

    if(dbCellSize != 0.0)
    {
        //Plots the bounds in the grid
        upperLeft.setX(round( ( dupperLeft.x() - xllCenter )/dbCellSize ));
        upperLeft.setY(NumberRows - round( ( dupperLeft.y() - yllCenter )/dbCellSize ) - 1);

        UL = dupperLeft;

        lowerRight.setX(round( ( dlowerRight.x() - xllCenter )/dbCellSize ));
        lowerRight.setY(NumberRows - round( ( dlowerRight.y() - yllCenter )/dbCellSize ) - 1);

        LR = dlowerRight;

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

    }

    return QString();
}

QString getWhereClauseFromExtent(QString extent,QSqlDatabase db, QString table, QPointF &UL, QPointF &LR, double &dbCellSize)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (!extent.count(" ") == 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count(",") == 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count("(") == 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (!extent.count(")") == 2)
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



        UL = dupperLeft;

        lowerRight.setX(round( ( dlowerRight.x() - xllCenter )/dbCellSize ));
        lowerRight.setY(NumberRows - round( ( dlowerRight.y() - yllCenter )/dbCellSize ) - 1);

        LR = dlowerRight;

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

    }

    return QString();
}

void calcBoundFromShapes(QString shapeConstraint,QSqlDatabase db, QPointF &UL, QPointF &LR, double &dbCellSize)
{
    //ShapeDataSet:shapeID,ShapeID,

    QString dataset;
    int pos;
    pos = 0;
    bool hasShapes;
    if (shapeConstraint.count(":") != 1)
    {
        dataset = shapeConstraint;
        hasShapes = false;
    }
    else
    {
        pos = shapeConstraint.indexOf(":");
        dataset = shapeConstraint.left(pos);
        hasShapes = true;
    }




    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return ;
    }
    else
        return;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr(" is not a shape dataset"));
                return;
            }
        }
        else
            return;
    }
    else
        return;

    if (hasShapes)
    {
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
    }
    else
    {
        sql = "SELECT shapeid,AsText(max(envelope(ogc_geom))) as boundbox FROM " + dataset + " GROUP BY shapeid ";
    }

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
        UL.setX(minX);
        UL.setY(maxY);

        LR.setX(maxX);
        LR.setY(minY);



    }
    else
        return;
}

QString getShapeClause(QString shapeConstraint,QSqlDatabase db)
{
    //ShapeDataSet:shapeID,ShapeID,
    QString dataset;
    int pos;
    pos = 0;
    bool hasShapes;
    if (shapeConstraint.count(":") != 1)
    {
        //gbtLog(QObject::tr("Error in shape constraint"));
        //return QString();
        dataset = shapeConstraint;
        hasShapes = false;
    }
    else
    {
        pos = shapeConstraint.indexOf(":");
        dataset = shapeConstraint.left(pos);
        hasShapes = true;
    }

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

    if (hasShapes)
    {
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
    }
    else
        sql = "SELECT geokey FROM " + dataset + "_raster";

    return sql;
}

void calcExtendFromGrid(QString gridName,QSqlDatabase db, QPointF &UL, QPointF &LR, double &dbCellSize)
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
            return ;
    }
    else
        return;

    float xllCenter;
    float yllCenter;
    //int ncols;
    int nrows;

    sql = "SELECT xllcenter,yllcenter,ncols,nrows FROM datasetinfo WHERE dataset_id = '" + gridName + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            xllCenter = qry.value(0).toDouble();
            yllCenter = qry.value(1).toDouble();
            //ncols = qry.value(2).toInt();
            nrows = qry.value(3).toInt();
        }
        else
            return;
    }
    else
        return;

    int xpos;
    int ypos;
    ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + nrows)) + 1;
    xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

    sql = "SELECT max(xpos),max(ypos),min(xpos),min(ypos) FROM " + gridName;
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            double utx;
            //double uty;

            utx = ((qry.value(0).toInt() - xpos) * dbCellSize) + xllCenter;
            //uty = (((((qry.value(1).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            double ltx;
            double lty;

            ltx = ((qry.value(2).toInt() - xpos) * dbCellSize) + xllCenter;
            lty = (((((qry.value(3).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            UL.setX(ltx);
            UL.setY(lty);

            LR.setX(utx);
            LR.setY(utx);
        }
        else
            return;
    }
    else
        return;

}

QString getClasses(QString classdef,QString field)
{
    int pos;

    QString temp;
    temp = classdef;
    pos = 0;
    QString classCode;
    QString sql;
    while (pos <= temp.length()-1)
    {
        if (temp[pos] != ',')
        {
            classCode = classCode + temp[pos];
            pos++;
        }
        else
        {
            if (!classCode.isEmpty())
                sql = sql + "TA." + field + " = " + classCode + " OR ";
            temp = temp.mid(pos+1,temp.length()-pos+1);
            pos = 0;
            classCode = "";
        }
    }
    if (!classCode.isEmpty())
        sql = sql + "TA." + field + " = " + classCode;

    return sql;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    TCLAP::ValueArg<std::string> datasetTypeArg("i","inputDatasetType","Input Dataset type: (g)rid, (c)lassification, com(b)ination",true,"","string");
    TCLAP::ValueArg<std::string> outputFileArg("o","outputFile","output ascii grid file name",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint output using a shapefile and shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");
    TCLAP::ValueArg<std::string> classesArg("c","combinationtoshow","Combination/classification to show: 'Combination/Class Code,Combination/Class Code,....'",false,"","string");

    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(datasetTypeArg);
    cmd.add(outputFileArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(classesArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    bool remote = remoteSwitch.getValue();
    QString path = QString::fromUtf8(pathArg.getValue().c_str());
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    QString datasetType = QString::fromUtf8(datasetTypeArg.getValue().c_str());
    QString outputFile = QString::fromUtf8(outputFileArg.getValue().c_str());
    QString classestoShow = QString::fromUtf8(classesArg.getValue().c_str());

    myDBConn con;
    QSqlDatabase mydb;
    if (!remote)
    {
        QDir dir;
        dir.setPath(path);
        if (con.connectToDB(dir.absolutePath()) == 1)
        {
            if (!dir.cd(dbName))
            {
                gbtLog(QObject::tr("The database does not exists"));
                con.closeConnection();
                return 1;
            }
            mydb = QSqlDatabase::addDatabase(con.getDriver(),"connection1");
        }
    }
    else
    {
        mydb = QSqlDatabase::addDatabase("QMYSQL","connection1");
        mydb.setHostName(host);
        mydb.setPort(port.toInt());
        if (!userName.isEmpty())
            mydb.setUserName(userName);
        if (!password.isEmpty())
            mydb.setPassword(password);
    }

    mydb.setDatabaseName(dbName);

    if (!mydb.open())
    {
        gbtLog(QObject::tr("Cannot open database"));
        con.closeConnection();
        return 1;
    }
    else
    {

        int ncols,nrows;
        int left,bottom;
        double xllCenter,yllCenter;

        xllCenter = 0.0;
        yllCenter = 0.0;
        ncols = 0;
        nrows = 0;

        QTime procTime;
        procTime.start();

        QString sql;
        QSqlQuery qry(mydb);

        QPointF upperLeft;
        QPointF lowerRight;
        double dbCellSize;

        QString whereClause;
        QString shapeClause;

        if (datasetType == "g" || datasetType == "c")
        {
            if (!extent.isEmpty())
                whereClause = getWhereClauseFromExtent(extent,mydb,tableName,upperLeft,lowerRight,dbCellSize);
        }
        else
        {
            if (!extent.isEmpty())
                whereClause = getWhereClauseFromExtent2(extent,mydb,upperLeft,lowerRight,dbCellSize);
        }

        QString shapeFromSQL;
        QString shapeWhereSQL;
        if (!shapes.isEmpty())
        {

            shapeClause = getShapeClause(shapes,mydb);

            if (!shapeClause.isEmpty())
            {

                QString sqlcreate;
                sqlcreate = "CREATE TEMPORARY TABLE tmpshapes (";
                sqlcreate = sqlcreate + "geokey VARCHAR(14) NOT NULL ,";
                sqlcreate = sqlcreate + "PRIMARY KEY (geokey))";
                sqlcreate = sqlcreate + " ENGINE = MyISAM";
                if (qry.exec(sqlcreate))
                {
                    QString extentClause;
                    extentClause = whereClause;
                    extentClause.replace("TA.","");

                    if (!extentClause.isEmpty())
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                    else
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                    gbtLog(QObject::tr("Preselecting shapes"));
                    if (qry.exec(sqlcreate))
                    {
                        shapeFromSQL = ", tmpshapes TB";
                        shapeWhereSQL = " TA.geokey = TB.geokey";
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

        // This are the basic parameters of the grid
        // calculated from the extent given or from the edata


        if (!whereClause.isEmpty())
        {
            //We have an extent

            double v_xMin,v_xMax,v_yMin,v_yMax;
            double halfx;
            int v_cols,v_rows;

            v_xMin = trunc(upperLeft.x() / dbCellSize) * dbCellSize;
            v_yMin = trunc(lowerRight.y() / dbCellSize) * dbCellSize;
            v_cols = trunc((lowerRight.x() -  upperLeft.x()) / dbCellSize) + 1;
            v_rows = trunc((upperLeft.y() -  lowerRight.y()) / dbCellSize) + 1;
            v_xMax = v_xMin + v_cols * dbCellSize;
            v_yMax = v_yMin + v_rows * dbCellSize;

            halfx = dbCellSize / 2;
            xllCenter = v_xMin + halfx;
            yllCenter = (v_yMin + halfx); //- 0.0200;
            ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);
            nrows = trunc(fabs(v_yMax - v_yMin) / dbCellSize);

            left = ceil((upperLeft.x() + 180) / dbCellSize);
            bottom = ceil((90 - upperLeft.y()) / dbCellSize);

        }
        else
        {
            //We don't have an extent

            //Do we have a set of selected shapes?
            if (!shapes.isEmpty())
            {
                calcBoundFromShapes(shapes,mydb,upperLeft,lowerRight,dbCellSize);

                double v_xMin,v_xMax,v_yMin,v_yMax;
                double halfx;
                int v_cols,v_rows;

                v_xMin = trunc(upperLeft.x() / dbCellSize) * dbCellSize;
                v_yMin = trunc(lowerRight.y() / dbCellSize) * dbCellSize;
                v_cols = trunc((lowerRight.x() -  upperLeft.x()) / dbCellSize) + 1;
                v_rows = trunc((upperLeft.y() -  lowerRight.y()) / dbCellSize) + 1;
                v_xMax = v_xMin + v_cols * dbCellSize;
                v_yMax = v_yMin + v_rows * dbCellSize;

                halfx = dbCellSize / 2;
                xllCenter = v_xMin + halfx;
                yllCenter = (v_yMin + halfx); //- 0.0200;
                ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);
                nrows = trunc(fabs(v_yMax - v_yMin) / dbCellSize);

                left = ceil((upperLeft.x() + 180) / dbCellSize);
                bottom = ceil((90 - upperLeft.y()) / dbCellSize);

            }
            else
            {
                //We dont have an extent nor a set of shapes... Only rasterising a grid or a classification if possible
                if (datasetType == "b")
                {
                    gbtLog(QObject::tr("To rasterize a combination you need an extent or constrainig shapes"));
                    mydb.close();
                    con.closeConnection();
                    return 1;
                }
                calcExtendFromGrid(tableName,mydb,upperLeft,lowerRight,dbCellSize);

                sql = "SELECT xllcenter,yllcenter,ncols,nrows FROM datasetinfo WHERE dataset_id = '" + tableName + "' and dataset_type = 1";

                if (qry.exec(sql))
                {
                    if (qry.first())

                    xllCenter = qry.value(0).toDouble();
                    yllCenter = qry.value(1).toDouble(); //- 0.0200;
                    ncols = qry.value(2).toInt();
                    nrows = qry.value(3).toInt();
                }


                left = ceil((upperLeft.x() + 180) / dbCellSize);
                bottom = ceil((90 - upperLeft.y()) / dbCellSize);
            }

        }

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

        QString sqlFrom;
        QString sqlWhere;
        if (datasetType == "g")
        {
            sql = "SELECT TA.xpos,TA.ypos,TA.cellvalue ";
            sqlFrom = " FROM " + tableName + " TA";            
        }
        else
        {
            if (datasetType == "c")
            {
                sql = "SELECT TA.xpos,TA.ypos,TA.classCode ";
                sqlFrom = " FROM " + tableName + " TA";
            }
            else
            {
                if (datasetType == "b")
                {
                    sql = "SELECT TA.xpos,TA.ypos,TA.comCode ";
                    sqlFrom = " FROM combdataset TA";
                }
                else
                {
                    gbtLog(QObject::tr("Dataset type not valid"));
                    mydb.close();
                    con.closeConnection();
                    return 1;
                }
            }
        }
        if (!whereClause.isEmpty())
        {
            sqlWhere = + " WHERE " + whereClause;
        }
        if (shapeWhereSQL.isEmpty())
            sql = sql + sqlFrom + shapeFromSQL + sqlWhere;
        else
        {
            if (sqlWhere.isEmpty())
                sql = sql + sqlFrom + shapeFromSQL + sqlWhere + " WHERE " + shapeWhereSQL;
            else
                sql = sql + sqlFrom + shapeFromSQL + sqlWhere + " AND " + shapeWhereSQL;
        }
        if (datasetType == "c")
        {
            if (sql.contains("WHERE"))
            {
                if (!classestoShow.isEmpty())
                    sql = sql + " AND (" + getClasses(classestoShow,"classCode") + ")"; //classCode
                else
                    sql = sql + " AND TA.classCode is not null";
            }
            else
            {
                if (!classestoShow.isEmpty())
                    sql = sql + " WHERE (" + getClasses(classestoShow,"classCode") + ")";
                else
                    sql = sql + " WHERE TA.classCode is not null";
            }
        }
        if (datasetType == "b")
        {
            if (sql.contains("WHERE"))
                sql = sql + " AND TA.comCode is not null";
            else
                sql = sql + " WHERE TA.comCode is not null";
            if (!classestoShow.isEmpty())
            {
                sql = sql + " AND (" + getClasses(classestoShow,"comCode") + ")";
            }
        }

        int x;
        int y;

        gbtLog(QObject::tr("Selecting cells"));

        if (qry.exec(sql))
        {
            gbtLog(QObject::tr("Rasterizing"));
            while (qry.next())
            {
                x = qry.value(0).toInt();
                y = qry.value(1).toInt();

                if (((x-left-1) >= 0) && ((x-left-1) <= (ncols-1)))
                {
                    if (((y-bottom-1) >= 0) && ((y-bottom-1) <= (nrows -1)))
                        grid[y-bottom-1][x-left-1] = qry.value(2).toFloat();
                }
            }
            QFile file(outputFile);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return -1;

            QTextStream out(&file);
            out << "ncols         " << ncols << "\n";
            out << "nrows         " << nrows << "\n";
            out << "xllcorner     " << QString::number(xllCenter,'f',11) << "\n";
            out << "yllcorner     " << QString::number(yllCenter,'f',11) << "\n";
            out << "cellsize      " << QString::number(dbCellSize,'f',11) << "\n";
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
            gbtLog(QObject::tr("Cannot read dataset"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
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
        con.closeConnection();

        return 0;
    }

    return 0;

}
