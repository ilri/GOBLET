#include "gridtocsv.h"
#include <math.h>
#include <cmath>
#include <QFile>
#include <QTextStream>
#include <stdlib.h>
#include <qstringlist.h>
#include <QMutex>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

void gbtLog2(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

QMutex listMutex;
QList<fileData> listFiles;
bool gridToCSVStarted = true;

void addToList(QString file, long percentage)
{
    listMutex.lock();
    fileData data;
    data.file = file;
    data.percentage = percentage;
    listFiles.append(data);
    listMutex.unlock();
}



//**********************************************************************

uploadCSV::uploadCSV(QObject *parent) :
    QThread(parent)
{
    m_NODATA_value = -999;
}

void uploadCSV::setTableName(QString tableName)
{
    m_tableName = tableName;
}

void uploadCSV::setNonDataValue(double value)
{
    m_NODATA_value = value;
}

void uploadCSV::setDataBase(QString name)
{
    m_dataBase = name;
}

void uploadCSV::setHost(QString host)
{
    m_host = host;
}

void uploadCSV::setPort(int port)
{
    m_port = port;
}

void uploadCSV::setUser(QString user)
{
    m_user = user;
}

void uploadCSV::setPassword(QString password)
{
    m_password = password;
}

void uploadCSV::run()
{

    QSqlDatabase mydb;

        mydb = QSqlDatabase::addDatabase("QMYSQL","connection2");
        mydb.setHostName(m_host);
        mydb.setPort(m_port);
        mydb.setConnectOptions("MYSQL_OPT_LOCAL_INFILE=1");
        if (!m_user.isEmpty())
           mydb.setUserName(m_user);
        if (!m_password.isEmpty())
           mydb.setPassword(m_password);


    mydb.setDatabaseName(m_dataBase);
    if (mydb.open())
    {

        QDir dir;
        QString path;
        path = dir.absolutePath();
#ifdef Q_OS_WIN
        path = path.replace("\\","/");
#endif
        QString sql;
        int totFiles;
        totFiles = 0;
        QSqlQuery qry(mydb);
        QString fileName;
        int perc;
        while ((gridToCSVStarted) || (totFiles > 0))
        {
            listMutex.lock();
            totFiles = listFiles.count();
            listMutex.unlock();
            if (totFiles > 0)
            {
                sql = "LOAD DATA LOCAL INFILE '";
                listMutex.lock();
                fileName = path + dir.separator() + listFiles[0].file;

#ifdef Q_OS_WIN
        fileName = fileName.replace("\\","/");
#endif

                perc = listFiles[0].percentage;
                sql = sql + fileName + "' INTO TABLE " + m_tableName;
                sql = sql + " fields terminated by ',' lines terminated by '\n' (geokey, xpos, ypos, cellvalue, classCode)";
                listFiles.removeAt(0);
                listMutex.unlock();
                if (!qry.exec(sql))
                {
                    gbtLog2(tr("Error uploading table: ") + m_tableName);
                    gbtLog2(qry.lastError().databaseText());
                }
                //printf("\r%i %% inserted", perc); //Wierd.. this stop working!!!
                printf("%i %% inserted \n",perc);
                fflush(stdout);
                //qDebug() << QString::number(perc) + "% completed";
                QFile::remove(fileName);
            }
        }
        mydb.close();
    }
}

//****************************************

gridToCSV::gridToCSV(QObject *parent) :
    QThread(parent)
{
    m_abort = false;    
    error = false;
    m_CSVSeparator = ",";

}

double gridToCSV::getDoubleValue(QString value)
{
    QString temp;
    int pos;
    pos = value.indexOf(" ");
    temp = value.right(value.length()-pos);
    temp = temp.replace(" ","");
    return temp.toDouble();
}

double gridToCSV::getIntValue(QString value)
{
    QString temp;
    int pos;
    pos = value.indexOf(" ");
    temp = value.right(value.length()-pos);
    temp = temp.replace(" ","");
    return temp.toInt();
}

QString gridToCSV::getStrValue(int value)
{
  int pos;
  QString svalue;
  QString tvalue;

  svalue = QString::number(value);
  tvalue = svalue;
  for (pos = 1; pos <= 7-tvalue.length(); pos++)
  {
    svalue = "0" + svalue;
  }
  return svalue;
}

void gridToCSV::setNonDataValue(double value)
{
    NODATA_value = value;
}

void gridToCSV::run()
{
    listMutex.lock();
    gridToCSVStarted = true;
    listMutex.unlock();

    QFile file(m_gridFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        error = true;
        listMutex.lock();
        gridToCSVStarted = false;
        listMutex.unlock();
        return;
    }


    QTextStream in(&file);
    QString line;
    long lineSize;
    int filePart;

    QFile ofile;
    QString oFileName;
    QTextStream out;

    long inSize;
    inSize = file.size();


    long maxOutSize;
    maxOutSize = (long)((inSize * 6.7)*0.25); //The result CVS is in average 6.7 times bigger than the orignal ascii file

    if (maxOutSize > 250000000) //The maximum part is 250 MB
        maxOutSize = 250000000;

    bool finish;
    finish = false;

    while (!in.atEnd())
    {
        line = in.readLine();
        if (line.contains("ncols",Qt::CaseInsensitive))
        {
            ncols = getIntValue(line);
        }
        if (line.contains("nrows",Qt::CaseInsensitive))
        {
            nrows = getIntValue(line);
        }
        if (line.contains("xllcorner",Qt::CaseInsensitive))
        {
            xllcorner = getDoubleValue(line);
        }
        if (line.contains("yllcorner",Qt::CaseInsensitive))
        {
            yllcorner = getDoubleValue(line);
        }
        if (line.contains("cellsize",Qt::CaseInsensitive))
        {
            cellsize = getDoubleValue(line);
        }
        if (line.contains("NODATA_value",Qt::CaseInsensitive))
        {
            NODATA_value = getDoubleValue(line);
            finish = true;
        }
        if (finish)
            break;
        if (m_abort)
            return;
    }
    in.seek(0);

    int ypos;
    int xpos;

    ypos = round((180/cellsize) - (fabs((-90 - yllcorner)/cellsize) + nrows)) + 1;
    xpos = round(fabs((-180 - xllcorner)/cellsize))+1;

    long totalCells;
    totalCells = ncols*nrows;

    emit totCells(totalCells);

    long currCell;
    long perc;
    perc = 0;

    int txpos;
    double txdeg;
    double ydeg;
    ydeg = yllcorner;
    bool start;
    start = false;
    int pos;
    QString svalue;
    QString outString;
    QString idsuit;
    currCell = 0;

    lineSize = 0;
    filePart = 1;

    while (!in.atEnd())
    {
        txpos = xpos;
        txdeg = xllcorner;
        line = in.readLine();

        if (start)
        {
            if (!line.contains("NODATA_value",Qt::CaseInsensitive))
            {
                svalue = "";
                for (pos = 0; pos <= line.length()-1;pos++)
                {
                    if (line[pos] != ' ')
                        svalue = svalue + line[pos];
                    else
                    {
                        if (!svalue.isEmpty())
                        {
                            idsuit = getStrValue(ypos) + getStrValue(txpos);
                            outString = idsuit + m_CSVSeparator;
                            outString = outString + QString::number(txpos) + m_CSVSeparator;
                            outString = outString + QString::number(ypos) + m_CSVSeparator;
                            outString = outString + svalue + m_CSVSeparator + "null"; //

                            currCell++;
                            //printStage(totalCells,currCell);

                            if (svalue.toDouble() != NODATA_value )
                            {
                                if (lineSize == 0)
                                {
                                    oFileName = "tempcsv_"+QString::number(filePart) + ".gbt";
                                    ofile.setFileName(oFileName);
                                    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
                                    {
                                        gbtLog2(tr("Error opening part file"));
                                        error = true;
                                        return;
                                    }
                                    out.setDevice(&ofile);

                                }
                                out << outString << "\n";
                                lineSize = lineSize + outString.toLatin1().size();
                                if (lineSize > maxOutSize)
                                {
                                    ofile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther );
                                    ofile.close();
                                    filePart++;
                                    lineSize = 0;
                                    perc = (long)round((currCell*100)/totalCells);
                                    addToList(oFileName,perc);
                                }
                            }

                            svalue = "";
                            txpos = txpos +1;
                            if (xllcorner >= 0)
                                txdeg = txdeg + cellsize;
                            else
                                txdeg = txdeg - cellsize;
                        }
                    }
                }
                ypos = ypos +1;
                if (yllcorner >= 0)
                    ydeg = ydeg - cellsize;
                else
                    ydeg = ydeg + cellsize;
            }
        }

        if (line.contains("NODATA_value",Qt::CaseInsensitive))
        {
            start = true;
        }
        if (line.contains("cellsize",Qt::CaseInsensitive))
        {
            start = true;
        }

        if (m_abort)
            return;
    }        
    perc = 100;
    ofile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther );
    ofile.close();
    addToList(oFileName,perc);

    listMutex.lock();
    gridToCSVStarted = false;
    listMutex.unlock();


}

void gridToCSV::stopProcess()
{
    mutex.lock();
    m_abort = true;
    mutex.unlock();
}
