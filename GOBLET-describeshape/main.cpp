#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <QtXml>
#include <tclap/CmdLine.h>
#include <QCoreApplication>


int getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

QString getShapeType(int type)
{
    switch (type)
    {
    case 1:
        return "POINT";
    case 3:
        return "ARC";
    case 5:
        return "POLYGON";
    case 8:
        return "MULTIPOINT";
    case 11:
        return "POINTZ";
    case 13:
        return "ARCZ";
    case 15:
        return "POLYGONZ";
    case 18:
        return "MULTIPOINTZ";
    case 21:
        return "POINTM";
    case 23:
        return "ARCM";
    case 25:
        return "POLYGONM";
    case 28:
        return "MULTIPOINTM";
    case 31:
        return "MULTIPATCH";
    default:
        return "NULL";
    }
}

struct fielddef
{
  QString fieldName;
  QString fieldType;
};
typedef fielddef TfieldDef;

QString fixString(QString source,int len)
{
    QString res;
    res = source;
    int pos;
    for (pos = 1; pos <= len-source.length();pos++)
    {
        res = res + " ";
    }
    return res;
}

QString fixLine(QString source,int len)
{
    QString res;
    res = source;
    int pos;
    for (pos = 1; pos <= len-source.length();pos++)
    {
        res = res + "-";
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
    TCLAP::ValueArg<std::string> datasetArg("t","dataset","Dataset name",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> formatArg("f","format","Format: (h)uman or (c)omputer",false,"","string");
    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(formatArg);

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
    QString format = QString::fromUtf8(formatArg.getValue().c_str());

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
        if (format.isEmpty())
            format = "h";
        if (format != "h" && format != "c")
        {
            gbtLog(QObject::tr("Invalid format."));
            return 1;
        }

        QTime procTime;
        procTime.start();

        QString sql;
        QSqlQuery qry(mydb);

        QDomDocument doc;
        QDomElement root;

        doc = QDomDocument("GOBLETXML");

        root = doc.createElement("ShapeXML");
        root.setAttribute("version", "1.0");
        doc.appendChild(root);

        QDomElement varName;
        QDomText varValue;

        sql = "SELECT * from datasetinfo where dataset_id = '" + tableName + "' and dataset_type = 2";
        if (qry.exec(sql))
        {
            qry.first();
            if (format == "h")
            {
                gbtLog("ID: " + tableName);
                gbtLog("Description: " + qry.value(getFieldIndex(qry,"dataset_desc")).toString());
                gbtLog("Units: " + qry.value(getFieldIndex(qry,"dataset_unit")).toString());
                gbtLog("Shape type: " + getShapeType(qry.value(getFieldIndex(qry,"shapefiletype")).toInt()));
                if (qry.value(getFieldIndex(qry,"dataset_metadata")).toByteArray().size() == 0)
                    gbtLog("Has metadata: No");
                else
                    gbtLog("Has metadata: Yes");
                gbtLog("No. Colums in raster: " + qry.value(getFieldIndex(qry,"ncols")).toString());
                gbtLog("No. Rows in raster: " + qry.value(getFieldIndex(qry,"nrows")).toString());
                gbtLog("Raster X center: " + QString::number(qry.value(getFieldIndex(qry,"xllcenter")).toFloat(),'f',15));
                gbtLog("Raster Y center: " + QString::number(qry.value(getFieldIndex(qry,"yllcenter")).toFloat(),'f',15));
            }
            else
            {
                QDomElement shapedesc;
                shapedesc = doc.createElement("ShapeInfo");
                root.appendChild(shapedesc);



                varName = doc.createElement("ID");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(tableName);
                varName.appendChild(varValue);

                varName = doc.createElement("Description");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(qry.value(getFieldIndex(qry,"dataset_desc")).toString());
                varName.appendChild(varValue);

                varName = doc.createElement("Units");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(qry.value(getFieldIndex(qry,"dataset_unit")).toString());
                varName.appendChild(varValue);

                varName = doc.createElement("ShapeType");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(getShapeType(qry.value(getFieldIndex(qry,"shapefiletype")).toInt()));
                varName.appendChild(varValue);

                varName = doc.createElement("Metadata");
                shapedesc.appendChild(varName);
                if (qry.value(getFieldIndex(qry,"dataset_metadata")).toByteArray().size() == 0)
                    varValue = doc.createTextNode("NO");
                else
                    varValue = doc.createTextNode("YES");
                varName.appendChild(varValue);

                varName = doc.createElement("RasterNCols");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(qry.value(getFieldIndex(qry,"ncols")).toString());
                varName.appendChild(varValue);

                varName = doc.createElement("RasterNRows");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(qry.value(getFieldIndex(qry,"nrows")).toString());
                varName.appendChild(varValue);

                varName = doc.createElement("RasterXCenter");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(QString::number(qry.value(getFieldIndex(qry,"xllcenter")).toFloat(),'f',15));
                varName.appendChild(varValue);

                varName = doc.createElement("RasterYCenter");
                shapedesc.appendChild(varName);
                varValue = doc.createTextNode(QString::number(qry.value(getFieldIndex(qry,"yllcenter")).toFloat(),'f',15));
                varName.appendChild(varValue);


            }
        }
        else
        {
            gbtLog(QObject::tr("Cannot read dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        QList<TfieldDef> fields;
        int ncols;
        ncols = 0;
        sql = "DESC " + tableName;
        if (qry.exec(sql))
        {
            while (qry.next())
            {
                if (qry.value(getFieldIndex(qry,"field")).toString() != "ogc_geom")
                {
                    ncols++;
                    TfieldDef field;
                    field.fieldName = qry.value(getFieldIndex(qry,"field")).toString();
                    if (qry.value(getFieldIndex(qry,"Type")).toString().contains("double"))
                    {
                        field.fieldType = "DEC";
                    }
                    else
                    {
                        if (qry.value(getFieldIndex(qry,"Type")).toString().contains("decimal"))
                            field.fieldType = "DEC";
                        else
                            field.fieldType = "INT";
                    }
                    fields.append(field);
                }
            }
        }

        QDomElement shapevars;
        shapevars = doc.createElement("ShapeFields");
        root.appendChild(shapevars);
        int pos;

        for (pos = 0; pos <= fields.count()-1;pos++)
        {
            varName = doc.createElement("Field");
            shapevars.appendChild(varName);
            varValue = doc.createTextNode(fields[pos].fieldName);
            varName.appendChild(varValue);
        }

        int nrows;
        sql = "SELECT * from " + tableName;
        if (qry.exec(sql))
        {
            nrows = 0;
            while (qry.next())
            {
                nrows++;
            }
            nrows++;

            QVector<QVector<QString> >  grid;
            grid.resize(nrows);
            int r;
            for(r=0; r<nrows; r++)
            {
                grid[r].resize(ncols);
            }
            //Append the field names


            for (pos = 0; pos <= fields.count()-1;pos++)
                grid[0][pos] = fields[pos].fieldName;
            r = 1;
            qry.first();
            QString value;
            while (qry.isValid())
            {
                for (pos = 0; pos <= fields.count()-1;pos++)
                {
                    if (fields[pos].fieldType == "DEC")
                        value = QString::number(qry.value(getFieldIndex(qry,fields[pos].fieldName)).toDouble(),'f',3);
                    else
                        value = qry.value(getFieldIndex(qry,fields[pos].fieldName)).toString();
                    grid[r][pos] = value;
                }
                r++;
                qry.next();
            }
            if (format == "h")
            {
                QVector< int> colSizes;
                colSizes.resize(ncols);
                for (pos = 0; pos <= ncols-1;pos++)
                    colSizes[pos] = 0;
                //Get the maximum size of each column
                for (pos = 0; pos <= ncols-1;pos++)
                {
                    for(r=0; r<nrows; r++)
                    {
                        if (grid[r][pos].length() +2 > colSizes[pos])
                        {
                            colSizes[pos] = grid[r][pos].length() +2;
                        }
                    }
                }
                //Print the table
                printf("\n");
                //Print top line
                for (pos=0;pos<= ncols-1;pos++)
                {
                    printf("+");
                    printf(fixLine("-",colSizes[pos]).toLocal8Bit().data());
                }
                printf("+");
                printf("\n");
                //Print the columns headings

                for (pos=0;pos<= ncols-1;pos++)
                {
                    printf("+");
                    printf(fixString(" " + grid[0][pos] + " ",colSizes[pos]).toLocal8Bit().data());

                }
                printf("+");
                printf("\n");

                //Print separation
                for (pos=0;pos<= ncols-1;pos++)
                {
                    printf("+");
                    printf(fixLine("-",colSizes[pos]).toLocal8Bit().data());
                }
                printf("+");
                printf("\n");

                //Print the values
                for (r=1;r<=nrows-1;r++)
                {
                    printf("|");
                    for (pos=0;pos<= ncols-1;pos++)
                    {
                        printf(fixString(" " + grid[r][pos] + " ",colSizes[pos]).toLocal8Bit().data());
                        printf("|");
                    }
                    printf("\n");
                }

                //Print the end
                //Print separation
                for (pos=0;pos<= ncols-1;pos++)
                {
                    printf("+");
                    printf(fixLine("-",colSizes[pos]).toLocal8Bit().data());
                }
                printf("+");
                printf("\n");
            }
            else
            {
                QDomElement shapeData;
                shapeData = doc.createElement("ShapeData");
                root.appendChild(shapeData);

                QDomElement rowData;
                QDomElement fieldData;
                QDomText fieldValue;

                for (r=1;r<=nrows-1;r++)
                {
                    rowData = doc.createElement("Row");
                    shapeData.appendChild(rowData);
                    for (pos=0;pos<= ncols-1;pos++)
                    {
                        fieldData = doc.createElement("Field");
                        fieldData.setAttribute("Name",fields[pos].fieldName);
                        rowData.appendChild(fieldData);
                        fieldValue = doc.createTextNode(grid[r][pos]);
                        fieldData.appendChild(fieldValue);
                    }

                }

                QTextStream out(stdout);
                out.setCodec("UTF-8");
                doc.save(out,1,QDomNode::EncodingFromTextStream);
            }

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

        if (format == "h")
            gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();
        con.closeConnection();

        return 0;
    }

    return 0;

}
