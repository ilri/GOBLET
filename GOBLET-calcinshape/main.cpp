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
  QString alias;
  QString operation;
};
typedef datasetInfo TdatasetInfo;

struct fielddef
{
  QString fieldName;
  QString fieldDesc;
  QString fieldType;
};
typedef fielddef TfieldDef;


QStringList getFields(QString fieldStr, QString table, QSqlDatabase db)
{
    QStringList res;
    QSqlQuery qry(db);
    QString sql;
    sql = "DESC " + table;
    if (qry.exec(sql))
    {
        if (fieldStr == "all")
        {
            while (qry.next())
            {
                if (qry.value(0).toString() != "ogc_geom")
                    res.append(qry.value(0).toString());
            }
        }
        else
        {
             QStringList temp;
             temp = fieldStr.split(",", QString::SkipEmptyParts);
             for (int pos = 0; pos <= temp.count()-1;pos++)
             {
                 qry.first();
                 while (qry.isValid())
                 {
                     if (temp[pos].toUpper() == qry.value(0).toString().toUpper())
                     {
                         res.append(temp[pos]);
                     }
                     qry.next();
                 }
             }
        }
        if (res.count() == 0)
        {
            qry.first();
            while (qry.isValid())
            {
                if (qry.value(0).toString() != "ogc_geom")
                    res.append(qry.value(0).toString());
                qry.next();
            }
        }
    }
    if (!res.contains("shapeid",Qt::CaseInsensitive))
        res.prepend("shapeid");
    return res;
}


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

bool shapeFileExists(QString dataset, QSqlDatabase db)
{
    QSqlQuery qry(db);
    QString sql;
    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

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

QString getDataSetCode(QString dataset, QList <TdatasetInfo> list)
{
    for (int pos = 0; pos <= list.count()-1;pos++)
    {
        if (list[pos].name == dataset)
            return list[pos].code;
    }
    return "";
}

int constructSQL(QString calculation, QString &resultSQL, QSqlDatabase db, QString shapeFile, QStringList fields, QString where, QString group, QString descriptions)
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

    QList <TdatasetInfo> datasets;
    QList <TdatasetInfo> datasets2;

    //TB is the raster of the shape, TA is the shape
    TdatasetInfo combdset;
    combdset.code = "B";
    combdset.name = shapeFile + "_raster";
    datasets.append(combdset);

    int dsetcount;
    dsetcount = 1;

    QStringList calcs;
    calcs = calculation.split(",", QString::SkipEmptyParts);
    QString dataset;
    QString operation;
    QString temp;
    int pos;
    int start;
    int end;
    for (pos = 0; pos <= calcs.count()-1;pos++)
    {
        temp = calcs[pos];
        start = temp.indexOf("(");
        end = temp.indexOf(")");
        dataset = temp.mid(start+1,end-start-1);
        operation = temp.left(start);

        if (datasetExists(dataset,db))
        {
            if (!findDataset(dataset,datasets))
            {
                //Stores only unique datasets
                dsetcount++;
                TdatasetInfo dset;
                dset.name = dataset;
                dset.code = letters[dsetcount];
                datasets.append(dset);
            }
            //Stores all the datasets
            TdatasetInfo dset2;
            dset2.name = dataset;
            dset2.operation = operation;
            datasets2.append(dset2);
        }
    }

    if (datasets.count() == 1)
        return 1;


    QStringList lstdescs;
    if (!descriptions.isEmpty())
    {
        lstdescs = descriptions.split(",", QString::SkipEmptyParts);
        if (datasets2.count() != lstdescs.count())
        {
            gbtLog("Descriptions does not match calculations");
            return 1;
        }
    }

    QString calcstr;
    for (pos = 0; pos <= datasets2.count()-1;pos++)
    {
        calcstr = calcstr + datasets2[pos].operation + "(T";
        calcstr = calcstr + getDataSetCode(datasets2[pos].name,datasets);
        calcstr = calcstr + ".cellValue)";
        if (lstdescs.count() > 0)
            calcstr = calcstr + " '" + lstdescs[pos] + "',";
        else
            calcstr = calcstr + ",";
    }
    calcstr = calcstr.left(calcstr.length()-1);


    //Begin the construct of the result SQL

    QString resultSelect;
    //resultSelect = "SELECT T" + datasets[0].code + ".comCode," + calcstr;
    resultSelect = "SELECT ";
    for (pos = 0; pos <= fields.count()-1;pos++)
        resultSelect = resultSelect + "TA." + fields[pos] + ",";
    resultSelect = resultSelect + calcstr;


    QString resultFrom;
    resultFrom = " FROM " + shapeFile + " AS TA,";

    for (pos = 0; pos <= datasets.count()-1;pos++)
    {
        resultFrom = resultFrom + datasets[pos].name + " AS T" + datasets[pos].code + ",";
    }
    resultFrom = resultFrom.left(resultFrom.length()-1);

    QString resultWhere;


    //Link the tables
    if (datasets.count() > 1)
    {
        resultWhere = " WHERE TA.shapeid = TB.shapeid AND ";
        for (pos = 1; pos <= datasets.count()-1;pos++)
        {
            resultWhere = resultWhere + "T" + datasets[pos-1].code + ".geokey = T" + datasets[pos].code + ".geokey AND ";
        }
        resultWhere = resultWhere.left(resultWhere.length()-5);
    }

    if (!where.isEmpty())
        resultWhere = resultWhere + " AND  " + where;

    if (group.isEmpty())
        group = "TA.shapeid";
    resultSQL = resultSelect + resultFrom + resultWhere + " GROUP BY " + group + " ORDER BY TA.shapeid";

    return 0;

}

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

