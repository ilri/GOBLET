#include "shapeToGrid.h"
#include <shapefil.h>
#include <QMutex>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>


QMutex listMutex;
QList<fileData> listFiles;
bool gridToCSVStarted;

void gbtLog2(QString message)
{
    QString temp;
    temp = message + "\n";
    printf(temp.toLocal8Bit().data());
}

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

}

void uploadCSV::setTableName(QString tableName)
{
    m_tableName = tableName;
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
                sql = sql + fileName + "' IGNORE INTO TABLE " + m_tableName;
                sql = sql + " fields terminated by ',' lines terminated by '\n' (geokey, xpos, ypos, shapeid)";
                listFiles.removeAt(0);
                listMutex.unlock();

                if (!qry.exec(sql))
                {
                    gbtLog2(tr("Error uploading table: ") + m_tableName);
                    gbtLog2(qry.lastError().databaseText());
                }
                //printf("\r%i %% inserted", perc); //Wierd... this stop working...
                printf("%i %% inserted \n",perc);
                fflush(stdout);
                //qDebug() << QString::number(perc) + "% completed";
                //QFile::remove(fileName);
            }
        }
        mydb.close();
    }
}

//**************************************ShapeFile************************************************
shapeFile::shapeFile(QObject *parent) :
    QObject(parent)
{
    m_xMin = 0;
    m_yMin = 0;
    m_zMin = 0;
    m_mMin = 0;

    m_xMax = 0;
    m_yMax = 0;
    m_zMax = 0;
    m_mMax = 0;

    nShapeType = SHP_NULLSHAPE;
    nEntities = 0;
}

bool shapeFile::open(QString fileName)
{
    /*

    *****************************************************************************
    * $Id: dbfdump.c,v 1.12 2006-06-17 00:15:08 fwarmerdam Exp $
    *
    * Project:  Shapelib
    * Purpose:  Sample application for dumping .dbf files to the terminal.
    * Author:   Frank Warmerdam, warmerdam@pobox.com
    *
    ******************************************************************************
    * Copyright (c) 1999, Frank Warmerdam
    *
    * This software is available under the following "MIT Style" license,
    * or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
    * option is discussed in more detail in shapelib.html.

    */

    /*

      The following code is based on Frank Warmerdam's dbfdump

    */

    SHPHandle	hSHP;
    int i = 0;
    int j = 0;
    int bHeaderOnly = 0;
    int iPart = 0;

    //const char 	*pszPlus;

    double adfMinBound[4], adfMaxBound[4];

    hSHP = SHPOpen(fileName.toLocal8Bit().data(), "rb" );

    if( hSHP == NULL )
    {
        gbtLog2(tr("Unable to open: ") + fileName);
        return 0;
    }
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    m_xMin = adfMinBound[0];
    m_yMin = adfMinBound[1];
    m_zMin = adfMinBound[2];
    m_mMin = adfMinBound[3];

    m_xMax = adfMaxBound[0];
    m_yMax = adfMaxBound[1];
    m_zMax = adfMaxBound[2];
    m_mMax = adfMaxBound[3];

    //**********************

    for( i = 0; i < nEntities && !bHeaderOnly; i++ )
    {
        //int		j;
        SHPObject	*psShape;

        psShape = SHPReadObject( hSHP, i );

        if( psShape == NULL )
        {
            gbtLog2("Unable to read shape " + QString::number(i) + ". Terminating object reading.\n");
            break;
        }

        TShape ishape;

        if( psShape->bMeasureIsUsed )
        {

            ishape.numparts = psShape->nParts;
            ishape.nvertices = psShape->nVertices;
            ishape.xMin = psShape->dfXMin;
            ishape.yMin = psShape->dfYMin;
            ishape.zMin = psShape->dfZMin;
            ishape.mMin = psShape->dfMMin;

            ishape.xMax = psShape->dfXMax;
            ishape.yMax = psShape->dfYMax;
            ishape.zMax = psShape->dfZMax;
            ishape.mMax = psShape->dfMMax;

        }
        else
        {

            ishape.numparts = psShape->nParts;
            ishape.nvertices = psShape->nVertices;
            ishape.xMin = psShape->dfXMin;
            ishape.yMin = psShape->dfYMin;
            ishape.zMin = psShape->dfZMin;
            ishape.mMin = 0;

            ishape.xMax = psShape->dfXMax;
            ishape.yMax = psShape->dfYMax;
            ishape.zMax = psShape->dfZMax;
            ishape.mMax = 0;

        }

        if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 )
        {
            gbtLog2("panPartStart[0] = " + QString::number(psShape->panPartStart[0]) + " not zero as expected.\n");
        }

        for( j = 0, iPart = 1; j < psShape->nVertices; j++ )
        {
            //const char	*pszPartType = "";

            //if( j == 0 && psShape->nParts > 0 )
                //pszPartType = SHPPartTypeName( psShape->panPartType[0] );

            if( iPart < psShape->nParts
                    && psShape->panPartStart[iPart] == j )
            {
                //pszPartType = SHPPartTypeName( psShape->panPartType[iPart] );
                iPart++;
                //pszPlus = "+";
            }
            //else
                //pszPlus = " ";

            if( psShape->bMeasureIsUsed )
            {
                vertice vert;
                vert.x = psShape->padfX[j];
                vert.y = psShape->padfY[j];
                vert.z = psShape->padfZ[j];
                vert.m = psShape->padfM[j];
                ishape.vertices.append(vert);

            }
            else
            {
                vertice vert;
                vert.x = psShape->padfX[j];
                vert.y = psShape->padfY[j];
                vert.z = psShape->padfZ[j];
                vert.m = psShape->padfM[j];
                ishape.vertices.append(vert);

            }
        }

        shapes.append(ishape);

        SHPDestroyObject( psShape );
    }

    SHPClose( hSHP );
    return true;
}

