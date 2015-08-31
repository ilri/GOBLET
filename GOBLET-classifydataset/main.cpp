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


struct classInfo
{
  long classNumber;
  double from;
  double to;
};
typedef classInfo TclassInfo;

QList<classInfo> classes;



void separateClasses(QString classData)
{
    if (classData.length()==0)
        return;

    if (classData.indexOf(" ") < 0)
        return;
    int pos;
    QString temp;
    temp = classData;
    pos = temp.indexOf(":");
    TclassInfo classDef;
    classDef.classNumber = temp.left(pos).toInt();

    //mid(pos+1,classData.length()-pos+1);
    temp = temp.mid(pos+1,temp.length()-pos+1);
    pos = temp.indexOf(" ");
    classDef.from = temp.left(pos).toDouble();
    classDef.to = temp.mid(pos+1,temp.length()-pos+1).toDouble();
    classes.append(classDef);
}

void getClasses(QString classDef)
{
   //ClassNumber:valueFrom ValueTo,ClassNumber:valueFrom ValueTo,...
    int totalClasses = classDef.count(":");
    if (totalClasses == 0)
    {
        gbtLog("Error in class definition");
        return;
    }
    int totcomas;
    totcomas = classDef.count(",");
    if (!(totalClasses-1 == totcomas))
    {
        gbtLog("Error in class definition");
        return;
    }
    QString classData;
    QString temp;
    classData = classDef;
    int pos;
    pos = 0;
    while ((pos <= classData.length()-1))
    {        
        if (classData[pos] != ',')
        {
            temp = temp + classData[pos];
            pos++;
        }
        else
        {
            separateClasses(temp);

            temp = classData.mid(pos+1,classData.length()-pos+1);
            classData = temp;

            temp = "";
            pos = 0;
        }        
    }
    separateClasses(temp);
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    TCLAP::ValueArg<std::string> classArg("c","classdefinition","Class definition: 'ClassNumber:valueFrom ValueTo,ClassNumber:valueFrom ValueTo,...'",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint classification using shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");

    TCLAP::ValueArg<std::string> defValArg("v","defaultvalue","Default value 0",false,"0","string");

    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(classArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(defValArg);

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
    QString classDef = QString::fromUtf8(classArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    QString defvalue = QString::fromUtf8(defValArg.getValue().c_str());

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
        QTime procTime;
        procTime.start();

        //We need to check if dataset is a grid

        getClasses(classDef);
        if (classes.count() ==0 )
        {
            gbtLog(QObject::tr("There are no classes!"));
            mydb.close();
            con.closeConnection();
            return 1;
        }

        QString sql;
        QString headsql;

        headsql = "UPDATE " + tableName + " TA";
        QString WhereClause;
        sql = " SET TA.classCode = CASE";
        for (int t=0; t <= classes.count()-1;t++)
        {
            sql = sql + " WHEN (TA.cellvalue >= " + QString::number(classes[t].from,'f',5);
            sql = sql + " AND TA.cellvalue <= " + QString::number(classes[t].to,'f',5);
            sql = sql + ") THEN " + QString::number(classes[t].classNumber);            
        }
        sql = sql + " ELSE " + defvalue + " END";

        if (!extent.isEmpty())
        {

            WhereClause = getWhereClauseFromExtent(extent,mydb,tableName);
            if (!WhereClause.isEmpty())
                sql = sql + " WHERE " + WhereClause;            
        }

        QSqlQuery qry(mydb);

        if (!shapes.isEmpty())
        {
            QString shapeClause;

            shapeClause = getShapeClause(shapes,mydb);


            if (!shapeClause.isEmpty())
            {
                //
                QString sqlcreate;
                sqlcreate = "CREATE TEMPORARY TABLE tmpshapes (";
                sqlcreate = sqlcreate + "geokey VARCHAR(14) NOT NULL ,";
                sqlcreate = sqlcreate + "PRIMARY KEY (geokey))";
                sqlcreate = sqlcreate + " ENGINE = MyISAM";
                if (qry.exec(sqlcreate))
                {
                    QString extentClause;
                    extentClause = WhereClause;
                    extentClause.replace("TA.","");

                    if (!extentClause.isEmpty())
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                    else
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                    gbtLog(QObject::tr("Preselecting shapes"));
                    if (qry.exec(sqlcreate))
                    {
                        headsql = headsql + ", tmpshapes TB";

                        if (sql.contains("WHERE"))
                            sql = sql + " AND TA.geokey = TB.geokey";
                        else
                            sql = sql + " WHERE TA.geokey = TB.geokey";

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

        gbtLog(QObject::tr("Classifying...Please wait"));
        sql = headsql + sql;

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot classify dataset."));
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