/*QStringList getDescriptions(QString description)
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
}*/


void genXML(QString fileName, QSqlQuery query)
{
    QDomDocument doc;
    QDomElement root;

    doc = QDomDocument("GOBLETXML");

    root = doc.createElement("SQLResultXML");
    root.setAttribute("version", "1.0");
    doc.appendChild(root);

    QDomElement varName;
    QDomText varValue;

    QDomElement querycols;
    querycols = doc.createElement("ResultColumns");
    root.appendChild(querycols);
    int pos;
    for (pos = 0; pos <= query.record().count()-1;pos++)
    {
        varName = doc.createElement("Column");
        querycols.appendChild(varName);
        varValue = doc.createTextNode(query.record().field(pos).name());
        varName.appendChild(varValue);
    }


    QDomElement querydata;
    querydata = doc.createElement("ResultData");
    root.appendChild(querydata);

    while (query.next())
    {
        QDomElement queryRow;
        queryRow = doc.createElement("Row");
        querydata.appendChild(queryRow);
        for (pos = 0; pos <= query.record().count()-1;pos++)
        {
            varName = doc.createElement("Column");
            varName.setAttribute("name",query.record().field(pos).name());
            queryRow.appendChild(varName);
            varValue = doc.createTextNode(query.value(getFieldIndex(query,query.record().field(pos).name())).toString());
            varName.appendChild(varValue);
        }
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        doc.save(out,1,QDomNode::EncodingFromTextStream);
        file.close();
    }

}

