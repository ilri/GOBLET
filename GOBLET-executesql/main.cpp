#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QDebug>
#include <QCoreApplication>
#include <QtXml>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

struct fielddef
{
  QString fieldName;
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
            if (query.record().field(pos).type() != QVariant::Double)
                varValue = doc.createTextNode(query.value(getFieldIndex(query,query.record().field(pos).name())).toString());
            else
                varValue = doc.createTextNode(QString::number(query.value(getFieldIndex(query,query.record().field(pos).name())).toDouble(),'f',5));
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
    TCLAP::ValueArg<std::string> SQLArg("s","sqlstatement","SQL Statement to execute",true,"","string");
    //Non required arguments
    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Password. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> outputTypeArg("t","outputType","Output type (h)uman or (c)omputer. Default human",false,"h","string");
    TCLAP::ValueArg<std::string> outputFileArg("o","outputFile","If output type is computer, output xml file. Default ./executesql_out.xml",false,"./executesql_out.xml","string");

    cmd.add(databaseArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(SQLArg);
    cmd.add(outputTypeArg);
    cmd.add(outputFileArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString SQLStatement = QString::fromUtf8(SQLArg.getValue().c_str());
    QString outputType = QString::fromUtf8(outputTypeArg.getValue().c_str());
    QString outputFile = QString::fromUtf8(outputFileArg.getValue().c_str());

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


        QSqlQuery qry(mydb);
#ifdef Q_OS_UNIX
        SQLStatement = SQLStatement.replace("\"","'");
#endif

        if (qry.exec(SQLStatement))
        {
            if ((SQLStatement.left(6).toUpper() == "SELECT") ||
                    (SQLStatement.left(4).toUpper() == "DESC") ||
                    (SQLStatement.left(4).toUpper() == "SHOW"))
            {
                if (outputType == "h")
                    printResult(qry);
                else
                    genXML(outputFile,qry);

            }
        }
        else
        {
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

        if (outputType == "h")
            gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();


        return 0;
    }

    return 0;

}
