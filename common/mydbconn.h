#ifndef MYDBCONN_H
#define MYDBCONN_H

#include <QObject>
#include <QSqlDatabase>

#ifdef Q_OS_WIN
#include <winsock.h>
#endif

#include <mysql.h>
#include "embdriver.h"

void gbtLog(QString message);

class myDBConn : public QObject
{
    Q_OBJECT
public:
    explicit myDBConn(QObject *parent = 0);
    int connectToDB(QString path);
    int closeConnection();
    GOBLETDriver *getDriver();
private:
    MYSQL *m_mysql;
    GOBLETDriver *m_drv;
    bool m_connected;
    long getBufferSize();
};

#endif // MYDBCONN_H
