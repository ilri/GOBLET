#include <QObject>
#include "mydbconn.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QTime>
#include <QtXml>
#include <tclap/CmdLine.h>
#include <QCoreApplication>

struct datasetInfo
{
  QString name;
  QString code;
};
typedef datasetInfo TdatasetInfo;

struct fielddef
{
  QString fieldName;
  QString fieldDesc;
  QString fieldType;
};
typedef fielddef TfieldDef;


bool datasetExists(QString dataset, QSqlDatabase db)
{
    QSqlQuery qry(db);
    QString sql;
    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
                return false;
            else
                return true;
        }
        else
            return false;
    }
    else
        return false;

}

int findDataset(QString dataset, QList <TdatasetInfo> list)
{
    for (int pos = 0; pos <= list.count()-1;pos++)
    {
        if (list[pos].name == dataset)
            return 1;
    }
    return 0;
}

int constructSQL(QString calculation, QString &resultSQL, QSqlDatabase db, QList <TdatasetInfo> &datasets)
{
    int openB;
    int closeB;

    openB = calculation.count("(");
    closeB = calculation.count(")");

    if (openB != closeB)
        return 1;


    QStringList letters;

    letters.append("A");
    letters.append("B");
    letters.append("C");
    letters.append("D");
    letters.append("E");
    letters.append("F");
    letters.append("G");
    letters.append("H");
    letters.append("I");
    letters.append("J");
    letters.append("K");
    letters.append("L");
    letters.append("M");
    letters.append("N");
    letters.append("O");
    letters.append("P");
    letters.append("Q");
    letters.append("R");
    letters.append("S");
    letters.append("T");
    letters.append("U");
    letters.append("V");
    letters.append("W");
    letters.append("X");
    letters.append("Y");
    letters.append("Z");


    QString calcstr;


    calcstr = calculation.replace(" ",""); //Remove all spaces
    //Replace all operands with spaces;
    calcstr = calcstr.replace("("," ");
    calcstr = calcstr.replace(")"," ");
    calcstr = calcstr.replace("+"," ");
    calcstr = calcstr.replace("-"," ");
    calcstr = calcstr.replace("*"," ");
    calcstr = calcstr.replace("/"," ");
    calcstr = calcstr.simplified(); //Remove spaces from start / end


    //Extract datasets
    int pos;
    QString dataset;
    int dsetcount;
    dsetcount = 0;

    TdatasetInfo combdset;
    combdset.code = "A";
    combdset.name = "combdataset";
    datasets.append(combdset);

    for (pos = 0; pos <= calcstr.length()-1;pos++)
    {
        if (calcstr[pos] != ' ')
        {
            dataset = dataset + calcstr[pos];
        }
        else
        {
            dataset = dataset.simplified();
            if (!dataset.isEmpty())
            {
                if (datasetExists(dataset,db))
                {
                    if (!findDataset(dataset,datasets))
                    {
                        dsetcount++;
                        TdatasetInfo dset;
                        dset.name = dataset;
                        dset.code = letters[dsetcount];
                        datasets.append(dset);
                    }
                }
            }
            dataset = "";
        }
    }
    dataset = dataset.simplified();
    if (!dataset.isEmpty())
    {
        if (datasetExists(dataset,db))
        {
            if (!findDataset(dataset,datasets))
            {
                dsetcount++;
                TdatasetInfo dset;
                dset.name = dataset;
                dset.code = letters[dsetcount];
                datasets.append(dset);
            }
        }
    }

    if (datasets.count() == 1)
        return 1;

    //Replaces all datasets in the calculation to its cellValue
    calcstr = calculation.replace(" ",""); //Remove all spaces
    for (pos = 1; pos <= datasets.count()-1;pos++)
    {
        calcstr = calcstr.replace(datasets[pos].name,"T" + datasets[pos].code + ".cellvalue");
    }

    //Begin the construct of the result SQL

    QString resultSelect;
    resultSelect = "SELECT T" + datasets[0].code + ".comCode," + calcstr;

    QString resultFrom;
    resultFrom = " FROM ";

    for (pos = 0; pos <= datasets.count()-1;pos++)
    {
        resultFrom = resultFrom + datasets[pos].name + " AS T" + datasets[pos].code + ",";
    }
    resultFrom = resultFrom.left(resultFrom.length()-1);

    QString resultWhere;


    //Link the tables
    if (datasets.count() > 1)
    {
        resultWhere = " WHERE ";
        for (pos = 1; pos <= datasets.count()-1;pos++)
        {
            resultWhere = resultWhere + "T" + datasets[pos-1].code + ".geokey = T" + datasets[pos].code + ".geokey AND ";
        }
        resultWhere = resultWhere.left(resultWhere.length()-5);
    }



    resultSQL = resultSelect + resultFrom + resultWhere + " GROUP BY T" + datasets[0].code + ".comCode" + " ORDER BY T" + datasets[0].code + ".comCode";

    return 0;

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

QStringList getDescriptions(QString description)
{
    QStringList res;
    QString desc;
    res.append("Class code");
    for (int pos = 1; pos <= description.length();pos++)
    {
        if (description[pos-1] != ',')
        {
            desc = desc + description[pos-1];
        }
        else
        {
            res.append(desc);
            desc = "";
        }
    }
    if (!desc.isEmpty())
        res.append(desc);
    return res;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> calculationArg("c","calculation","Calculation to perform. For example: 'sum(DatasetA),sum(DatasetB)' ",true,"","string");
    TCLAP::ValueArg<std::string> ouputArg("o","output","Output type: (h)uman readable or (c)omputer readable",true,"","string");


    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> descArg("s","descriptions","Descriptions for the calculations separated by coma. Default value is the calculation string",false,"","string");
    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(calculationArg);
    cmd.add(ouputArg);


    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(descArg);


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
    QString calculation = QString::fromUtf8(calculationArg.getValue().c_str());
    QString format = QString::fromUtf8(ouputArg.getValue().c_str());
    QString description = QString::fromUtf8(descArg.getValue().c_str());

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

        QList <TdatasetInfo> datasets;

        if (constructSQL(calculation,sql,mydb,datasets))
        {
            gbtLog(QObject::tr("Error in calculation."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }


        QDomDocument doc;
        QDomElement root;

        doc = QDomDocument("GOBLETXML");

        root = doc.createElement("CalcXML");
        root.setAttribute("version", "1.0");
        doc.appendChild(root);

        QDomElement varName;
        QDomText varValue;

        QList<TfieldDef> fields;
        int ncols;
        ncols = 0;

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Cannot reset dataset."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }
        int nfields;
        nfields = qry.record().count();
        QString nfield;

        int pos;
        QStringList descriptions;
        descriptions = getDescriptions(description);

        if (descriptions.count() != nfields)
        {
            descriptions.clear();
            descriptions.append("Class code");
            for (pos = 1; pos <= nfields-1; pos++)
            {
                descriptions.append(qry.record().field(pos).name());
            }
        }


        for (pos = 0; pos <= nfields-1;pos++)
        {
            ncols++;
            nfield = descriptions[pos];
            nfield.replace("T" + datasets[pos].code + ".",datasets[pos].name + ".");
            nfield.replace(".cellvalue","");
            TfieldDef field;
            field.fieldName = nfield;
            field.fieldDesc = nfield; //Change for description

            if (pos == 0)
            {
                field.fieldType = "CHAR";
            }
            else
                field.fieldType = "DEC";
            fields.append(field);
        }

        QDomElement shapevars;
        shapevars = doc.createElement("Values");
        root.appendChild(shapevars);


        for (pos = 0; pos <= fields.count()-1;pos++)
        {
            varName = doc.createElement("Field");
            shapevars.appendChild(varName);
            varValue = doc.createTextNode(fields[pos].fieldDesc);
            varName.appendChild(varValue);
        }

        int nrows;

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

        for (pos = 0; pos <= fields.count()-1;pos++)
            grid[0][pos] = fields[pos].fieldDesc;
        r = 1;
        qry.first();
        QString value;
        while (qry.isValid())
        {
            for (pos = 0; pos <= fields.count()-1;pos++)
            {
                if (fields[pos].fieldType == "DEC")
                    value = QString::number(qry.value(pos).toDouble(),'f',3);
                else
                    value = qry.value(pos).toString();
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
            shapeData = doc.createElement("CalcData");
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
                    fieldData.setAttribute("Name",fields[pos].fieldDesc);
                    rowData.appendChild(fieldData);
                    fieldValue = doc.createTextNode(grid[r][pos]);
                    fieldData.appendChild(fieldValue);
                }

            }

            QTextStream out(stdout);
            out.setCodec("UTF-8");
            doc.save(out,1,QDomNode::EncodingFromTextStream);
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
