#ifndef INSERTSHAPE_H
#define INSERTSHAPE_H

#include <QObject>
#include <shapefil.h>
#include <QSqlDatabase>
#include <QStringList>

typedef struct {double x, y, z;} Point;

typedef struct Ring{
        Point *list;	//list of points
        struct Ring  *next;
        int		n;		//number of points in list
} Ring;

class insertShape : public QObject
{
    Q_OBJECT
public:
    explicit insertShape(QObject *parent = 0);
    void setShapeFile(QString shapeFile);
    void setTableName(QString tableName);
    //void setDatabase(QSqlDatabase database);
    QStringList getSQLs();
    int loadShape();
    int getShapeType(){return m_shapeType;}


private:
    QString m_shapeFile;
    QString m_tableName;
    //QSqlDatabase m_database;

    QStringList sqls;
    QString sql;
    int m_shapeType;

    //Private Functions from shp2mysql
    int Insert_attributes(DBFHandle hDBFHandle, int row);
    char *make_good_string(char *str);
    int ring_check(SHPObject* obj, char *table, char *sr_id, int rings, DBFHandle hDBFHandle);
    char *protect_quotes_string(char *str);
    int PIP( Point P, Point* V, int n );

    //GOBLET private functions
    void appendString(QString data);
};

#endif // INSERTSHAPE_H