//**************************************Rasterizer***************************************************

Rasterizer::Rasterizer(QObject *parent) :
    QThread(parent)
{
    m_CSVSeparator = ",";
}

void Rasterizer::run()
{
    listMutex.lock();
    gridToCSVStarted = true;
    listMutex.unlock();

    convertShapeFile();
    totalNumShapes = 0;
    shapefiles.clear();

    listMutex.lock();
    gridToCSVStarted = false;
    listMutex.unlock();

}

int Rasterizer::Sign(int value)
{
    if (value == 0)
        return 0;
    else
    {
        if (value > 0)
            return 1;
        else
            return -1;
    }
}

int Rasterizer::Sign(double value)
{
    if (value == 0.0)
        return 0;
    else
    {
        if (value > 0.0)
            return 1;
        else
            return -1;
    }
}


void Rasterizer::uploadPx(int shapeID,TGridPixel pix)
{
    TGridPixel tmp;
    tmp = pix;
    tmp.shapeID = shapeID;
    shapePixels.append(tmp);
}

void Rasterizer::uploadPxList(int shapeID,QList<TGridPixel> pxList)
{
    QList<TGridPixel> temp;
    temp.append(pxList);
    for (int pos = 0; pos <= temp.count()-1;pos++)
    {
        temp[pos].shapeID = shapeID;
    }
    shapePixels.append(temp);
}


int Rasterizer::NumPercentShapes(int percent, int nShps)
{
    return trunc((( nShps * percent )*1.0/100 )) + 1;
}

