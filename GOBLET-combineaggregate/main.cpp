#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QPointF>
#include <QCoreApplication>
#include <QVariant>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

QString getWhereClauseFromExtent(QString extent)
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> shapeArg("s","shapedataset","The target shape dataset",true,"","string");
    //Non required arguments

    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> functionArg("f","combfunction","Combination function sum,and,or,xor. Default sum",false,"","string");

    cmd.add(databaseArg);
    cmd.add(shapeArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);

    cmd.add(extentArg);
    cmd.add(functionArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command

    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString tableName = QString::fromUtf8(shapeArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString function = QString::fromUtf8(functionArg.getValue().c_str());


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
        if (function.isEmpty())
            function = "sum";

        if (function != "sum" && function != "and" && function != "or" && function != "xor" )
        {
            gbtLog(QObject::tr("Invalid function."));
            return 1;
        }

        QTime procTime;
        procTime.start();

        QString sql;
        QSqlQuery qry(mydb);


        sql = "SELECT count(*) from datasetinfo WHERE dataset_id = '" + tableName + "' and dataset_type = 2";
        if (qry.exec(sql))
        {
            qry.first();
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr("Shapefile ") + tableName + QObject::tr(" does not exists"));
                mydb.close();

                return 1;
            }
        }
        else
        {
            gbtLog(QObject::tr("Cannot classify dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }

        sql = "DELETE FROM combaggregate WHERE shapedataset = '" + tableName + "'";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot reset dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }
        QStringList grids;
        sql = "SELECT DISTINCT griddataset FROM aggrtable WHERE shapedataset = '" + tableName + "'";
        if (qry.exec(sql))
        {
            while (qry.next())
            {
                grids.append(qry.value(0).toString());
            }
        }
        //qDebug() << grids;

        sql = "INSERT INTO combaggregate (shapedataset,shapeid,comCode) SELECT TA.shapedataset,TA.shapeid,";
        if (function == "sum")
            sql = sql + "sum(TA.classCode)";
        if (function == "and")
            sql = sql + "BIT_AND(TA.classCode)";
        if (function == "or")
            sql = sql + "BIT_OR(TA.classCode)";
        if (function == "xor")
            sql = sql + "BIT_XOR(TA.classCode)";
        sql = sql + " FROM aggrtable TA, " + tableName + " TB";
        sql = sql + " WHERE TA.shapeid = TB.shapeid AND TA.shapedataset = '" + tableName + "'";
        for (int grd = 0; grd < grids.count(); grd++)
        {
            sql = sql + " AND TA.shapeid IN (SELECT DISTINCT shapeid FROM aggrtable WHERE classCode IS NOT NULL AND griddataset = '" + grids[grd] + "' AND shapedataset = '" + tableName + "')";
        }


        QString WhereClause;
        if (!extent.isEmpty())
        {
            WhereClause = getWhereClauseFromExtent(extent);
            if (!WhereClause.isEmpty())
                sql = sql + " AND " + WhereClause;
        }

        sql = sql + " GROUP BY TA.shapedataset,TA.shapeid";

        //qDebug() << sql;

        gbtLog(QObject::tr("Combining aggregate."));

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot reset dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

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


        return 0;
    }

    return 0;

}
