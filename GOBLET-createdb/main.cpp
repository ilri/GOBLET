#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTextStream>
#include <tclap/CmdLine.h>
#include <QCoreApplication>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

int main(int argc, char *argv[])
{    
    //QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"GOBLET","string");
    TCLAP::ValueArg<std::string> cellSizeArg("c","cellsize","Cell size",true,"0.083333","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    //Switches

    cmd.add(databaseArg);
    cmd.add(cellSizeArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString cellSize = QString::fromUtf8(cellSizeArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());


    QSqlDatabase mydb;

    mydb = QSqlDatabase::addDatabase("QMYSQL","connection1");
    mydb.setHostName(host);
    mydb.setPort(port.toInt());
    mydb.setConnectOptions("MYSQL_OPT_LOCAL_INFILE=1");
    if (!userName.isEmpty())
        mydb.setUserName(userName);
    if (!password.isEmpty())
        mydb.setPassword(password);



    mydb.setDatabaseName("mysql");

    if (!mydb.open())
    {
        gbtLog(QObject::tr("Cannot connect to MySQL"));
        return 1;
    }
    else
    {
        QSqlQuery qry(mydb);
        QString sql;

        sql = "CREATE schema " + dbName;
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot create database"));
            mydb.close();
            return 1;
        }


        QString degrees;
        QString decimals;
        int point;
        point = cellSize.indexOf(".");
        degrees = cellSize.left(point);
        decimals = cellSize.right(cellSize.length()-point-1);

        if (decimals.length() > 11)
            decimals = decimals.left(11);
        else
        {
            if (decimals.length() < 11)
            {
                QString lastDigit;
                lastDigit = "0"; //cellSize[cellSize.length()-1]
                int currLen;
                currLen = decimals.length();
                for (int pos = 1; pos <= 11-currLen;pos++)
                    decimals = decimals + lastDigit;
            }
        }
        cellSize = degrees + "." + decimals;


        sql = "CREATE TABLE " + dbName + ".gbtconfig(";
        sql = sql + "conf_code CHAR(3) NOT NULL,";
        sql = sql + "cellSize DECIMAL(13,11) NULL,";
        sql = sql + "PRIMARY KEY(conf_code))";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot create database"));
            mydb.close();
            return 1;
        }
        sql = "INSERT INTO " + dbName + ".gbtconfig (conf_code,cellSize) VALUES ('001',";
        sql = sql + cellSize + ")";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot create database"));
            mydb.close();
            return 1;
        }
        sql = "CREATE TABLE " + dbName + ".datasetinfo (";
        sql = sql + "dataset_id VARCHAR(60) NOT NULL ,";
        sql = sql + "dataset_desc VARCHAR(120) NULL ,";
        sql = sql + "dataset_unit VARCHAR(60) NULL ,";
        sql = sql + "dataset_metadata MEDIUMBLOB NULL ,";
        sql = sql + "dataset_type INT(11)  NULL DEFAULT 0 ,";
        sql = sql + "ncols INT(11)  NULL ,";
        sql = sql + "nrows INT(11)  NULL ,";
        sql = sql + "xllcenter DECIMAL(18,15) NULL ,";
        sql = sql + "yllcenter DECIMAL(18,15) NULL ,";
        sql = sql + "shapefiletype INT(2) NULL DEFAULT -1 ,";
        sql = sql + "PRIMARY KEY (dataset_id) )";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot datasets table"));
            mydb.close();
            return 1;
        }

        sql = "CREATE TABLE " + dbName + ".aggrtable (";
        sql = sql + "griddataset VARCHAR(60) NOT NULL ,";
        sql = sql + "shapedataset VARCHAR(60) NOT NULL ,";
        sql = sql + "shapeid INT(11)  NOT NULL ,";
        sql = sql + "shapevalue DECIMAL(14,5) NULL DEFAULT 0 ,";
        sql = sql + "classCode BIGINT  NULL DEFAULT NULL ,";
        //sql = sql + "classColour VARCHAR(9) NULL DEFAULT '' ,";
        sql = sql + "PRIMARY KEY (griddataset, shapedataset, shapeid))";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot aggregation table"));
            mydb.close();
            return 1;
        }

        sql = "CREATE TABLE " + dbName + ".combdataset (";
        sql = sql + "geokey VARCHAR(14) NOT NULL ,";
        sql = sql + "xpos DECIMAL(7) NULL ,";
        sql = sql + "ypos DECIMAL(7) NULL ,";
        sql = sql + "comCode DECIMAL(10) NULL ,";
        sql = sql + "PRIMARY KEY (geokey) )";
        sql = sql + "ENGINE = MyISAM";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot combination table"));
            mydb.close();
            return 1;
        }

        sql = "CREATE TABLE IF NOT EXISTS " + dbName + ".combaggregate (";
        sql = sql + "shapedataset VARCHAR(60) NOT NULL ,";
        sql = sql + "shapeid INT(11)  NOT NULL ,";
        sql = sql + "comCode DECIMAL(10) NULL ,";
        sql = sql + "PRIMARY KEY (shapedataset, shapeid) )";
        sql = sql + "ENGINE = MyISAM";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot combination table"));
            mydb.close();
            return 1;
        }

        //Testing the upload of external files
        sql = "CREATE TABLE " + dbName + ".testupload(";
        sql = sql + "code VARCHAR(3) NOT NULL,";
        sql = sql + "description VARCHAR(120) NULL,";
        sql = sql + "PRIMARY KEY (code))";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot create upload testing table"));
            mydb.close();
            return 1;
        }

        QFile file("test.csv");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << "001,Test" << "\n";
        }
        file.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther );
        file.close();
        QDir dir2;
        QString testFile;
        testFile = dir2.absolutePath() + "/test.csv";

#ifdef Q_OS_WIN
        testFile = testFile.replace("\\","/");
#endif

        //gbtLog(testFile);
        sql = "LOAD DATA LOCAL INFILE '";
        sql = sql + testFile + "' INTO TABLE " + dbName + ".testupload";
        sql = sql + " fields terminated by ',' lines terminated by '\n' (code,description)";
        //gbtLog(sql);
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot upload test data: ") + qry.lastError().text());
            mydb.close();
            return 1;
        }

        sql = "SELECT count(*) from " + dbName + ".testupload";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot select test data: ") + qry.lastError().databaseText());
            mydb.close();
            return 1;
        }
        else
        {
            qry.first();
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr("Cannot upload test data: Upload returned 0 records"));
                mydb.close();
                return 1;
            }
        }

        file.remove("test.csv");

        sql = "DROP TABLE " + dbName + ".testupload";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot remove upload testing table"));
            mydb.close();
            return 1;
        }
        //gbtLog(QObject::tr("Closing DB"));
        mydb.close();
        //gbtLog(QObject::tr("Closing connection"));
        gbtLog(QObject::tr("Done"));
    }
    return 0;
}
