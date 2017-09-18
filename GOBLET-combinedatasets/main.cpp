#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QTime>
#include <tclap/CmdLine.h>
#include <QPoint>
#include <QPointF>
#include <math.h>
#include <QCoreApplication>
#include <QVariant>

void gbtLog(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

QString getStrValue(int value)
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

QString getWhereClauseFromExtent(QString extent,QSqlDatabase db, QString table)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (extent.count(" ") != 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count(",") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count("(") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    if (extent.count(")") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return QString();
    }
    int pos;
    pos = extent.indexOf(" ");
    QString from;
    QString to;
    from = extent.left(pos);
    to = extent.mid(pos+1,extent.length()-pos+1);
    from.replace("(","");
    from.replace(")","");
    to.replace("(","");
    to.replace(")","");

    //Get UpperLeft
    QPointF dupperLeft;
    pos = from.indexOf(",");
    dupperLeft.setX(from.left(pos).toDouble());
    dupperLeft.setY(from.mid(pos+1,from.length()-pos+1).toDouble());


    //Get lower right
    QPointF dlowerRight;
    pos = to.indexOf(",");
    dlowerRight.setX(to.left(pos).toDouble());
    dlowerRight.setY(to.mid(pos+1,to.length()-pos+1).toDouble());

    QSqlQuery qry(db);
    QString sql;

    double dbCellSize;
    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return QString();
    }
    else
        return QString();

    if (dbCellSize == 0.0)
        return QString();
    double xllCenter;
    double yllCenter;
    int NumberRows;
    sql = "SELECT xllCenter,yllCenter,nrows FROM datasetinfo WHERE dataset_id = '" + table + "'";
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            xllCenter = qry.value(0).toDouble();
            yllCenter = qry.value(1).toDouble();
            NumberRows = qry.value(2).toInt();
        }
        else
            return QString();
    }
    else
        return QString();

    QPoint upperLeft;
    QPoint lowerRight;

    int xpos;
    int ypos;

    if(dbCellSize != 0.0)
    {
        //Plots the bounds in the grid
        upperLeft.setX(round( ( dupperLeft.x() - xllCenter )/dbCellSize ));
        upperLeft.setY(NumberRows - round( ( dupperLeft.y() - yllCenter )/dbCellSize ) - 1);


        QPointF UL;
        UL = dupperLeft;

        lowerRight.setX(round( ( dlowerRight.x() - xllCenter )/dbCellSize ));
        lowerRight.setY(NumberRows - round( ( dlowerRight.y() - yllCenter )/dbCellSize ) - 1);

        QPointF LR;
        LR = dlowerRight;

        //Plots the bounds in the GOBLET coordinate system
        ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + NumberRows)) + 1;
        xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

        upperLeft.setX(xpos + upperLeft.x());
        upperLeft.setY(ypos + upperLeft.y());

        lowerRight.setX(xpos + lowerRight.x());
        lowerRight.setY(ypos + lowerRight.y());

        //Crete the where clause

        QString idsuit;
        idsuit = getStrValue(upperLeft.y()) + getStrValue(upperLeft.x());

        QString where;
        where = " TA.geokey BETWEEN '" + idsuit + "' AND ";

        idsuit = getStrValue(lowerRight.y()) + getStrValue(lowerRight.x());
        where = where + "'" + idsuit + "'";
        return where;

    }

    return QString();
}

QString firstGrid;

void getDataSetList(QString dataSetString, QStringList &datasets)
{
    int pos;
    QString dataset;
    QString cadena;
    cadena = dataSetString;
    pos = 0;
    while (pos <= cadena.length()-1)
    {
        if (cadena[pos] != ',')
        {
            dataset = dataset + cadena[pos];
            pos++;
        }
        else
        {
            if (datasets.indexOf(dataset) == -1)
            {
                datasets.append(dataset);
            }
            dataset = "";
            cadena = cadena.mid(pos+1,cadena.length()-pos+1);
            pos = 0;
        }
    }
    if (datasets.indexOf(dataset) == -1)
    {
        datasets.append(dataset);
    }

    if (datasets.count() > 0)
        firstGrid = datasets[0];
}

