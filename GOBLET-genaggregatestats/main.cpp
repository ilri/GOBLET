#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    TCLAP::ValueArg<std::string> shapeArg("s","shapedataset","The aggregated shape dataset",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
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

        sql = "SELECT count(shapeid),max(shapevalue),min(shapevalue),avg(shapevalue),std(shapevalue) FROM aggrtable";
        sql = sql + " WHERE griddataset = '" + tableName + "' AND shapedataset = '" + shapeDataSet + "'";

        if (qry.exec(sql))
        {
            qry.first();
            printf("Number of shapes: %s\n", qry.value(0).toString().toLatin1().data());
            printf("Maximum value: %s\n", qry.value(1).toString().toLatin1().data());
            printf("Minimum value: %s\n", qry.value(2).toString().toLatin1().data());
            printf("Average value: %s\n", qry.value(3).toString().toLatin1().data());
            printf("Standard deviation: %s\n", qry.value(4).toString().toLatin1().data());
        }
        else
        {
            gbtLog(QObject::tr("Cannot read dataset."));
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
