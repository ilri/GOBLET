#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QCoreApplication>
#include <QVariant>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

bool createShape(QString outputType, QStringList datasets, QSqlDatabase db, QString shape, QString targetShape, QString description, QString function)
{
    //bool res;
    //res = true;
    QSqlQuery query(db);
    QSqlQuery query2(db);
    QString sql;

    gbtLog("Removing previous target shapefile if exits");

    sql = "DELETE FROM datasetinfo WHERE dataset_id = '" + targetShape + "'";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }
    sql = "DROP TABLE IF EXISTS " + targetShape;
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }
    sql = "DROP TABLE IF EXISTS " + targetShape + "_raster";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    gbtLog("Creating new shapefile");

    sql = "CREATE TABLE " + targetShape + " (shapeid INT(11), ogc_geom geometry, num_cells INT (11),";
    if ((outputType != "b") && (outputType != "g"))
    {
        for (int pos = 0; pos <= datasets.count()-1;pos++)
        {
            if (outputType == "v")
                sql = sql + datasets[pos] + " double,";
            else
                sql = sql + datasets[pos] + " INT(11),";
        }
        sql = sql + " PRIMARY KEY (shapeid))";
    }
    else
    {
        if (outputType == "b")
            sql = sql + "comCode INT (11), PRIMARY KEY (shapeid))";
        else
           sql = sql + "grpVal DOUBLE, PRIMARY KEY (shapeid))";
    }
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    sql = "CREATE TABLE " + targetShape + "_raster (geokey varchar(14), xpos decimal(7,0), ypos decimal(7,0), shapeid int(11), PRIMARY KEY (geokey))";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    gbtLog("Copying data from " +  shape + " to " + targetShape);

    sql = "SELECT dataset_unit,dataset_type,ncols,nrows,xllcenter,yllcenter,shapefiletype FROM datasetinfo";
    sql = sql + " WHERE dataset_id = '" + shape + "'";
    if (query.exec(sql))
    {
        if (query.first())
        {
            sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_unit,dataset_type,ncols,nrows,xllcenter,yllcenter,shapefiletype)  VALUES (";
            sql = sql + "'" + targetShape + "',";
            sql = sql + "'" + description + "',";
            sql = sql + "'" + query.value(0).toString() + "',";
            sql = sql + QString::number(query.value(1).toInt()) + ",";
            sql = sql + QString::number(query.value(2).toInt()) + ",";
            sql = sql + QString::number(query.value(3).toInt()) + ",";

            sql = sql + QString::number(query.value(4).toFloat(),'f',12) + ",";
            sql = sql + QString::number(query.value(5).toFloat(),'f',12) + ",";
            sql = sql + QString::number(query.value(6).toInt()) + ")";

            if (!query.exec(sql))
            {
                gbtLog(query.lastError().databaseText());
                return false;
            }
        }
        else
            return false;
    }
    else
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    sql = "INSERT INTO " + targetShape + " (shapeid,ogc_geom,num_cells)";
    sql = sql + " SELECT shapeid,ogc_geom,num_cells FROM " + shape;
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }


    //This moves the data from the aggregate/combine to the shapefile table

    if ((outputType == "b") || (outputType == "g"))
    {
        if (outputType == "b")
            sql = "SELECT shapeid,comCode FROM combaggregate WHERE shapedataset = '" + shape + "'";
        else
            sql = "SELECT shapeid," + function + "(shapevalue) as vtotal FROM aggrtable WHERE shapedataset = '" + shape + "' GROUP BY shapeid";
    }
    else
        sql = "SELECT griddataset,shapeid,shapevalue,classCode FROM aggrtable WHERE shapedataset = '" + shape + "'";

    if (query.exec(sql))
    {
        while (query.next())
        {
            if ((outputType == "b") || (outputType == "g"))
            {
                if (outputType == "b")
                {
                    sql = "UPDATE " + targetShape + " SET comCode = ";
                    sql = sql + query.value(1).toString() + " WHERE shapeid = ";
                    sql = sql + query.value(0).toString();
                    if (!query2.exec(sql))
                        gbtLog(query2.lastError().databaseText());
                }
                else
                {
                    sql = "UPDATE " + targetShape + " SET grpVal = ";
                    sql = sql + query.value(1).toString() + " WHERE shapeid = ";
                    sql = sql + query.value(0).toString();
                    if (!query2.exec(sql))
                        gbtLog(query2.lastError().databaseText());
                }
            }
            else
            {
                if (datasets.contains(query.value(0).toString(),Qt::CaseInsensitive))
                {
                    sql = "UPDATE " + targetShape + " SET " + query.value(0).toString() + " = ";
                    if (outputType == "v")
                        sql = sql + QString::number(query.value(2).toFloat(),'f',3);
                    else
                        sql = sql + query.value(3).toString();
                    sql = sql + " WHERE shapeid = ";
                    sql = sql + query.value(1).toString();
                    if (!query2.exec(sql))
                        gbtLog(query2.lastError().databaseText());
                }
            }
        }
    }
    else
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    gbtLog("Copying raster data from " +  shape + " to " + targetShape);

    sql = "ALTER TABLE " + targetShape + " DISABLE KEYS";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    sql = "INSERT INTO " + targetShape + "_raster (geokey,xpos,ypos,shapeid)";
    sql = sql + " SELECT geokey,xpos,ypos,shapeid FROM " + shape +  "_raster";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    sql = "ALTER TABLE " + targetShape + " ENABLE KEYS";
    if (!query.exec(sql))
    {
        gbtLog(query.lastError().databaseText());
        return false;
    }

    return true;
}