QString getShapeClause(QString shapeConstraint,QSqlDatabase db)
{
    //ShapeDataSet:shapeID,ShapeID,
    if (shapeConstraint.count(":") != 1)
    {
        gbtLog(QObject::tr("Error in shape constraint"));
        return QString();
    }

    int pos;
    pos = shapeConstraint.indexOf(":");

    QString dataset = shapeConstraint.left(pos);

    QSqlQuery qry(db);
    QString sql;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(dataset + QObject::tr(" is not a shape dataset"));
                return QString();
            }
        }
        else
            return QString();
    }
    else
        return QString();

    sql = "SELECT geokey FROM " + dataset + "_raster WHERE ";
    QString shapes = shapeConstraint.mid(pos+1,shapeConstraint.length()-pos+1);
    QString shape;

    pos =0;
    while (pos <= shapes.length()-1)
    {
        if (shapes[pos] != ',')
        {
            shape = shape + shapes[pos];
            pos++;
        }
        else
        {
            sql = sql + " shapeid = " + shape + " OR ";
            shape = "";
            shapes = shapes.mid(pos+1,shapes.length()-pos+1);
            pos = 0;
        }
    }
    sql = sql + "shapeid = " + shape;

    return sql;
}

//Latest version
int getGridValuesFromExtent(QString extent,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    //(1.3333,32.1212321) (-4.12121,41.212121)
    if (extent.count(" ") != 1)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count(",") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count("(") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    if (extent.count(")") != 2)
    {
        gbtLog(QObject::tr("Extent is invalid"));
        return 1;
    }
    int pos;
    pos = extent.indexOf(" ");
    QString from;
    QString to;
    from = extent.left(pos);
    to = extent.mid(pos+1,extent.length()-pos+1);
    from.replace("(","");
    from.replace(")","");
    to.replace("(","");
    to.replace(")","");

    //Get UpperLeft
    QPointF dupperLeft;
    pos = from.indexOf(",");
    dupperLeft.setX(from.left(pos).toDouble());
    dupperLeft.setY(from.mid(pos+1,from.length()-pos+1).toDouble());


    //Get lower right
    QPointF dlowerRight;
    pos = to.indexOf(",");
    dlowerRight.setX(to.left(pos).toDouble());
    dlowerRight.setY(to.mid(pos+1,to.length()-pos+1).toDouble());

    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;

    if (dbCellSize == 0.0)
        return 1;



    //*************************
    double v_xMin;
    double v_xMax;
    double v_yMin;
    double v_yMax;
    double halfx;
    int v_cols;
    int v_rows;


    v_xMin = trunc(( dupperLeft.x() *1.0/ dbCellSize )) * dbCellSize;
    v_yMin = trunc(( dlowerRight.y() *1.0/ dbCellSize )) * dbCellSize;
    v_cols = trunc( ( dlowerRight.x() - dupperLeft.x() ) *1.0/ dbCellSize ) + 1;
    v_rows = trunc( ( dupperLeft.y() - dlowerRight.y() ) *1.0/ dbCellSize ) + 1;
    v_xMax = v_xMin + v_cols * dbCellSize;
    v_yMax = v_yMin + v_rows * dbCellSize;

    halfx = dbCellSize *1.0/ 2;

    xllCenter = v_xMin + halfx;
    yllCenter = (v_yMin + halfx) - 0.0200;
    nrows = trunc( fabs( v_yMax - v_yMin ) *1.0/ dbCellSize );
    ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);

    //*******************

    left = ceil((dupperLeft.x() + 180) / dbCellSize);
    bottom = ceil((90 - dupperLeft.y()) / dbCellSize);

    return 0;


}

