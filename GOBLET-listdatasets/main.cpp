#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QtXml>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QCoreApplication>

struct fielddef
{
  QString fieldName;
  QString fieldDesc;
  QString fieldType;
};
typedef fielddef TfieldDef;

int getFieldIndex(QSqlQuery query,QString fieldName)
{
    return query.record().indexOf(fieldName); //Return the index of fieldName;
}

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
        return "NA";
    }
}

QString getFieldDesc(QString fieldCode)
{
    QString res;
    res = fieldCode;

    if (fieldCode == "dataset_id")
        res = "Dataset code";

    if (fieldCode == "dataset_desc")
        res = "Description";

    if (fieldCode == "dataset_unit")
        res = "Units";

    if (fieldCode == "dataset_metadata")
        res = "Metadata";

    if (fieldCode == "dataset_type")
        res = "Dataset type";

    if (fieldCode == "ncols")
        res = "Number of columns";

    if (fieldCode == "nrows")
        res = "Number of rows";

    if (fieldCode == "xllcenter")
        res = "X center";

    if (fieldCode == "yllcenter")
        res = "Y center";

    if (fieldCode == "shapefiletype")
        res = "Shape type (if type is shape)";


    return res;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");

    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> formatArg("f","format","Format: (h)uman or (c)omputer",false,"h","string");
    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
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
        QTime procTime;
        procTime.start();

        QString sql;
        QSqlQuery qry(mydb);

        QDomDocument doc;
        QDomElement root;

        doc = QDomDocument("GOBLETXML");

        root = doc.createElement("DataSetsXML");
        root.setAttribute("version", "1.0");
        doc.appendChild(root);

        QDomElement varName;
        QDomText varValue;


        QList<TfieldDef> fields;
        int ncols;
        ncols = 0;
        sql = "DESC datasetinfo";
        if (qry.exec(sql))
        {
            while (qry.next())
            {
                ncols++;
                TfieldDef field;
                if (format == "h")
                    field.fieldDesc = getFieldDesc(qry.value(getFieldIndex(qry,"field")).toString());
                else
                    field.fieldDesc = qry.value(getFieldIndex(qry,"field")).toString();

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

        double cellSize;
        sql = "SELECT cellSize from gbtconfig";
        if (qry.exec(sql))
        {
            qry.first();
            cellSize = qry.value(0).toDouble();
        }
        else
        {
            gbtLog(QObject::tr("Cannot read dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        QDomElement cellNode;
        cellNode = doc.createElement("CellSize");
        root.appendChild(cellNode);
        varValue = doc.createTextNode(QString::number(cellSize,'f',11));
        cellNode.appendChild(varValue);


        QDomElement shapevars;
        shapevars = doc.createElement("DatasetsVars");
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


        sql = "SELECT * from datasetinfo";
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
            {
                if (format == "h")
                    grid[0][pos] = fields[pos].fieldDesc;
                else
                    grid[0][pos] = fields[pos].fieldName;
            }
            r = 1;
            qry.first();
            QString value;
            while (qry.isValid())
            {
                for (pos = 0; pos <= fields.count()-1;pos++)
                {
                    if ((fields[pos].fieldName != "dataset_type") && (fields[pos].fieldName != "shapefiletype"))
                    {
                        if (fields[pos].fieldType == "DEC")
                            value = QString::number(qry.value(getFieldIndex(qry,fields[pos].fieldName)).toDouble(),'f',15);
                        else
                        {
                            if (fields[pos].fieldName != "dataset_metadata")
                                value = qry.value(getFieldIndex(qry,fields[pos].fieldName)).toString();
                            else
                            {
                                if (qry.value(getFieldIndex(qry,fields[pos].fieldName)).toString().isEmpty())
                                    value = "NO";
                                else
                                    value = "YES";
                            }
                        }
                        grid[r][pos] = value;
                    }
                    else
                    {
                        value = qry.value(getFieldIndex(qry,fields[pos].fieldName)).toString();
                        if (fields[pos].fieldName == "dataset_type")
                        {
                            if (value == "1")
                                value = "grid";
                            else
                                value = "shape";
                        }
                        if (fields[pos].fieldName == "shapefiletype")
                        {
                            value = getShapeType(value.toInt());
                        }
                        grid[r][pos] = value;
                    }
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
                shapeData = doc.createElement("Datasets");
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
