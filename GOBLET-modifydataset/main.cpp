#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QCoreApplication>
#include <QDebug>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Password. Default no password",true,"","string");

    TCLAP::ValueArg<std::string> descriptionArg("s","description","New description for the dataset",false,"","string");
    TCLAP::ValueArg<std::string> unitArg("U","units","New unit for the dataset",false,"","string");
    TCLAP::ValueArg<std::string> metaArg("m","metadata","File containing metadata",false,"None","string");

    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);

    cmd.add(descriptionArg);
    cmd.add(unitArg);
    cmd.add(metaArg);



    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());

    QString description = QString::fromUtf8(descriptionArg.getValue().c_str());
    QString unit = QString::fromUtf8(unitArg.getValue().c_str());
    QString meta = QString::fromUtf8(metaArg.getValue().c_str());


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

        QString sql;
        QSqlQuery qry(mydb);


        sql = "SELECT dataset_desc,dataset_unit FROM datasetinfo WHERE dataset_id = '" + tableName + "'";

        if (qry.exec(sql))
        {
            if (qry.first())
            {
                if (description.isEmpty())
                    description = qry.value(0).toString();
                if (unit.isEmpty())
                    unit = qry.value(1).toString();
            }
            else
            {
                gbtLog(QObject::tr("Dataset does not exists"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }

        }
        else
        {
            gbtLog(QObject::tr("Cannot read dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }

        sql = "UPDATE datasetinfo SET dataset_desc = '" + description + "', ";
        sql = sql + "dataset_unit = '" + unit + "'";
        sql = sql + " WHERE dataset_id = '" + tableName + "'";
        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot update dataset."));
            gbtLog(qry.lastError().databaseText());
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
                    qry.clear();

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

        gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");


        mydb.close();



        return 0;
    }

    return 0;
}