//Latest and most updated version
int calcBoundFromShapes(QString shapeConstraint,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    //ShapeDataSet:shapeID,ShapeID,
    if (shapeConstraint.count(":") != 1)
    {
        gbtLog(QObject::tr("Error in shape constraint"));
        return 1;
    }

    int pos;
    pos = shapeConstraint.indexOf(":");

    QString dataset = shapeConstraint.left(pos);

    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;

    sql = "SELECT count(*) FROM datasetinfo WHERE dataset_id = '" + dataset + "' and dataset_type = 2";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            if (qry.value(0).toInt() == 0)
            {
                gbtLog(QObject::tr(" is not a shape dataset"));
                return 1;
            }
        }
        else
            return 1;
    }
    else
        return 1;

    sql = "SELECT shapeid,AsText(max(envelope(ogc_geom))) as boundbox FROM " + dataset + " WHERE ";
    QString shapes = shapeConstraint.mid(pos+1,shapeConstraint.length()-pos+1);
    QString shape;

    pos =0;
    while (pos <= shapes.length()-1)
    {
        if (shapes[pos] != ',')
        {
            shape = shape + shapes[pos];
            pos++;
        }
        else
        {
            sql = sql + " shapeid = " + shape + " OR ";
            shape = "";
            shapes = shapes.mid(pos+1,shapes.length()-pos+1);
            pos = 0;
        }
    }
    sql = sql + "shapeid = " + shape;
    sql = sql + " GROUP BY shapeid";

    double maxX;
    double maxY;
    double minX;
    double minY;

    maxX = -3000;
    maxY = -3000;
    minX = 3000;
    minY = 3000;


    QString cadena;
    QString geopos;

    double temp;
    if (qry.exec(sql))
    {
        while (qry.next())
        {
            cadena = qry.value(1).toString();
            pos = cadena.indexOf("(");
            cadena = cadena.mid(pos+1,cadena.length()-pos+1); //Remove the type of shape
            cadena = cadena.replace("(","");
            cadena = cadena.replace(")","");
            pos = 0;
            while (pos <= cadena.length()-1)
            {
                if (cadena[pos] != ',')
                {
                    geopos = geopos + cadena[pos];
                    pos++;
                }
                else
                {
                    cadena = cadena.mid(pos+1,cadena.length()-pos+1);

                    pos = geopos.indexOf(" ");
                    temp = geopos.left(pos).toDouble();
                    if (temp > maxX)
                        maxX = temp;
                    if (temp < minX)
                        minX = temp;

                    temp = geopos.mid(pos+1,geopos.length()-pos+1).toDouble();

                    if (temp > maxY)
                        maxY = temp;
                    if (temp < minY)
                        minY = temp;

                    pos = 0;
                    geopos = "";
                }
            }

            //Final string
            pos = geopos.indexOf(" ");
            temp = geopos.left(pos).toDouble();
            if (temp > maxX)
                maxX = temp;
            if (temp < minX)
                minX = temp;

            temp = geopos.mid(pos+1,geopos.length()-pos+1).toDouble();

            if (temp > maxY)
                maxY = temp;
            if (temp < minY)
                minY = temp;

        }
        QPointF UL;
        QPointF LR;

        UL.setX(minX);
        UL.setY(maxY);

        LR.setX(maxX);
        LR.setY(minY);


        double v_xMin,v_xMax,v_yMin,v_yMax;
        double halfx;
        int v_cols,v_rows;

        v_xMin = trunc(UL.x() / dbCellSize) * dbCellSize;
        v_yMin = trunc(LR.y() / dbCellSize) * dbCellSize;
        v_cols = trunc((LR.x() -  UL.x()) / dbCellSize) + 1;
        v_rows = trunc((UL.y() -  LR.y()) / dbCellSize) + 1;
        v_xMax = v_xMin + v_cols * dbCellSize;
        v_yMax = v_yMin + v_rows * dbCellSize;

        halfx = dbCellSize / 2;
        xllCenter = v_xMin + halfx;
        yllCenter = (v_yMin + halfx) - 0.0200;
        ncols = trunc(fabs(v_xMax - v_xMin) / dbCellSize);
        nrows = trunc(fabs(v_yMax - v_yMin) / dbCellSize);

        left = ceil((UL.x() + 180) / dbCellSize);
        bottom = ceil((90 - UL.y()) / dbCellSize);


        return 0;

    }
    else
        return 1;
}

