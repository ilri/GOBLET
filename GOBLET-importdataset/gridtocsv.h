#ifndef GRIDTOCSV_H
#define GRIDTOCSV_H

#include <QThread>
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>

struct fileData
{
  QString file;
  long percentage;
};
typedef fileData TfileData;

class uploadCSV : public QThread
{
    Q_OBJECT
public:
    uploadCSV(QObject *parent = 0);
    void run();
    void setTableName(QString tableName);
    void setDataBase(QString name);
    void setHost(QString host);
    void setPort(int port);
    void setUser(QString user);
    void setPassword(QString password);
    void setNonDataValue(double value);
private:
    QString m_tableName;
    QString m_dataBase;
    //For remote
    QString m_host;
    int m_port;
    QString m_user;
    QString m_password;
    double m_NODATA_value;
};

class gridToCSV : public QThread
{
    Q_OBJECT
public:
    gridToCSV(QObject *parent = 0);
    void run();
    void setGridFile(QString fileName){m_gridFile = fileName;}
    bool error;
    double getXllCenter(){return xllcorner;}
    double getYllCenter(){return yllcorner;}
    int getNCols(){return ncols;}
    int getNRows(){return nrows;}
    void setNonDataValue(double value);
private:
    bool m_abort;
    QMutex mutex;
    QString m_gridFile;
    QString m_CSVSeparator;
    int ncols;
    int nrows;
    double xllcorner;
    double yllcorner;
    double cellsize;
    double NODATA_value;
    double getDoubleValue(QString value);
    double getIntValue(QString value);
    QString getStrValue(int value);    
signals:
    void totCells(long total);

public slots:
    void stopProcess();

};

#endif // GRIDTOCSV_H
