#ifndef INCLUDED_SHAPETOGRID_H
#define INCLUDED_SHAPETOGRID_H

#include <QtCore>
#include "LineEdge.h"

struct ShapeItem
{
    QString fileName;
    double cellSize;
    double NoData;
    bool createNewDataTable;
    QString dataTableCode;
    QString dataTableDesc;
    bool tableSelected;
    bool fieldSelected;
};
typedef ShapeItem TShapeItem;


struct gridHeader
{
    QString gridName;
    int NumberCols;
    int NumberRows;
    double NodataValue;
    double dX;
    double dY;
    double XllCenter;
    double YllCenter;
};
typedef gridHeader TGridHeader;

struct ImpItem
{
    QString shpFileName;
    double cellSize;
    double noDataValue;
    QString gridName;
    QString dataTableCode;
    QString dataTableDesc;
    TGridHeader gridHeader;
    //IGrid resultGrid;
    int numberofShapes;
    bool procResult;
};
typedef ImpItem TImpItem;


struct bord
{
    double x1;
    double x2;
    double m;
    double q;
};
typedef bord Tbord;

struct Dvalue
{
    double value;
};
typedef Dvalue TDvalue;



struct ImpFldInfo
{
    bool isNewField;
    QString appendtoField;
    bool useShapeID;
    QString shapeFieldCod;
    int shapeFieldID;
    QString tblFieldCod;
    QString tblFieldDes;
    QString tblFieldTyp;
    QString tblFieldUnt;
    int tblFieldSize;
    QVariant ShapeFieldValue;
};
typedef ImpFldInfo TImpFldInfo;

enum ShpfileType {
    SHP_NULLSHAPE = 0,
      SHP_POINT = 1,
      SHP_POLYLINE = 3,
      SHP_POLYGON = 5,
      SHP_MULTIPOINT = 8,
      SHP_POINTZ = 11,
      SHP_POLYLINEZ = 13,
      SHP_POLYGONZ = 15,
      SHP_MULTIPOINTZ = 18,
      SHP_POINTM = 21,
      SHP_POLYLINEM = 23,
      SHP_POLYGONM = 25,
      SHP_MULTIPOINTM = 28,
      SHP_MULTIPATCH = 31
};

struct vertice
{
    double x;
    double y;
    double z;
    double m;
};
typedef vertice Tvertice;

struct Shape
{
    int numparts;
    int nvertices;
    ShpfileType type;
    double xMin;
    double yMin;
    double zMin;
    double mMin;
    double xMax;
    double yMax;
    double zMax;
    double mMax;
    QList<Tvertice> vertices;
};
typedef Shape TShape;

class shapeFile : public QObject
{
    Q_OBJECT
public:
    shapeFile(QObject *parent = 0);
    bool open(QString fileName);
    int getShapefileType(){return nShapeType;}
    int getNumShapes(){return nEntities;}
    //min
    double getxMin(){return m_xMin;}
    double getyMin(){return m_yMin;}
    double getzMin(){return m_zMin;}
    double getmMin(){return m_mMin;}
    //max
    double getxMax(){return m_xMax;}
    double getyMax(){return m_yMax;}
    double getzMax(){return m_zMax;}
    double getmMax(){return m_mMax;}

    QList<TShape> shapes;

private:
    int nShapeType;
    int nEntities;
    //Min
    double m_xMin;
    double m_yMin;
    double m_zMin;
    double m_mMin;
    //Max
    double m_xMax;
    double m_yMax;
    double m_zMax;
    double m_mMax;
};

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
    void setPath(QString path);
    void setDataBase(QString name);
    void setRemote(bool remote);
    void setHost(QString host);
    void setPort(int port);
    void setUser(QString user);
    void setPassword(QString password);
private:
    QString m_tableName;
    QString m_path;
    QString m_dataBase;
    //For remote
    bool m_remote;
    QString m_host;
    int m_port;
    QString m_user;
    QString m_password;
};


/*
********************************************************************************************************
File name: Rasterizer.cs
Description: Internal class, provides methods for converting polyline and polygon shapefiles to grid
********************************************************************************************************
The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy of the License at
http:www.mozilla.org/MPL/
Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
ANY KIND, either express or implied. See the License for the specific language governing rights and
limitations under the License.

The Original Code for rasterizing polygons (Scanline algorithm) was written by Enrico Chiaraidia.
Jiri Kadlec implemented the polyline rasterization (Bresenham algorithm, partially base on the MapWinGIS
ActiveX c++ open source code (LineBresenham.cpp) written by Rob Cairns

For more information about the scanline algorithm, go to
http:www.cs.rit.edu/~icss571/filling/ ('Polygon fill teaching tool', department of computer sciences,
Rochester Institute of Technology)
For Bresenham algorithm, see the Wikipedia (http:en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)

 Authors: Enrico Chiraidia (provided the MapWindow script for rasterizing polygons in vb.net)
          Jiri Kadlec (rewritten the code in c#, implemented Bresenham algorithm for lines

Contributor(s): (Open source contributors should list themselves and their modifications here).
12/21/2007 Jiri Kadlec provided the initial implementation of polygon and polyline rasterization
algorithms.
********************************************************************************************************

*/

/*

GOBLET implements a QT port of Rasterizer.

*/


class Rasterizer : public QThread
{
    Q_OBJECT

public:
    Rasterizer(QObject *parent = 0);
    void run();

    QList<TImpItem> shapefiles;
    int totalNumShapes;
    void setTableName(QString tableName);

    bool loadShapeFile(QString shpFileName, double cellSize, double noDataValue, QString gridName);

    double getXllCenter(){return m_xllcenter;}
    double getYllCenter(){return m_yllcenter;}
    int getNCols(){return m_ncols;}
    int getNRows(){return m_nrows;}
    int getShapeType(){return m_shapeType;}

private:
    bool ShapefileProcessSucess;

    QList<TGridPixel> shapePixels;
    QString m_tableName;
    QString m_CSVSeparator;

    int m_ncols;
    int m_nrows;
    double m_xllcenter;
    double m_yllcenter;
    int m_shapeType;

    bool convertShapeFile();
    bool ShapeFileToGrid(QString SfNm,TGridHeader GrdHd);

    bool Poly2Grid(shapeFile &PolySf, gridHeader header);
    bool Line2Grid(shapeFile &LineSf, gridHeader header);
    bool Point2Grid(shapeFile &PointSf, gridHeader header);
    bool Multipoint2Grid(shapeFile &MultipointSf, gridHeader header);

    int NumPercentShapes(int percent, int nShps);
    bool isGridContainsShape(Shape shp,  gridHeader hdr);
    double FirstLineXY(double xy, double xymin,  double pxsize,  int fact);
    double Fix(double number);

    int Sign(int value);
    int Sign(double value);

    bool Interseca(QList<bord> bordi2, QList<TDvalue> &intersezioni,  double xls);
    bool SortArray(QList<TDvalue> &vettore);
    bool ScanLine2(QList<TDvalue> vettore, double xref, QList<TGridPixel> &pixels, gridHeader header);
    void LineBresenham(QList<TGridPixel> vertices,  QList<TGridPixel> &gridPixels);

    void uploadPx(int shapeID,TGridPixel pix);
    void uploadPxList(int shapeID,QList<TGridPixel> pxList);

    void PixListToCSV(TGridHeader GrdHd);
    QString getStrValue(int value);

signals:
    void reportProgress(int shapeID);
    void reportError(QString errorMsg);
    void processingShape(QString shape, int numShapes);

};

#endif//INCLUDED_SHAPETOGRID_H

