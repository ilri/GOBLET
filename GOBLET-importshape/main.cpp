#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include "insertshape.h"
#include "shapeToGrid.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    TCLAP::ValueArg<std::string> shapeArg("S","shapefile","Shape file",true,"","string");
    TCLAP::ValueArg<std::string> datasetDescArg("s","datasetDescription","Dataset description",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> unitArg("U","units","New unit for the dataset",false,"Not set","string");
    TCLAP::ValueArg<std::string> metaArg("m","metadata","File containing metadata",false,"None","string");

    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    TCLAP::SwitchArg overwriteSwitch("o","overwrite","Overwrite the previous dataset if present", cmd, false);

    TCLAP::SwitchArg rasterSwitch("n","notrasterize","Not rasterize the shape (false by default)", cmd, false);

    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(shapeArg);
    cmd.add(datasetDescArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(unitArg);
    cmd.add(metaArg);

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
    QString shapeFile = QString::fromUtf8(shapeArg.getValue().c_str());
    QString tableDesc = QString::fromUtf8(datasetDescArg.getValue().c_str());

    QString unit = QString::fromUtf8(unitArg.getValue().c_str());
    QString meta = QString::fromUtf8(metaArg.getValue().c_str());

    bool replace = overwriteSwitch.getValue();
    bool rasterize = !rasterSwitch.getValue();

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
                con.closeConnection();
                return 1;
            }

            //We need to drop the _raster of the table

            sql = "DROP TABLE IF EXISTS " + tableName + "_raster";
            qry.exec(sql);

        }

        // This process insert the shapeFile into mysql using geometries
        // This process if based on Jeff Lounsbury's shape to mysql program
        insertShape insShape;
        insShape.setTableName(tableName);
        insShape.setShapeFile(shapeFile);
        insShape.loadShape();

        QStringList sqls;
        sqls.append(insShape.getSQLs());
        for (int pos = 0; pos <= sqls.count()-1;pos++)
        {
            sql = sqls[pos].replace("\n","");
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot remove previous dataset."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();
                con.closeConnection();
                return 1;
            }
        }

        sql = "ALTER TABLE " + tableName + " ADD COLUMN num_cells INT(11) default 0";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot add numcells to dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        sql = "CREATE TABLE " + tableName + "_raster (";
        sql = sql + "geokey VARCHAR(14) NOT NULL ,";
        sql = sql + "xpos DECIMAL(7) NULL ,";
        sql = sql + "ypos DECIMAL(7) NULL ,";
        sql = sql + "shapeid INT(11) ,";
        sql = sql + "PRIMARY KEY (geokey)) ENGINE = MyISAM";

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot create dataset. It might already exists"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        //Get the cellSize of the database
        double dbCellSize;
        dbCellSize = 0.0;

        sql = "SELECT cellSize FROM gbtconfig";
        if (qry.exec(sql))
        {
            if (qry.first())
                dbCellSize = qry.value(0).toDouble();            
        }


        //Rasterize the shapefile

        sql = "ALTER TABLE " + tableName + "_raster DISABLE KEYS";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot disable keys"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }
        Rasterizer rasterizeShape;
        if (rasterize)
        {
            gbtLog(QObject::tr("Rasterizing shape"));

            //Then we need to rasterize the shapes.

            rasterizeShape.loadShapeFile(shapeFile,dbCellSize,-9999,tableName);
            rasterizeShape.setTableName(tableName + "_raster");

            uploadCSV uploadData;
            uploadData.setTableName(tableName + "_raster");
            uploadData.setPath(path);
            uploadData.setDataBase(dbName);
            uploadData.setRemote(remote);
            uploadData.setHost(host);
            uploadData.setPort(port.toInt());
            uploadData.setUser(userName);
            uploadData.setPassword(password);
            rasterizeShape.start();
            uploadData.start();
            rasterizeShape.wait();
            uploadData.wait();
        }

        sql = "ALTER TABLE " + tableName + "_raster ENABLE KEYS";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot enable keys"));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        //Update the number of cells
        if (rasterize)
        {
            sql = "UPDATE " + tableName + " TA SET num_cells = (SELECT count(geokey) FROM ";
            sql = sql + tableName + "_raster TB WHERE TA.shapeid = TB.shapeid);";

            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot update number of cells"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();
                con.closeConnection();
                return 1;
            }
        }
        //Insert the dataset into the dataset information table

        if (rasterize)
        {
            sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_unit,dataset_type,ncols,nrows,xllcenter,yllcenter,shapefiletype)";
            sql = sql + " VALUES ('" + tableName +"',";
            sql = sql + "'" + tableDesc +"',";
            sql = sql + "'" + unit +"',2,";
            sql = sql + QString::number(rasterizeShape.getNCols()) + ",";
            sql = sql + QString::number(rasterizeShape.getNRows()) + ",";
            sql = sql + QString::number(rasterizeShape.getXllCenter(),'f',11) + ",";
            sql = sql + QString::number(rasterizeShape.getYllCenter(),'f',11) + ",";
            sql = sql + QString::number(rasterizeShape.getShapeType()) + ")";
        }
        else
        {
            sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_unit,dataset_type,shapefiletype)";
            sql = sql + " VALUES ('" + tableName +"',";
            sql = sql + "'" + tableDesc +"',";
            sql = sql + "'" + unit +"',2,";
            sql = sql + QString::number(insShape.getShapeType()) + ")";
        }

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot insert dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
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

        int Hours;
        int Minutes;
        int Seconds;
        int Milliseconds;

        Milliseconds = procTime.elapsed();

        Hours = Milliseconds / (1000*60*60);
        Minutes = (Milliseconds % (1000*60*60)) / (1000*60);
        Seconds = ((Milliseconds % (1000*60*60)) % (1000*60)) / 1000;

        printf("\n");

        gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();

        con.closeConnection();

        return 0;
    }

    return 0;

}
