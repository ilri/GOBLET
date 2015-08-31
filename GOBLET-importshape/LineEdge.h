#ifndef INCLUDED_LINEEDGE_H
#define INCLUDED_LINEEDGE_H

 //********************************************************************************************************
//FileName: shapetoGrid.pas
//This is the Delphi implementation of the C# class Rasterizer.cs by Jiri Kadlec.
//Authors
//Enrico Chiaradia ( created rasterize script - polygon to grid using scanline algorithm in vb.net )
//Jiri Kadlec      ( rewritten code in c#, added line to grid function using Bresenham algorithm and created user interface)
//Carlos Quiros    ( rewritten code in QT)

    // LineEdge represents a polyline edge to be rasterized using
    // Bresenham's line algorithm.
    // The edge is a straight line joining two points.
    // after initialization, you can repeatedly call NextPixel()
    // and get the pixels on the line.
    // LineEdge uses the Bresenham line algorithm.
    //
    // ASSUMPTIONS OF THIS CLASS:
    // (1) the starting and ending point x, y coordinates
    //     have already been converted to grid coordinates
    //     for example, using ProjToPixel() MW function
    //     and rounded to integers
    //     the GRID COORDINATES are defined:
    //     (a) both axis start in the centre of the cell in
    //         top left corner of the grid
    //     (b) values on the (x) axis increase in downward direction
    //     (c) values on the (y) axis increase from left to right
    // (2) the grid pixel size is 1 * 1 grid coordinate system
    //     units.



#include <math.h>
#include <QtCore>

struct GridPixel
{
  int shapeID;
  int col;
  int row;
};
typedef GridPixel TGridPixel;



//struct/*class*/ TLineEdge: public TObject
class LineEdge : public QObject
{
    Q_OBJECT
public:
    LineEdge(QObject *parent = 0, int xstart = 0, int ystart = 0, int xEnd = 0, int yend = 0);
    LineEdge(QObject *parent = 0, TGridPixel startPoint = TGridPixel(), TGridPixel endPoint = TGridPixel());

    // /*?*/static/*CONSTRUCTOR*/void Create(int xstart, int ystart, int xEnd, int yend)/*overload*/;
    // /*?*/static/*CONSTRUCTOR*/void Create(PGridPixel startPoint, PGridPixel endPoint)/*overload*/;

    void reset(int xstart, int ystart, int xEnd, int yend);
    QString ToString();
    bool NextPixel();

    int getNumPixels(){ return es; }
    int getCurX(){ return vcurX; }
    int getCurY(){ return vcurY; }

private:
    int dX; int dY; int adx; int ady; int sdx; int sdy; int pdx;
    int pdy; int ddx; int ddy; int ef; int es; int error;
    int m_xStart; int m_yStart; int m_xEnd; int m_yEnd;
    int vcurX; int vcurY;
    int m_index;
    void initialize();

    int Sign(int value);
    int Sign(double value);

};




#endif//INCLUDED_LINEEDGE_H
//END