bool Rasterizer::loadShapeFile(QString shpFileName, double cellSize, double noDataValue, QString gridName)
{
    bool result;
    shapeFile ShapeFile;
    bool flg;
    bool res;

    double halfx;
    double XllCenter;
    double YllCenter;
    int ncols;
    int nrows;
    int nshapes;
    TImpItem data;
    double v_xMin;
    double v_xMax;
    double v_yMin;
    double v_yMax;
    int v_cols;
    int v_rows;


    res = false;
    v_xMin = 0;
    v_xMax = 0;
    v_yMin = 0;
    v_yMax = 0;
    v_cols = 0;
    v_rows = 0;
    nshapes = 0;

    flg = ShapeFile.open(shpFileName);
    if(  flg == true )
    {
        v_xMin = trunc(( ShapeFile.getxMin() *1.0/ cellSize )) * cellSize;
        v_yMin = trunc(( ShapeFile.getyMin() *1.0/ cellSize )) * cellSize;
        v_cols = trunc( ( ShapeFile.getxMax() -  ShapeFile.getxMin() ) *1.0/ cellSize ) + 1;
        v_rows = trunc( ( ShapeFile.getyMax() -  ShapeFile.getyMin() ) *1.0/ cellSize ) + 1;
        v_xMax = v_xMin + v_cols * cellSize;
        v_yMax = v_yMin + v_rows * cellSize;
        nshapes = ShapeFile.getNumShapes();
    }
    if(  ( ( v_xMin != 0 ) && ( v_xMax != 0 ) && ( v_yMin != 0 ) &&
           ( v_yMax != 0 ) && ( v_cols != 0 ) && ( v_rows != 0 ) ) )
    {
        data.shpFileName = shpFileName;
        data.cellSize = cellSize;
        data.noDataValue = noDataValue;
        data.gridName = gridName;

        data.dataTableCode = "";
        data.dataTableDesc = "";

        data.numberofShapes = nshapes;
        totalNumShapes = totalNumShapes + nshapes;
        halfx = cellSize *1.0/ 2;

        XllCenter = v_xMin + halfx;
        YllCenter = (v_yMin + halfx);
        ncols = trunc( fabs( v_xMax - v_xMin ) *1.0/ cellSize );
        nrows = trunc( fabs( v_yMax - v_yMin ) *1.0/ cellSize );
        data.gridHeader.gridName = gridName;
        data.gridHeader.dX = cellSize;
        data.gridHeader.dY = cellSize;
        data.gridHeader.NumberCols = ncols;
        data.gridHeader.NumberRows = nrows;
        data.gridHeader.XllCenter = XllCenter;
        data.gridHeader.YllCenter = YllCenter;
        data.gridHeader.NodataValue = noDataValue;

        m_ncols = ncols;
        m_nrows = nrows;
        m_xllcenter = XllCenter;
        m_yllcenter = YllCenter;
        m_shapeType = ShapeFile.getShapefileType();

        data.procResult = false;
        shapefiles.append(data);
        res = true;
    }
    result = res;
    return result;
}

void Rasterizer::setTableName(QString tableName)
{
    m_tableName = tableName;
}

QString Rasterizer::getStrValue(int value)
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

