
#include "LineEdge.h"

LineEdge::LineEdge(QObject *parent, int xstart, int ystart, int xEnd, int yend) :
    QObject(parent)
{
    reset( xstart, ystart, xEnd, yend );
}

LineEdge::LineEdge(QObject *parent, TGridPixel startPoint, TGridPixel endPoint) :
    QObject(parent)
{
    reset(startPoint.col, startPoint.row, endPoint.col, endPoint.row);
}

int LineEdge::Sign(int value)
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

int LineEdge::Sign(double value)
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

void LineEdge::reset(int xstart, int ystart, int xEnd, int yend)
{
    dX = xEnd - xstart;
    dY = yend - ystart;
    m_xStart = xstart;
    m_yStart = ystart;
    m_xEnd = xEnd;
    m_yEnd = yend;
    initialize();
}

void LineEdge::initialize()
{
    adx = fabs( dX );
    ady = fabs( dY );
    sdx = Sign( dX );
    sdy = Sign( dY );
    if(  adx > ady )
    {
        pdx = sdx;
        pdy = 0;
        ddx = sdx;
        ddy = sdy;
        ef = ady;
        es = adx;}
    else
    {
        pdx = 0;
        pdy = sdy;
        ddx = sdx;
        ddy = sdy;
        ef = adx;
        es = ady;
    }
    m_index = 0;
    vcurX = m_xStart;
    vcurY = m_yStart;
    error = int(round(es *1.0/ 2.0 ) );
}

QString LineEdge::ToString()
{
    QString str;

    str = "[" + QString::number(m_xStart) + "," + QString::number(m_yStart) + "] ";
    str = str + "[" + QString::number(m_xEnd) + "," + QString::number(m_yEnd) + "]";

    return str;
}

bool LineEdge::NextPixel()
{   bool result;
    if(  m_index == es )
    {
        result = false;
    }
    else
    {
        error = error - ef;
        if(  error < 0 )
        {
            error = error + es;
            vcurX = vcurX + ddx;
            vcurY = vcurY + ddy;
        }
        else
        {
            vcurX = vcurY + pdx;
            vcurY = vcurY + pdy;
        }
        m_index = m_index + 1;
        result = true;
    }
    return result;
}


//END
