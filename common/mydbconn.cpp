#include "mydbconn.h"
#include <QDir>
#include <qglobal.h>
#include <QFile>
#include <QTextStream>
#include <stdlib.h>
#include <QDebug>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

myDBConn::myDBConn(QObject *parent) :
    QObject(parent)
{
    m_connected = false;
    m_drv = 0;
}

long myDBConn::getBufferSize()
{
    long res;
    res = 16777216; //Minimum required

#ifdef Q_OS_WIN
    //We need to check how to do it!
    //MEMORYSTATUSEX statex;
    //statex.dwLength = sizeof (statex);
    //GlobalMemoryStatusEx (&statex);
    //res = statex.ullAvailPhys;
#endif

#ifdef Q_OS_UNIX
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return res;

    QTextStream in(&file);
    QString line = in.readLine();
    QString freeMem;
    while (!line.isNull())
    {
        if (line.contains("MemFree"))
        {
            freeMem = line;
            break;
        }
        line = in.readLine();
    }
    if (!freeMem.isEmpty())
    {
        int pos;
        pos = freeMem.indexOf(":");
        freeMem = freeMem.right(freeMem.length()-pos-1);
        freeMem = freeMem.replace("kB","");
        freeMem = freeMem.replace(" ","");
        res = (freeMem.toFloat() * 1024)*0.25;
        if (res > 256217728) //No more than 256 Megabytes
            res = 256217728;
    }
#endif

#ifdef Q_OS_DARWIN
    //No idea how to do it!
    //Will use minimum required then
#endif
    return res;
}

int myDBConn::connectToDB(QString path)
{
    QDir dir;
    dir.setPath(path);
    if (!dir.exists())
    {
        gbtLog(tr("The target path does not exists"));
        return 0;
    }

    QString npath;
    npath = path;
#ifdef Q_OS_WIN
    npath = npath.replace("\\","/");
#endif

    if (!m_connected)
    {

        QString dbParam;
        dbParam = "--datadir="+npath;

        QString kbSize;
        kbSize = "--key_buffer_size="+QString::number(getBufferSize());


        QByteArray ba = dbParam.toLocal8Bit();
        char *c_str2 = ba.data();

        QByteArray ba2 = kbSize.toLocal8Bit();
        char *c_str3 = ba2.data();


        QString baseParam;
        baseParam = "--basedir="+npath;
        QByteArray ba3 = baseParam.toLocal8Bit();
        char *c_str4 = ba3.data();



        static char *server_options[] = \
        { (char *)"mysql_test",
          c_str2,

          c_str4,

          (char *)"--default-storage-engine=MyISAM",
          (char *)"--loose-innodb=0",
          (char *)"--skip-grant-tables=1",
          (char *)"--myisam-recover=FORCE",
          c_str3,
          (char *)"--character-set-server=utf8",
          (char *)"--collation-server=utf8_bin",
          NULL };

        int num_elements = (sizeof(server_options) / sizeof(char *)) - 1;


        mysql_library_init(num_elements, server_options, NULL);
        m_mysql = mysql_init(NULL);

        int valopt;
        valopt = 1;
        if (mysql_options(m_mysql,MYSQL_OPT_LOCAL_INFILE,(void *)&valopt))
            gbtLog("Error setting option");

        mysql_real_connect(m_mysql, NULL,NULL,NULL, "database1", 0,NULL,0);

        m_drv = new GOBLETDriver(m_mysql);

        if (m_drv)
        {
            m_connected = true;
            return 1;
        }
    }
    return 0;
}

int myDBConn::closeConnection()
{
    if (m_connected)
    {
        mysql_close(m_mysql);
        mysql_library_end();
        return 1;
    }
    return 0;
}

GOBLETDriver *myDBConn::getDriver()
{
    return m_drv;
}
