#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QPointF>
#include "writeshapefile.h"
#include <QCoreApplication>
#include <QDebug>

QString getWhereClauseFromExtent(QString extent)
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

    QString res;
    res = "contains(PolygonFromText('POLYGON((";
    res = res + QString::number(dupperLeft.x(),'f',15) + " ";
    res = res + QString::number(dupperLeft.y(),'f',15) + ", ";

    res = res + QString::number(dlowerRight.x(),'f',15) + " ";
    res = res + QString::number(dupperLeft.y(),'f',15) + ", ";

    res = res + QString::number(dlowerRight.x(),'f',15) + " ";
    res = res + QString::number(dlowerRight.y(),'f',15) + ", ";

    res = res + QString::number(dupperLeft.x(),'f',15) + " ";
    res = res + QString::number(dlowerRight.y(),'f',15) + ", ";

    res = res + QString::number(dupperLeft.x(),'f',15) + " ";
    res = res + QString::number(dupperLeft.y(),'f',15);

    res = res + "))'),TB.ogc_geom)";


    return res;
}

QString getClasses(QString classdef, QString field)
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
            classCode = "";
            pos = 0;
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
    TCLAP::ValueArg<std::string> shapeArg("s","shapedataset","Shape dataset name",true,"","string");
    TCLAP::ValueArg<std::string> outputArg("o","outputfile","Output shape file",true,"","string");
    TCLAP::ValueArg<std::string> datasetypeArg("i","datasetype","Dataset type: (s)hapefile, (c)lassification, com(b)ination",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",false,"","string");

    TCLAP::ValueArg<std::string> classesArg("c","classestoshow","'classCode,classCode,....'",false,"","string");
    TCLAP::ValueArg<std::string> combsArg("b","combtoshow","'classCode,classCode,....'",false,"","string");


    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    TCLAP::SwitchArg overwriteSwitch("O","overwrite","Overwrite shape if exists", cmd, false);
    cmd.add(databaseArg);
    cmd.add(shapeArg);
    cmd.add(datasetArg);
    cmd.add(outputArg);
    cmd.add(datasetypeArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(extentArg);
    cmd.add(classesArg);
    cmd.add(combsArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    bool remote = remoteSwitch.getValue();
    bool overWrite = overwriteSwitch.getValue();
    QString path = QString::fromUtf8(pathArg.getValue().c_str());
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString shapeName = QString::fromUtf8(shapeArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString outputFile = QString::fromUtf8(outputArg.getValue().c_str());
    QString datasetType = QString::fromUtf8(datasetypeArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString classestoShow = QString::fromUtf8(classesArg.getValue().c_str());
    QString combstoShow = QString::fromUtf8(combsArg.getValue().c_str());


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

        QString sql;
        QSqlQuery qry(mydb);

        sql = "SELECT count(*) from datasetinfo WHERE dataset_id = '" + shapeName + "' and dataset_type = 2";
        if (qry.exec(sql))
        {
            qry.first();
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr("Shapefile ") + shapeName + QObject::tr(" does not exists"));
                mydb.close();
                con.closeConnection();
                return 1;
            }
        }
        else
        {
            gbtLog(QObject::tr("Cannot locate shape dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        QString WhereClause;
        if (!extent.isEmpty())
            WhereClause = getWhereClauseFromExtent(extent);

        sql = "";
        if (datasetType == "s")
        {
            QString structure;
            structure = "DESC " + shapeName;
            if (!qry.exec(structure))
            {
                gbtLog(QObject::tr("Cannot read dataset."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();
                con.closeConnection();
                return 1;
            }
            sql = "SELECT ";
            while (qry.next())
            {
                if (qry.value(0).toString() != "ogc_geom")
                    sql = sql + qry.value(0).toString() + ",";
                else
                    sql = sql + "AsText(ogc_geom) as shpgeometry,";
            }
            sql = sql.left(sql.length()-1);

            sql = sql + " FROM " + shapeName + " TB ";
            if (!WhereClause.isEmpty())
                sql = sql + " WHERE " + WhereClause;
        }
        if (datasetType == "c")
        {
            if (tableName.isEmpty())
            {
                gbtLog(QObject::tr("To output a classification you need to specify a dataset use -t datasetname"));
                mydb.close();
                con.closeConnection();
                return 1;
            }

            sql = "SELECT TA.shapeid,TA.classCode,AsText(TB.ogc_geom) as shpgeometry";
            sql = sql + " FROM aggrtable TA," + shapeName + " TB";
            sql = sql + " WHERE TA.shapedataset = '" + shapeName + "'";
            sql = sql + " AND TA.griddataset = '" + tableName + "'";
            sql = sql + " AND TA.shapeid = TB.shapeid";
            if (!WhereClause.isEmpty())
                sql = sql + " AND " + WhereClause;
            if (!classestoShow.isEmpty())
            {
                sql = sql + " AND (" + getClasses(classestoShow,"classCode") + ")";
            }
        }
        if (datasetType == "b")
        {
            sql = "SELECT TA.shapeid,TA.comCode,AsText(TB.ogc_geom) as shpgeometry";
            sql = sql + " FROM combaggregate TA," + shapeName + " TB";
            sql = sql + " WHERE TA.shapedataset = '" + shapeName + "'";
            sql = sql + " AND TA.shapeid = TB.shapeid";
            if (!WhereClause.isEmpty())
                sql = sql + " AND " + WhereClause;
            if (!combstoShow.isEmpty())
            {
                sql = sql + " AND (" + getClasses(combstoShow,"comCode") + ")";
            }
        }

        writeShapefile shapefile;
        shapefile.setDatabase(mydb);
        shapefile.setSql(sql);
        shapefile.setShapeFile(outputFile);
        shapefile.setOverWrite(overWrite);
        shapefile.setTableName(shapeName);
        shapefile.setOutputType(datasetType);
        shapefile.createShapeFile();


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