QStringList getDatasets(QString shape,QSqlDatabase db)
{
    QSqlQuery query(db);
    QString sql;
    QStringList res;
    sql = "SELECT DISTINCT griddataset FROM aggrtable WHERE shapedataset = '" + shape + "'";
    if (query.exec(sql))
    {
        while (query.next())
        {
            res.append(query.value(0).toString());
        }
    }
    return res;
}

bool splitDataSets(QString dataset, QStringList &datasets, QSqlDatabase db, QString shape)
{
    bool res;
    res = true;
    datasets = dataset.split(",",QString::SkipEmptyParts);
    QSqlQuery query(db);
    QString sql;
    for (int pos = 0; pos <= datasets.count()-1;pos++)
    {
        sql = "SELECT count(*) FROM aggrtable WHERE shapedataset = '" + shape + "' AND ";
        sql = sql + "griddataset = '" + datasets[pos] + "'";
        if (query.exec(sql))
        {
            query.first();
            if (query.value(0).toInt() == 0)
                res = false;
        }
        else
        {
            res = false;
        }
    }
    return res;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> shapeArg("i","inputshapefile","The input shapefile used in the aggregation",true,"","string");
    TCLAP::ValueArg<std::string> targetshapeArg("e","targetshapefile","The name of the target shapefile",true,"","string");

    //Non required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","datasets","The aggregated datasets separated by coma. Default: All aggregated datasets",false,"all","string");
    TCLAP::ValueArg<std::string> outputArg("O","outputtype","Output type: aggregated (v)value (default), (c)lass code, com(b)ination code, (g)grouped value per shape",false,"v","string");
    TCLAP::ValueArg<std::string> descArg("s","description","Description of target shape",false,"Result of aggregate","string");
    TCLAP::ValueArg<std::string> functionrg("f","function","Function used if outputs is grouped",false,"AVG","string");


    //Switches
    TCLAP::SwitchArg overwriteSwitch("o","overwrite","Overwrite the currect shape", cmd, false);


    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(outputArg);
    cmd.add(shapeArg);
    cmd.add(targetshapeArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(descArg);
    cmd.add(functionrg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString dataset = QString::fromUtf8(datasetArg.getValue().c_str());
    QString shape = QString::fromUtf8(shapeArg.getValue().c_str());
    bool overWrite = overwriteSwitch.getValue();
    QString outputType = QString::fromUtf8(outputArg.getValue().c_str());
    QString targetShape = QString::fromUtf8(targetshapeArg.getValue().c_str());
    QString description = QString::fromUtf8(descArg.getValue().c_str());
    QString function = QString::fromUtf8(functionrg.getValue().c_str());


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

        sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + targetShape + "'";
        if (qry.exec(sql))
        {
            qry.first();
            if (qry.value(0).toInt() > 0 && !overWrite)
            {
                gbtLog(QObject::tr("The target datasets already exists."));
                mydb.close();

                return 1;
            }
        }
        else
        {
            gbtLog(QObject::tr("Cannot convert aggregate."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();

            return 1;
        }

        if (outputType != "b")
        {
            sql = "SELECT count(*) FROM aggrtable WHERE shapedataset = '";
            sql = sql + shape + "'";
            if (qry.exec(sql))
            {
                qry.first();
                if (qry.value(0).toInt() > 0)
                {
                    QStringList datasets;
                    if (dataset != "all")
                    {
                        if (!splitDataSets(dataset,datasets,mydb,shape))
                        {
                            gbtLog(QObject::tr("One of the datasets does not exists."));
                            mydb.close();

                            return 1;
                        }
                    }
                    else
                    {
                        datasets = getDatasets(shape,mydb);
                    }
                    if (datasets.count() > 0)
                    {
                        if (!createShape(outputType,datasets,mydb,shape,targetShape,description,function))
                        {
                            gbtLog(QObject::tr("Unable to create shapefile from aggregate"));
                            mydb.close();

                            return 1;
                        }
                    }
                    else
                    {
                        gbtLog(QObject::tr("There are no aggregated datasets to convert"));
                        mydb.close();

                        return 1;
                    }
                }
                else
                {
                    gbtLog(QObject::tr("The aggregate does not exits."));
                    mydb.close();

                    return 1;
                }
            }
            else
            {
                gbtLog(QObject::tr("Cannot convert aggregate."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
        }
        else
        {
            sql = "SELECT count(*) FROM combaggregate WHERE shapedataset = '";
            sql = sql + shape + "'";
            if (qry.exec(sql))
            {
                qry.first();
                if (qry.value(0).toInt() > 0)
                {
                    //Proceeds to convert the combination
                }
                else
                {
                    gbtLog(QObject::tr("The aggregate does not exits."));
                    mydb.close();

                    return 1;
                }
            }
            else
            {
                gbtLog(QObject::tr("Cannot convert aggregate."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

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


        return 0;
    }

    return 0;

}