void Rasterizer::PixListToCSV(TGridHeader GrdHd)
{
    int pos;
    int xpos;
    int ypos;

    QFile ofile;
    QString oFileName;
    QTextStream out;
    QString idsuit;
    QString outString;


    long lineSize;
    int filePart;
    long currCell;
    long perc;

    long totalCells;
    totalCells = GrdHd.NumberCols*GrdHd.NumberRows;

    long maxOutSize;

    maxOutSize = round ((totalCells * 28) * 0.25); //25% of the total file
    if (maxOutSize > 250000000)
        maxOutSize = 250000000;


    lineSize = 0;
    filePart = 1;
    currCell = 0;
    QString svalue;

    for (pos = 0; pos <= shapePixels.count()-1;pos++)
    {
      ypos = round((180/GrdHd.dX) - (fabs((-90 - GrdHd.YllCenter)/GrdHd.dX) + GrdHd.NumberRows)) + 1;
      xpos = round(fabs((-180 - GrdHd.XllCenter)/GrdHd.dX))+1;
      xpos = xpos + shapePixels[pos].col; //-1
      ypos = ypos + shapePixels[pos].row; //-1
      svalue = QString::number(shapePixels[pos].shapeID);

      //*********
      idsuit = getStrValue(ypos) + getStrValue(xpos);

      outString = idsuit + m_CSVSeparator;
      outString = outString + QString::number(xpos) + m_CSVSeparator;
      outString = outString + QString::number(ypos) + m_CSVSeparator;
      outString = outString + svalue;

      currCell++;
      //printStage(totalCells,currCell);

      if (lineSize == 0)
      {
          oFileName = "tempshpcsv_"+QString::number(filePart) + ".gbt";
          ofile.setFileName(oFileName);
          if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
          {
              gbtLog2(tr("Error opening part file"));
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


      //******
    }
    perc = 100;
    ofile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther );
    ofile.close();
    addToList(oFileName,perc);
}

bool Rasterizer::convertShapeFile()
{

    int pos;
    bool res;

    res = true;
    for( pos = 0 ; pos < shapefiles.count() ; pos++ )
    {
        shapePixels.clear();
        emit processingShape(shapefiles[pos].shpFileName,shapefiles[pos].numberofShapes);

        shapefiles[pos].procResult = ShapeFileToGrid(shapefiles[pos].shpFileName                                                     
                                                     ,shapefiles[pos].gridHeader);

        PixListToCSV(shapefiles[pos].gridHeader);

        res = shapefiles[pos].procResult;
    }

    return res;
}




bool Rasterizer::ShapeFileToGrid(QString SfNm, TGridHeader GrdHd)
{

    bool result;

    bool flg;

    shapeFile MySf;

    int sfType;

    ShapefileProcessSucess = true;


    flg = MySf.open(SfNm);
    if(  flg == false )
    {
        gbtLog2(tr("Error opening shapefile: ") + SfNm);
        result = false;
    }

    sfType = MySf.getShapefileType();

    switch( sfType )
    {
    case SHP_POLYGON:
        flg = Poly2Grid( MySf, GrdHd);
        break;
    case SHP_POLYGONM:
        flg = Poly2Grid( MySf, GrdHd);
        break;
    case SHP_POLYGONZ:
        flg = Poly2Grid( MySf, GrdHd);
        break;
    case SHP_POLYLINE:
        flg = Line2Grid( MySf, GrdHd);
        break;
    case SHP_POLYLINEM:
        flg = Line2Grid( MySf, GrdHd);
        break;
    case SHP_POLYLINEZ:
        flg = Line2Grid( MySf, GrdHd);
        break;
    case SHP_POINT:
        flg = Point2Grid( MySf, GrdHd);
        break;
    case SHP_POINTM:
        flg = Point2Grid( MySf, GrdHd);
        break;
    case SHP_POINTZ:
        flg = Point2Grid( MySf, GrdHd);
        break;
    case SHP_MULTIPOINT:
        flg = Multipoint2Grid( MySf, GrdHd);
        break;
    case SHP_MULTIPOINTM:
        flg = Multipoint2Grid( MySf, GrdHd);
        break;
    case SHP_MULTIPOINTZ:
        flg = Multipoint2Grid( MySf, GrdHd);
        break;
    default:
    {
        emit reportError( "The Shapefile: " + SfNm + " is from a not supported type");
        flg = false;
        result = flg;
    }
    }

    result = ShapefileProcessSucess;
    return result;
}


bool Rasterizer::isGridContainsShape(Shape shp,  gridHeader hdr)
{

    double gridXmin;
    double gridYmin;
    double gridXmax;
    double gridYmax;


    gridXmin = hdr.XllCenter;
    gridYmin = hdr.YllCenter;
    gridXmax = gridXmin + ( hdr.NumberCols * hdr.dX );
    gridYmax = gridYmin + ( hdr.NumberRows * hdr.dY );

    if(  ( ( shp.xMin > gridXmax ) || ( shp.yMin > gridYmax ) ||
           ( shp.xMax < gridXmin ) || ( shp.yMax < gridYmin ) ) )
        return false;
    else
        return true;
}

double Rasterizer::FirstLineXY(double xy, double xymin,  double pxsize,  int fact)
{

    double ncol;
    double resto;
    double res;

    res = xy;
    ncol = Fix((xymin - xy) *1.0/ pxsize);
    resto = fmod((xymin - xy), pxsize);
    //resto := trunc((xymin - xy)) mod trunc(pxsize);
    //resto := (xymin - xy) mod pxsize;
    if(resto > 0)
    {
        ncol = ncol + 1;
    }
    res = xymin + (fact * ncol * pxsize);
    return  res;

}

bool Rasterizer::Poly2Grid(shapeFile &PolySf, gridHeader header)
{   bool result;
    double xMin;
    double xMax;
    double x0;
    double x1;
    double x2;
    double y0;
    double y1;
    double y2;
    double m;
    double q;
    double xstart;
    int nShps;
    int nVs;
    //int i;
    int s;
    int nP;
    int tempn;
    int v;
    //bool flg;

    QList<Tbord> bordi2;
    QList<TDvalue> y_int;
    QList<TGridPixel> pixels;
    //int perc;
    //int shpsPerc;
    Shape PolyShp;
    int p;
    //perc = 1;
    nShps = PolySf.getNumShapes();
    //shpsPerc = NumPercentShapes( perc, nShps );

    for( s = 0 ; s < nShps ; s++ )
    {
        //i = 0;
        PolyShp = PolySf.shapes[s];
        nP = PolyShp.numparts;
        xMin = PolyShp.xMin;
        xMax = PolyShp.xMax;
        if(!isGridContainsShape( PolyShp, header ) )
        {
            continue;
        }

        nVs = PolyShp.nvertices-1;
        bordi2.clear();
        tempn = 0;

        for (p = 0; p <= nP -1;p++)
        {

            x0 = PolyShp.vertices[tempn].x;
            y0 = PolyShp.vertices[tempn].y;
            for( v = tempn ; v < nVs ; v++ )
            {
                x1 = PolyShp.vertices[v].x;
                x2 = PolyShp.vertices[v+1].x;
                y1 = PolyShp.vertices[v].y;
                y2 = PolyShp.vertices[v+1].y;
                if(  x1 != x2 )
                {
                    m = ( y2 - y1 ) *1.0/ ( x2 - x1 );
                    q = y1 - ( m * x1 );
                    Tbord brd;
                    brd.x1 = x1;
                    brd.x2 = x2;
                    brd.m = m;
                    brd.q = q;
                    bordi2.append(brd);
                }
                if(  ( ( x2 == x0 ) && ( y2 == y0 ) ) )
                {
                    tempn = v + 2;
                    break;
                }
            }
        }

        if(  xMin >= header.XllCenter )
        {
            xstart = FirstLineXY( xMin, header.XllCenter, header.dX, 1 );
        }
        else
        {
            xstart = FirstLineXY( xMin, header.XllCenter, header.dX, -1 );
        }
        y_int.clear();
        pixels.clear();

        do
        {
            Interseca( bordi2, y_int, xstart );
            SortArray( y_int );
            ScanLine2( y_int, xstart, pixels, header);
            xstart = xstart + header.dX;
        }
        while(!(  xMax  <= xstart ));


        uploadPxList(s,pixels);

        pixels.clear();
        emit reportProgress(s);
    }

    result = true;
    return result;
}


double Rasterizer::Fix(double number)
{
    return Sign(number) * floor(fabs(number));
}



bool Rasterizer::Interseca(QList<bord> bordi2, QList<TDvalue> &intersezioni,  double xls)
{

    bool res;
    int l;
    int nlati;
    double x1;
    double x2;
    double m;
    double q;


    res = false;
    nlati = bordi2.count();
    intersezioni.clear();
    for( l = 0 ; l < nlati ; ++l )
    {
        x1 = bordi2[l].x1;
        x2 = bordi2[l].x2;
        if(  ( ( ( xls >= x1 ) && ( xls < x2 ) ) || ( ( xls >= x2 ) && ( xls < x1 ) ) ) )
        {
            m = bordi2[l].m;
            q = bordi2[l].q;
            TDvalue Dvalue;
            Dvalue.value = m * xls + q;
            intersezioni.append(Dvalue);
        }
    }
    res = true;
    return res;
}

bool compareValue(TDvalue a, TDvalue b)
{
    return a.value < b.value;
}

bool Rasterizer::SortArray(QList<TDvalue> &vettore)
{
    qSort(vettore.begin(),vettore.end(),compareValue);
    return true;
}


bool Rasterizer::ScanLine2(QList<TDvalue> vettore, double xref, QList<TGridPixel> &pixels, gridHeader header)
{
    bool result;
    bool res;
    bool control;
    double yll;
    double ystart;
    double yend;
    double ycellstart;
    double pxsize;
    int c =-1;
    int r = -1;
    int v;


    res = false;
    r = 0;
    control = false;
    yll = header.YllCenter;
    pxsize = header.dX;
    ystart = 0;
    if(  vettore.count() > 0 )
    {
        for( v = vettore.count() -1 ; v >= 0 ; --v )
        {
            if(  control == false )
            {
                ystart = vettore[v].value;
                control = true;
            }
            else
            {
                yend = vettore[v].value;
                ycellstart = FirstLineXY(ystart, yll, pxsize, -1);

                do
                {
                    //gr.ProjToCell( xref, ycellstart, c, r );
                    if( (header.dX != 0.0) && (header.dY != 0.0) )
                    {	c = round( ( xref - header.XllCenter )/header.dX );
                        r = header.NumberRows - round( ( ycellstart - header.YllCenter )/header.dY ) - 1;
                    }

                    TGridPixel curPx;
                    curPx.col = c;
                    curPx.row = r;
                    pixels.append(curPx);
                    ycellstart = ycellstart - pxsize;
                }
                while(!(   yend  >= ycellstart ));
                //until ycellstart >= yend;
                control = false;
            }
        }
    }
    result = res;
    return result;
}



bool Rasterizer::Line2Grid(shapeFile &LineSf, gridHeader header)
{   bool result;
    //int NumParts;
    int numPoints;
    int curPartStart;
    int curPartEnd;
    int vertexCol;
    int vertexRow;
    int lastCol;
    int lastRow;
    int nShps;
    //int perc;
    //int shpsPerc;
    int s;    
    int k;    
    Shape LineShp;    
    QList<TGridPixel> vertices;
    QList<TGridPixel> pixels;



    nShps = LineSf.getNumShapes();
    //perc = 1;
    //shpsPerc = NumPercentShapes( perc, nShps );



    for( s = 0 ; s < nShps ; ++s )
    {
        LineShp = LineSf.shapes[s];
        //NumParts = LineShp.numparts;
        numPoints = LineShp.nvertices;
        if(!isGridContainsShape( LineShp, header ) )
        {
            continue;
        }

        curPartStart = 0;
        vertices.clear();
        pixels.clear();

        curPartEnd = numPoints -1;

        if(  numPoints <= 0 )
        {
            continue;
        }

        vertexCol = 0;
        vertexRow = 0;

        //curPoint = LineShp.Point[ curPartStart ];
        //Newgrd.ProjToCell( curPoint.x, curPoint.y, vertexCol, vertexRow );
        if( (header.dX != 0.0) && (header.dY != 0.0) )
        {
            vertexCol = round( ( LineShp.vertices[curPartStart].x - header.XllCenter )/header.dX );
            vertexRow = header.NumberRows - round( ( LineShp.vertices[curPartStart].y - header.YllCenter )/header.dY ) - 1;
        }

        TGridPixel px;
        px.col = vertexCol;
        px.row = vertexRow;
        vertices.append(px);
        lastCol = vertexCol;
        lastRow = vertexRow;
        for( k = curPartStart +1 ; k < curPartEnd+1 ; ++k )
        {
            //curPoint = LineShp.Point[ k ];
            //Newgrd.ProjToCell( curPoint.x, curPoint.y, vertexCol, vertexRow );
            if( (header.dX != 0.0) && (header.dY != 0.0) )
            {
                vertexCol = round( ( LineShp.vertices[k].x - header.XllCenter )/header.dX );
                vertexRow = header.NumberRows - round( ( LineShp.vertices[k].y - header.YllCenter )/header.dY ) - 1;
            }

            if(  ( ( vertexCol != lastCol ) || ( vertexRow != lastRow ) ) )
            {
                TGridPixel px2;
                px2.col = vertexCol;
                px2.row = vertexRow;
                vertices.append(px2);
                lastCol = vertexCol;
                lastRow = vertexRow;
            }
        }
        LineBresenham( vertices, pixels );

        uploadPxList(s,pixels);
        emit reportProgress( s );
    }
    result = true;
    return result;
}



void Rasterizer::LineBresenham(QList<TGridPixel> vertices,  QList<TGridPixel> &gridPixels)
{

    TGridPixel pxOne;
    TGridPixel pxTwo;
    int index;
    int num_vertices;
    int num_edges;

    index = 0;
    num_vertices = vertices.count();
    num_edges = num_vertices -1;
    if(  num_vertices < 2 )
    {
        gridPixels.append(vertices[0]);
    }
    else
    {
        LineEdge curEdge(this,vertices[0],vertices[1]);
        TGridPixel px;
        px.col = curEdge.getCurX();
        px.row = curEdge.getCurY();
        gridPixels.append(px);
        while(index < num_edges)
        {
            pxOne = vertices[index];
            pxTwo = vertices[index + 1];
            curEdge.reset(pxOne.col, pxOne.row, pxTwo.col, pxTwo.row);
            while(curEdge.NextPixel())
            {
                TGridPixel px2;
                px2.col = curEdge.getCurX();
                px2.row = curEdge.getCurY();
                gridPixels.append(px2);
            }
            index = index + 1;
        }       
    }
}

bool Rasterizer::Point2Grid(shapeFile &PointSf, gridHeader header)
{
    int nShps;
    //int perc;
    //int shpsPerc;
    int s;
    Shape shp;

    nShps = PointSf.getNumShapes();
    //perc = 1;
    //shpsPerc = NumPercentShapes( perc, nShps );
    for(s = 0 ; s < nShps ; ++s)
    {
        shp = PointSf.shapes[s];
        if(!isGridContainsShape(shp,header))
        {
            continue;
        }
        if(  shp.nvertices > 0 )
        {

            TGridPixel px;
            //Newgrd.ProjToCell( pt.x, pt.y, px->col, px->row );
            if( (header.dX != 0.0) && (header.dY != 0.0) )
            {
                px.col = round( ( shp.vertices[0].x - header.XllCenter )/header.dX );
                px.row = header.NumberRows - round( ( shp.vertices[0].y - header.YllCenter )/header.dY ) - 1;
            }

            uploadPx(s,px);

        }
        emit reportProgress( s );
    }
    return true;
}

bool Rasterizer::Multipoint2Grid(shapeFile &MultipointSf, gridHeader header)
{

    int s;
    int p;
    int nShps;
    int nPts;
    //int perc;
    //int shpsPerc;

    Shape shp;

    QList<TGridPixel> pixels;

    nShps = MultipointSf.getNumShapes();
    nPts = 0;
    //perc = 1;
    //shpsPerc = NumPercentShapes( perc, nShps );
    for( s = 0 ; s < nShps ; ++s )
    {
        shp = MultipointSf.shapes[s];
        if(!isGridContainsShape(shp,header))
        {
            continue;
        }

        nPts = shp.nvertices;
        for( p = 0 ; p < nPts ; ++p )
        {

            TGridPixel px;

            //Newgrd.ProjToCell( pt.x, pt.y, px->col, px->row );  //Double check because of out statement
            if( (header.dX != 0.0) && (header.dY != 0.0) )
            {
                px.col = round( ( shp.vertices[p].x - header.XllCenter )/header.dX );
                px.row = header.NumberRows - round( ( shp.vertices[p].y - header.YllCenter )/header.dY ) - 1;
            }

            pixels.append(px);

        }

        uploadPxList(s,pixels);

        pixels.clear();
        emit reportProgress(s);
    }
    return true;
}