void printResult(QSqlQuery query)
{
    int pos;
    QList<TfieldDef> fields;
    int ncols;
    ncols = 0;
    for (pos = 0; pos <= query.record().count()-1;pos++)
    {
        ncols++;
        TfieldDef field;
        field.fieldName = query.record().field(pos).name();


        switch (query.record().field(pos).type())
        {
        case QVariant::Bool:
            field.fieldType = "BOL";
            break;
        case QVariant::Char:
            field.fieldType = "CHR";
            break;
        case QVariant::Date:
            field.fieldType = "DAT";
            break;
        case QVariant::DateTime:
            field.fieldType = "DTM";
            break;
        case QVariant::Double:
            field.fieldType = "DBL";
            break;
        case QVariant::Int:
            field.fieldType = "INT";
            break;
        case QVariant::String:
            field.fieldType = "STR";
            break;
        case QVariant::Time:
            field.fieldType = "TIM";
            break;
        case QVariant::UInt:
            field.fieldType = "UIN";
            break;
        default:
            field.fieldType = "NA";
        }
        fields.append(field);
    }
    int nrows;
    nrows = 0;
    while (query.next())
    {
        nrows++;
    }
    nrows++; //Heads

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

    for (pos = 0; pos <= fields.count()-1;pos++)
        grid[0][pos] = fields[pos].fieldName;
    r = 1;
    query.first();
    QString value;
    while (query.isValid())
    {
        for (pos = 0; pos <= fields.count()-1;pos++)
        {
            if (fields[pos].fieldType == "DBL")
                value = QString::number(query.value(getFieldIndex(query,fields[pos].fieldName)).toDouble(),'f',3);
            else
            {
                if (fields[pos].fieldName != "ogc_geom")
                    value = query.value(getFieldIndex(query,fields[pos].fieldName)).toString().simplified();
                else
                    value = "Geometry data";
            }
            grid[r][pos] = value;
        }
        r++;
        query.next();
    }

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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> calculationArg("c","calculation","Calculation to perform. For example: 'sum(DatasetA),sum(DatasetB)' ",true,"","string");    
    TCLAP::ValueArg<std::string> shapeArg("s","shapefile","Shapefile to use",true,"","string");



    //Non required arguments
    TCLAP::ValueArg<std::string> pathArg("a","path","Path to database. Default .",false,".","string");
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",false,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",false,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",false,"","string");
    TCLAP::ValueArg<std::string> descArg("S","descriptions","Descriptions for the calculations separated by coma. Default value is the calculation string",false,"","string");
    TCLAP::ValueArg<std::string> fieldArg("f","fields","Field of shapefile to include in result separared by coma. Default value all",false,"all","string");
    TCLAP::ValueArg<std::string> whereArg("w","where","WHERE statement for shapefile",false,"","string");
    TCLAP::ValueArg<std::string> groupArg("g","group","GROUP fields statements for shapefile",false,"","string");
    TCLAP::ValueArg<std::string> ouputArg("t","outputType","Output type: (h)uman readable or (c)omputer readable",false,"h","string");
    TCLAP::ValueArg<std::string> outputFileArg("o","outputFile","If output type is computer, output xml file. Default ./executesql_out.xml",false,"./executesql_out.xml","string");

    //Switches
    TCLAP::SwitchArg remoteSwitch("r","remote","Connect to remote host", cmd, false);
    cmd.add(databaseArg);
    cmd.add(calculationArg);
    cmd.add(ouputArg);
    cmd.add(shapeArg);
    cmd.add(whereArg);
    cmd.add(groupArg);


    cmd.add(pathArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(descArg);
    cmd.add(fieldArg);
    cmd.add(outputFileArg);


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
    QString shape = QString::fromUtf8(shapeArg.getValue().c_str());
    QString cmdWhere = QString::fromUtf8(whereArg.getValue().c_str());
    QString cmdgroup = QString::fromUtf8(groupArg.getValue().c_str());
    QString strfields = QString::fromUtf8(fieldArg.getValue().c_str());
    QString outfile = QString::fromUtf8(outputFileArg.getValue().c_str());


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

        if (!shapeFileExists(shape,mydb))
        {
            gbtLog(QObject::tr("The shapefile does not exists"));
            mydb.close();
            con.closeConnection();
            return 1;
        }



        QStringList sfields;
        sfields = getFields(strfields,shape,mydb);

        if (constructSQL(calculation,sql,mydb,shape,sfields,cmdWhere,cmdgroup,description))
        {
            gbtLog(QObject::tr("Error in calculation."));            
            mydb.close();
            con.closeConnection();
            return 1;
        }

        //gbtLog(sql);

        if (!qry.exec(sql))
        {
            gbtLog(QObject::tr("Error in calculation."));
            gbtLog(qry.lastError().databaseText());
            mydb.close();
            con.closeConnection();
            return 1;
        }

        if (format == "h")
            printResult(qry);
        else
            genXML(outfile,qry);

        //---------------Print or save result-------------------------------



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
