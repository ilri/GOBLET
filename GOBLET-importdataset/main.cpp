#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include "gridtocsv.h"
#include <tclap/CmdLine.h>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

QString stripCellSize(QString value)
{
    QString temp;
    int pos;
    pos = value.indexOf(" ");
    temp = value.right(value.length()-pos);
    temp = temp.replace(" ","");
    return temp;
}

bool cellSizeMatch(QString gridfile,QSqlDatabase mydb)
{
    QFile file(gridfile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream in(&file);
    QString line;
    QString cellSize;

    while (!in.atEnd())
    {
        line = in.readLine();

        if (line.contains("cellsize",Qt::CaseInsensitive))
        {
            cellSize = stripCellSize(line);
            break;
        }
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


    QSqlQuery qry(mydb);
    QString sql;

    QString dbCellSize;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            dbCellSize = QString::number(qry.value(0).toDouble(),'f',11);
        }
        else
            return false;
    }
    else
        return false;




    point = dbCellSize.indexOf(".");
    degrees = dbCellSize.left(point);
    decimals = dbCellSize.right(dbCellSize.length()-point-1);

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
    dbCellSize = degrees + "." + decimals;


    if (dbCellSize == cellSize)
        return true;
    else
    {
        gbtLog(QObject::tr("The cellsize of the grid does not match with the database"));
        gbtLog(QObject::tr("The cellsize of grid is: ") + cellSize);
        gbtLog(QObject::tr("The cellsize of database is: ") + dbCellSize);
        return false;
    }

    //return true;
}

int main(int argc, char *argv[])
{

    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    TCLAP::ValueArg<std::string> datasetDescArg("s","datasetDescription","Dataset description",true,"","string");
    TCLAP::ValueArg<std::string> gridFileArg("g","gridfile","Source grid file",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> unitArg("U","units","New unit for the dataset",false,"Not set","string");
    TCLAP::ValueArg<std::string> metaArg("m","metadata","File containing metadata",false,"None","string");
    TCLAP::ValueArg<std::string> noneArg("n","nonedata","None data value. Default -999",false,"-999","string");

    //Switches
    TCLAP::SwitchArg overwriteSwitch("o","overwrite","Overwrite dataset if exists", cmd, false);

    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(datasetDescArg);
    cmd.add(gridFileArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(unitArg);
    cmd.add(metaArg);
    cmd.add(noneArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString gridFile = QString::fromUtf8(gridFileArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString tableDesc = QString::fromUtf8(datasetDescArg.getValue().c_str());
    QString unit = QString::fromUtf8(unitArg.getValue().c_str());
    QString meta = QString::fromUtf8(metaArg.getValue().c_str());
    QString noneValue = QString::fromUtf8(noneArg.getValue().c_str());

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
        if (!cellSizeMatch(gridFile,mydb))
        {
            mydb.close();

            return 1;
        }

        QString sql;
        QSqlQuery qry(mydb);

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


        if (!QFile::exists(gridFile))
        {
            gbtLog(QObject::tr("Grid file not found"));
            mydb.close();

            return 1;
        }

        QTime procTime;
        procTime.start();

        gridToCSV grdtoCSV;
        uploadCSV uploadData;
        uploadData.setTableName(tableName);
        uploadData.setDataBase(dbName);
        uploadData.setHost(host);
        uploadData.setPort(port.toInt());
        uploadData.setUser(userName);
        uploadData.setPassword(password);
        uploadData.setNonDataValue(noneValue.toDouble());

        grdtoCSV.setGridFile(gridFile);
        grdtoCSV.setNonDataValue(noneValue.toDouble());
        grdtoCSV.start();
        uploadData.start();
        grdtoCSV.wait();
        uploadData.wait();

        sql = "ALTER TABLE " + tableName + " ENABLE KEYS";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot enable keys"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }

        sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_unit,dataset_type,ncols,nrows,xllcenter,yllcenter)";
        sql = sql + " VALUES ('" + tableName +"',";
        sql = sql + "'" + tableDesc +"',";
        sql = sql + "'" + unit +"',1,";
        sql = sql + QString::number(grdtoCSV.getNCols()) + ",";
        sql = sql + QString::number(grdtoCSV.getNRows()) + ",";
        sql = sql + QString::number(grdtoCSV.getXllCenter(),'f',15) + ",";
        sql = sql + QString::number(grdtoCSV.getYllCenter(),'f',15) + ")";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot insert dataset."));
            gbtLog(qry.lastError().databaseText());
            gbtLog(sql);
            mydb.close();

            return 1;
        }

        if (meta != "None")
        {
            if (QFile::exists(meta))
            {
                QFile metaFile(meta);
                if(metaFile.open(QIODevice::ReadOnly))
                {
                    QByteArray ba;
                    ba = metaFile.readAll();
                    metaFile.close();

                    qry.prepare("UPDATE datasetinfo SET dataset_metadata = :metafile WHERE dataset_id = '" + tableName + "'");
                    qry.bindValue(":metafile", ba);
                    if (!qry.exec())
                    {
                        gbtLog(QObject::tr("Cannot insert the metadata."));
                        gbtLog(qry.lastError().databaseText());
                    }
                }
                else
                {
                    gbtLog(QObject::tr("Cannot open the metadata file."));
                    gbtLog(qry.lastError().databaseText());
                }
            }
            else
            {
                gbtLog(QObject::tr("The metadata file does not exits."));
                gbtLog(qry.lastError().databaseText());
            }
        }

        sql = "SELECT count(geokey) FROM " + tableName;
        if (qry.exec(sql))
        {
            qry.first();
            gbtLog(QObject::tr("\nUpload finished. %n cells imported.","",qry.value(0).toInt()));
        }
        else
        {
            gbtLog(QObject::tr("Cannot open databaset"));
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
