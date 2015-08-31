#ifndef WRITESHAPEFILE_H
#define WRITESHAPEFILE_H

#include <QObject>
#include <QSqlDatabase>
#include <shapefil.h>

struct fielddef
{
  QString fieldName;
  DBFFieldType fieldType;
  int fieldPos;
};
typedef fielddef TfieldDef;

class writeShapefile : public QObject
{
    Q_OBJECT
public:
    explicit writeShapefile(QObject *parent = 0);
    void setDatabase(QSqlDatabase database){m_database = database;}
    void setShapeFile(QString shapeFile){m_shapeFile = shapeFile;}
    void setSql(QString sql){m_sql = sql;}
    bool createShapeFile();
    void setOverWrite(bool overWrite){m_overWrite = overWrite;}
    void setTableName(QString tableName){m_tableName = tableName;}
    void setOutputType (QString outputType) {m_outputType = outputType;}

private:
    int getFieldIndex(QSqlQuery query,QString fieldName);
    QSqlDatabase m_database;
    QString m_sql;
    QString m_shapeFile;
    QString m_outputType;
    bool m_overWrite;
    QString m_tableName;
    void getFieldSize(QString fieldDef, int &size, int &prec);
    int shpadd( int argc, char ** argv );
    QStringList getGeometry(QString mysqlGeo);
    QStringList separateCoords(QString data);


};

#endif // WRITESHAPEFILE_H
