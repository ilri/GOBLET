#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <QPoint>
#include <QPointF>
#include <math.h>
#include <tclap/CmdLine.h>
#include <QCoreApplication>


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
        where = " TC.geokey BETWEEN '" + idsuit + "' AND ";

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

QStringList dataSets;

bool existDataset(QString table,QSqlDatabase db)
{
    QSqlQuery qry(db);
    QString sql;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + table + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr(" is not a grid dataset"));
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

bool existShape(QString table,QSqlDatabase db)
{
    QSqlQuery qry(db);
    QString sql;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + table + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(table + QObject::tr(" is not a shape dataset"));
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

void getDataSets(QString datasets,QSqlDatabase db)
{
    int pos=0;
    QString table;
    while (pos <= datasets.length()-1)
    {
        if (datasets[pos] != ',')
        {
            table = table + datasets[pos];
            pos++;
        }
        else
        {
            if (existDataset(table,db))
            {
                dataSets.append(table);
            }
            table = "";
            datasets = datasets.mid(pos+1,datasets.length()-pos+1);
            pos = 0;
        }
    }
    if (existDataset(table,db))
    {
        dataSets.append(table);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","datasets","The datasets to aggregate",true,"","string");
    TCLAP::ValueArg<std::string> shapeArg("s","shapedataset","The target shape dataset",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint classification using shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");
    TCLAP::ValueArg<std::string> functionArg("f","combfunction","Combination function avg,sum,max,min,and,or,xor,count. Default avg",false,"","string");

    TCLAP::ValueArg<std::string> whereArg("w","whereCellValue","Condition to aggregate certain cells of the datasets using the cell value. For example '=1'. Separate with coma for two or more datasets. Default empty",false,"","string");


    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(shapeArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(functionArg);
    cmd.add(whereArg);

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
    QString shapeDataSet = QString::fromUtf8(shapeArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    QString function = QString::fromUtf8(functionArg.getValue().c_str());
    QString whereCellValue = QString::fromUtf8(whereArg.getValue().c_str());

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
        if (function.isEmpty())
            function = "avg";

        if (function != "sum" &&
                function != "and" &&
                function != "or" &&
                function != "xor" &&
                function != "avg" &&
                function != "max" &&
                function != "min" &&
                function != "count")
        {
            gbtLog(QObject::tr("Invalid function."));
            return 1;
        }

        QTime procTime;
        procTime.start();

        QString sqlHead;
        QString sql;
        QString sqlFrom;
        QString shpFrom;
        QString shpWhere;
        QSqlQuery qry(mydb);


        if (!existShape(shapeDataSet,mydb))
        {
            mydb.close();
            con.closeConnection();
            return 1;
        }

        getDataSets(tableName,mydb);

        if (dataSets.count() == 0)
        {
            mydb.close();
            con.closeConnection();
            return 1;
        }

        QString WhereClause;
        if (!extent.isEmpty())
        {

            WhereClause = getWhereClauseFromExtent(extent,mydb,dataSets[0]);

        }

        QString sqlcreate;
        QString shapeClause;

        if (!shapes.isEmpty())
        {
            shapeClause = getShapeClause(shapes,mydb);
            if (!shapeClause.isEmpty())
            {
                sqlcreate = "CREATE TEMPORARY TABLE tmpshapes (";
                sqlcreate = sqlcreate + "geokey VARCHAR(14) NOT NULL ,";
                sqlcreate = sqlcreate + "PRIMARY KEY (geokey))";
                sqlcreate = sqlcreate + " ENGINE = MyISAM";
                if (qry.exec(sqlcreate))
                {

                    QString extentClause;
                    extentClause = WhereClause;
                    extentClause.replace("TC.","");

                    if (!extentClause.isEmpty())
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                    else
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                    gbtLog(QObject::tr("Preselecting shapes"));
                    if (qry.exec(sqlcreate))
                    {
                        shpFrom = shpFrom + ",tmpshapes TD";
                        shpWhere = shpWhere + " AND TC.geokey = TD.geokey";
                    }
                    else
                    {
                        gbtLog(QObject::tr("Cannot insert temporary shapes."));
                        gbtLog(qry.lastError().databaseText());
                    }
                }
                else
                {
                    gbtLog(QObject::tr("Cannot create temporary table."));
                    gbtLog(qry.lastError().databaseText());
                }
            }

        }

        //Aggregate the dataset the datasets

        QStringList wherecells;
        wherecells = whereCellValue.split(",",QString::SkipEmptyParts);
        int pos;
        int total;
        //Add empty wherecells
        if (wherecells.count() < dataSets.count())
        {
            total = wherecells.count();
            for (pos = 1; pos <= dataSets.count() - total;pos++)
            {
                wherecells.append("");
            }
        }
        //Remove the extra wherecells at the end
        if (wherecells.count() > dataSets.count())
        {
            total = wherecells.count();
            for (pos = 1; pos <= total - dataSets.count();pos++)
            {
                wherecells.removeLast();
            }
        }


        for (pos = 0; pos <= dataSets.count()-1;pos++)
        {
            sql = "DELETE FROM aggrtable WHERE griddataset = '" + dataSets[pos];
            sql = sql + "' AND shapedataset = '" + shapeDataSet + "' ";
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot delete previous aggregation."));
                break;
            }

            sqlHead = "SELECT '" + dataSets[pos] + "','" + shapeDataSet + "',";
            sqlHead = sqlHead + "TA.shapeid,";
            if (function == "avg")
                sqlHead = sqlHead + "avg(TC.cellvalue)";
            if (function == "sum")
                sqlHead = sqlHead + "sum(TC.cellvalue)";
            if (function == "max")
                sqlHead = sqlHead + "max(TC.cellvalue)";
            if (function == "min")
                sqlHead = sqlHead + "min(TC.cellvalue)";
            if (function == "and")
                sqlHead = sqlHead + "BIT_AND(TC.cellvalue)";
            if (function == "or")
                sqlHead = sqlHead + "BIT_OR(TC.cellvalue)";
            if (function == "xor")
                sqlHead = sqlHead + "BIT_XOR(TC.cellvalue)";
            if (function == "count")
                sqlHead = sqlHead + "count(TB.geokey)";


            sqlFrom = " FROM ";
            sqlFrom = sqlFrom + shapeDataSet + " TA, ";
            sqlFrom = sqlFrom + shapeDataSet + "_raster TB, ";
            sqlFrom = sqlFrom + dataSets[pos] + " TC";

            sql = " WHERE TA.shapeid = TB.shapeid AND TB.geokey = TC.geokey ";

            if (!WhereClause.isEmpty())
                sql = sql + " AND " + WhereClause;


            if (!shpFrom.isEmpty())
            {
                sql = sql + shpWhere;
            }

            if (!wherecells[pos].isEmpty())
            {
                sql = sql + " AND TC.cellvalue " + wherecells[pos];
            }

            sql = "INSERT INTO aggrtable(griddataset,shapedataset,shapeid,shapevalue) " + sqlHead + sqlFrom + shpFrom + sql + " GROUP BY TA.shapeid";

            gbtLog(QObject::tr("Aggregating ") + dataSets[pos] + QObject::tr(" into ") + shapeDataSet);
            if (!qry.exec(sql))
            {
                 gbtLog(QObject::tr("Error aggregating values"));
                 gbtLog(qry.lastError().databaseText());
                 mydb.close();
                 con.closeConnection();
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
        con.closeConnection();

        return 0;
    }

    return 0;

}