//Most updated version
int calcExtendFromGrid(QString gridName,QSqlDatabase db, double &dbCellSize, double &xllCenter, double &yllCenter, int &ncols, int &nrows, int &bottom, int &left)
{
    QSqlQuery qry(db);
    QString sql;

    dbCellSize = 0.0;

    sql = "SELECT cellSize FROM gbtconfig";
    if (qry.exec(sql))
    {
        if (qry.first())
            dbCellSize = qry.value(0).toDouble();
        else
            return 1;
    }
    else
        return 1;



    sql = "SELECT xllcenter,yllcenter,ncols,nrows FROM datasetinfo WHERE dataset_id = '" + gridName + "' and dataset_type = 1";

    if (qry.exec(sql))
    {
        if (qry.first())
        {
            xllCenter = qry.value(0).toDouble();
            yllCenter = qry.value(1).toDouble() - 0.0200;
            ncols = qry.value(2).toInt();
            nrows = qry.value(3).toInt();
        }
        else
            return 1;
    }
    else
        return 1;

    int xpos;
    int ypos;
    ypos = round((180/dbCellSize) - (fabs((-90 - yllCenter)/dbCellSize) + nrows)) + 1;
    xpos = round(fabs((-180 - xllCenter)/dbCellSize))+1;

    sql = "SELECT max(xpos),max(ypos),min(xpos),min(ypos) FROM " + gridName;
    if (qry.exec(sql))
    {
        if (qry.first())
        {
            //double utx;
            //double uty;

            //utx = ((qry.value(0).toInt() - xpos) * dbCellSize) + xllCenter;
            //uty = (((((qry.value(1).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            double ltx;
            double lty;

            ltx = ((qry.value(2).toInt() - xpos) * dbCellSize) + xllCenter;
            lty = (((((qry.value(3).toInt() - ypos) + 1) - nrows) * dbCellSize) - yllCenter) * -1;

            QPointF UL;

            UL.setX(ltx);
            UL.setY(lty);


            left = ceil((UL.x() + 180) / dbCellSize);
            bottom = ceil((90 - UL.y()) / dbCellSize);

            return 0;

        }
        else
            return 1;
    }
    else
        return 1;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //Command line arguments
    TCLAP::CmdLine cmd("GOBLET (c) 2012, International Livestock Research Institute (ILRI) \n Developed by Carlos Quiros (c.f.quiros@cgiar.org)", ' ', "1.0 (Beta 1)");
    //Required arguments
    TCLAP::ValueArg<std::string> databaseArg("d","database","Database name",true,"","string");
    TCLAP::ValueArg<std::string> datasetArg("t","datasets","Datasets to combine. For example: 'dataset1,dataset2,dataset3,....'",true,"","string");
    //Non required arguments

    TCLAP::ValueArg<std::string> hostArg("H","host","Connect to host. Default localhost",true,"localhost","string");
    TCLAP::ValueArg<std::string> portArg("P","port","Port number to use. Default 3306",false,"3306","string");
    TCLAP::ValueArg<std::string> userArg("u","user","User. Default empty",true,"","string");
    TCLAP::ValueArg<std::string> passArg("p","password","Passwork. Default no password",true,"","string");
    TCLAP::ValueArg<std::string> extentArg("e","extent","Extent: '(upperLeft degrees lat,log) (lowerRight degrees lat,log)'",false,"","string");
    TCLAP::ValueArg<std::string> shpConstraintArg("S","constraintbyshapes","Constraint classification using shapes: ShapeDataSet:shapeID,ShapeID,....",false,"","string");
    TCLAP::ValueArg<std::string> functionArg("f","combfunction","Combination function sum or (mul)tiplication Default sum",false,"sum","string");
    TCLAP::ValueArg<std::string> targetDatasetArg("T","targetDataset","Target dataset if stored. Default combinationoutput",false,"combinationoutput","string");
    TCLAP::ValueArg<std::string> targetDatasetDescArg("s","targetDatasetDesc","Description of target dataset if stored. Default: Result of combination ...",false,"Result of combination ...","string");


    TCLAP::SwitchArg storeSwitch("o","store","Store the combination as a dataset", cmd, false);
    TCLAP::SwitchArg overWriteSwitch("O","overwrite","Overwrite dataset if exists", cmd, false);
    cmd.add(databaseArg);
    cmd.add(datasetArg);
    cmd.add(hostArg);
    cmd.add(portArg);
    cmd.add(userArg);
    cmd.add(passArg);
    cmd.add(extentArg);
    cmd.add(shpConstraintArg);
    cmd.add(functionArg);
    cmd.add(targetDatasetArg);
    cmd.add(targetDatasetDescArg);

    //Parsing the command lines
    cmd.parse( argc, argv );

    //Getting the variables from the command

    bool store =  storeSwitch.getValue();
    bool overwrite = overWriteSwitch.getValue();
    QString dbName = QString::fromUtf8(databaseArg.getValue().c_str());
    QString host = QString::fromUtf8(hostArg.getValue().c_str());
    QString port = QString::fromUtf8(portArg.getValue().c_str());
    QString userName = QString::fromUtf8(userArg.getValue().c_str());
    QString password = QString::fromUtf8(passArg.getValue().c_str());
    QString tableName = QString::fromUtf8(datasetArg.getValue().c_str());
    QString extent = QString::fromUtf8(extentArg.getValue().c_str());
    QString shapes = QString::fromUtf8(shpConstraintArg.getValue().c_str());
    QString function = QString::fromUtf8(functionArg.getValue().c_str());
    QString targetDataset = QString::fromUtf8(targetDatasetArg.getValue().c_str());
    QString targetDatasetDesc = QString::fromUtf8(targetDatasetDescArg.getValue().c_str());

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
        if (function.isEmpty())
            function = "sum";

        if (function != "sum" &&
                function != "mul")
        {
            gbtLog(QObject::tr("Invalid function."));
            return 1;
        }

        if (!tableName.contains(","))
        {
            gbtLog(QObject::tr("You need to indicate two or more classified datasets"));
            mydb.close();

            return 1;
        }

        QStringList dataSets;
        getDataSetList(tableName,dataSets);

        if (dataSets.count() < 2)
        {
            gbtLog(QObject::tr("You need to indicate two or more classified datasets"));
            mydb.close();

            return 1;
        }

        if (dataSets.count() > 20 && function == "sum")
        {
            gbtLog(QObject::tr("You cannot classify more than 20 datasets using sum as function"));
            mydb.close();

            return 1;
        }

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


        QTime procTime;
        procTime.start();

        QString sqlSelect;
        QSqlQuery qry(mydb);

        sqlSelect = "SELECT T" + letters[0] + ".geokey,";
        sqlSelect = sqlSelect + "T" + letters[0] + ".xpos,";
        sqlSelect = sqlSelect + "T" + letters[0] + ".ypos,";
        sqlSelect = sqlSelect + "(";
        int pos;
        for (pos = 0; pos <= dataSets.count()-1;pos++)
        {
            if (function == "sum")
                sqlSelect = sqlSelect + "T" + letters[pos] + ".classCode + ";
            else
                sqlSelect = sqlSelect + "T" + letters[pos] + ".classCode * ";
        }
        sqlSelect = sqlSelect.left(sqlSelect.length()-3) + ") as comCode";

        QString sqlFrom;
        sqlFrom = " FROM ";
        for (pos = 0; pos <= dataSets.count()-1;pos++)
        {
            sqlFrom = sqlFrom + dataSets[pos] + " T" + letters[pos] + ",";
        }
        sqlFrom = sqlFrom.left(sqlFrom.length()-1);


        QString extentWhere;
        if (!extent.isEmpty())
            extentWhere =  getWhereClauseFromExtent(extent,mydb,dataSets[0]);

        QString shapeFromSQL;
        QString shapeWhereSQL;
        QString shapeClause;
        if (!shapes.isEmpty())
        {

            shapeClause = getShapeClause(shapes,mydb);

            if (!shapeClause.isEmpty())
            {

                QString sqlcreate;
                sqlcreate = "CREATE TEMPORARY TABLE tmpshapes (";
                sqlcreate = sqlcreate + "geokey VARCHAR(14) NOT NULL ,";
                sqlcreate = sqlcreate + "PRIMARY KEY (geokey))";
                sqlcreate = sqlcreate + " ENGINE = MyISAM";
                if (qry.exec(sqlcreate))
                {
                    QString extentClause;
                    extentClause = extentWhere;
                    extentClause.replace("TA.","");

                    if (!extentClause.isEmpty())
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause + " AND " + extentClause;
                    else
                        sqlcreate = "INSERT INTO tmpshapes " + shapeClause;

                    gbtLog(QObject::tr("Preselecting shapes"));
                    if (qry.exec(sqlcreate))
                    {
                        shapeFromSQL = ", tmpshapes TK";
                        shapeWhereSQL = " TA.geokey = TK.geokey";
                    }
                    else
                    {
                        gbtLog(QObject::tr("Cannot insert temporary shapes."));
                        gbtLog(qry.lastError().databaseText());
                    }
                }
                else
                {
                    gbtLog(QObject::tr("Cannot shapes temporary table."));
                    gbtLog(qry.lastError().databaseText());
                }
            }
        }

        sqlFrom = sqlFrom + shapeFromSQL;

        //Link the tables
        QString sqlWhere;
        sqlWhere = " WHERE ";
        for (pos = 1; pos <= dataSets.count()-1;pos++)
        {
            sqlWhere = sqlWhere + "T" + letters[pos-1] + ".geokey = ";\
            sqlWhere = sqlWhere + "T" + letters[pos] + ".geokey AND ";
        }

        if (!extentWhere.isEmpty())
            sqlWhere = sqlWhere + extentWhere + " AND ";

        if (!shapeWhereSQL.isEmpty())
            sqlWhere = sqlWhere + shapeWhereSQL + " AND ";


        ////Append the not null
        //for (pos = 0; pos <= dataSets.count()-1;pos++)
       // {
       //     sqlWhere = sqlWhere + "T" + letters[pos] + ".classCode IS NOT NULL AND ";
       // }
        sqlWhere = sqlWhere.left(sqlWhere.length()-5);

        QString sql;
        sql = sqlSelect + sqlFrom + sqlWhere;

        sql = "INSERT INTO combdataset (geokey,xpos,ypos,comCode) " + sql;

        gbtLog("Deleting previous combination");

        QString sql2;
        sql2 = "DELETE FROM combdataset";

        if (qry.exec(sql2))
        {
            sql2 = "ALTER TABLE combdataset DISABLE KEYS";
            if (qry.exec(sql2))
            {
                bool error;
                QString dberror;

                gbtLog("Combining datasets. Please wait....");

                if (qry.exec(sql))
                {
                    error = false;
                    sql2 = "ALTER TABLE combdataset ENABLE KEYS";
                    if (!qry.exec(sql2))
                    {
                        gbtLog("Unable to enable keys in combination table");
                        mydb.close();

                        return 1;
                    }
                }
                else
                {
                    dberror = qry.lastError().databaseText();
                    error = true;
                }
                if (error)
                {
                    sql2 = "ALTER TABLE combdataset ENABLE KEYS";
                    if (!qry.exec(sql2))
                    {
                        gbtLog("Unable to enable keys in combination table");
                        mydb.close();

                        return 1;
                    }
                    gbtLog("Unable to combine datasets");
                    gbtLog(dberror);
                    mydb.close();

                    return 1;
                }
            }
            else
            {
                gbtLog("Unable to disable keys in combination table");
                mydb.close();

                return 1;
            }
        }
        else
        {
            gbtLog("Unable to delete preivous combinations");
            mydb.close();

            return 1;
        }


        if (store)
        {
            gbtLog("Storing combination as a dataset");

            if (overwrite)
            {
                sql = "DROP TABLE IF EXISTS " + targetDataset;
                qry.exec(sql);

                sql = "DELETE FROM datasetinfo WHERE dataset_id = '" + targetDataset + "'";
                if (!qry.exec(sql))
                {
                    gbtLog(QObject::tr("Cannot remove previous dataset."));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }
            }

            sql = "CREATE TABLE " + targetDataset + " (";
            sql = sql + "geokey VARCHAR(14) NOT NULL ,";
            sql = sql + "xpos DECIMAL(7) NULL ,";
            sql = sql + "ypos DECIMAL(7) NULL ,";
            sql = sql + "cellvalue DECIMAL(14,5) NULL DEFAULT -9999 ,";
            sql = sql + "classCode BIGINT  NULL DEFAULT NULL ,";
            //sql = sql + "classColour VARCHAR(9) NULL DEFAULT NULL ,"; //,
            sql = sql + "PRIMARY KEY (geokey) ,";
            sql = sql + "INDEX " + targetDataset + "_IDXVALUE (cellvalue ASC)) ENGINE = MyISAM";


            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot create dataset. It might already exists"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }

            sql = "ALTER TABLE " + targetDataset + " DISABLE KEYS";
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot disable keys"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }

            sql = "INSERT INTO " + targetDataset + " (geokey,xpos,ypos,cellvalue) SELECT geokey,xpos,ypos,comCode FROM combdataset";
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Unable to store combination"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }

            sql = "ALTER TABLE " + targetDataset + " ENABLE KEYS";
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot enable keys"));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }

            int ncols;
            int nrows;
            double xllCenter;
            double yllCenter;
            double dbCellSize;
            int bottom;
            int left;

            if (!extent.isEmpty())
            {
                if (getGridValuesFromExtent(extent,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
                {
                    gbtLog(QObject::tr("Error in extent"));
                    gbtLog(qry.lastError().databaseText());
                    mydb.close();

                    return 1;
                }
            }
            else
            {
                if (!shapes.isEmpty())
                {
                    if (calcBoundFromShapes(shapes,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
                    {
                        gbtLog(QObject::tr("Error in constraining shapes"));
                        gbtLog(qry.lastError().databaseText());
                        mydb.close();

                        return 1;
                    }
                }
                else
                {
                    if (calcExtendFromGrid(firstGrid,mydb,dbCellSize,xllCenter,yllCenter,ncols,nrows,bottom,left))
                    {
                        gbtLog(QObject::tr("Error in grid extent"));
                        gbtLog(qry.lastError().databaseText());
                        mydb.close();

                        return 1;
                    }
                }
            }

            sql = "select count(distinct xpos),count(distinct ypos) from " + targetDataset;
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot read dataset."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
            qry.first();

            ncols = qry.value(0).toInt();
            nrows = qry.value(1).toInt();

            sql = "select min(xpos),max(ypos) from " + targetDataset;
            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot read dataset."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }
            qry.first();

            int xpos;
            int ypos;

            xpos = qry.value(0).toInt() -1;
            ypos = qry.value(1).toInt() -1;

            sql = "INSERT INTO datasetinfo (dataset_id,dataset_desc,dataset_type,ncols,nrows,xllcenter,yllcenter)";
            sql = sql + " VALUES ('" + targetDataset +"',";
            sql = sql + "'" + targetDatasetDesc +"',1,";
            sql = sql + QString::number(ncols) + ",";
            sql = sql + QString::number(nrows) + ",";
            sql = sql + QString::number(((xpos * dbCellSize) - 180),'f',15) + ",";
            sql = sql + QString::number((90 - (ypos * dbCellSize)),'f',15) + ")"; // + 0.0200 so its reduce

            if (!qry.exec(sql))
            {
                gbtLog(QObject::tr("Cannot insert dataset."));
                gbtLog(qry.lastError().databaseText());
                mydb.close();

                return 1;
            }


        }

        int Hours;
        int Minutes;
        int Seconds;
        int Milliseconds;

        Milliseconds = procTime.elapsed();

        Hours = Milliseconds / (1000*60*60);
        Minutes = (Milliseconds % (1000*60*60)) / (1000*60);
        Seconds = ((Milliseconds % (1000*60*60)) % (1000*60)) / 1000;

        gbtLog("Finished in " + QString::number(Hours) + " Hours," + QString::number(Minutes) + " Minutes and " + QString::number(Seconds) + " Seconds.");

        mydb.close();


        return 0;
    }

    return 0;

}
