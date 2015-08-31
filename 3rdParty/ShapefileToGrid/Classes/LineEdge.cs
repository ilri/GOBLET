using System;
using System.Collections;
using System.Text;


namespace jk.ShapefileToGrid
{
    /// <summary>
    /// LineEdge represents a polyline edge to be rasterized using
    /// Bresenham's line algorithm. 
    /// The edge is a straight line joining two points.
    /// after initialization, you can repeatedly call NextPixel()
    /// and get the pixels on the line.
    /// LineEdge uses the Bresenham line algorithm.
    /// 
    /// ASSUMPTIONS OF THIS CLASS:
    /// (1) the starting and ending point x, y coordinates
    ///     have already been converted to grid coordinates 
    ///     for example, using ProjToPixel() MW function
    ///     and rounded to integers
    ///     the GRID COORDINATES are defined:
    ///     (a) both axis start in the centre of the cell in 
    ///         top left corner of the grid
    ///     (b) values on the (x) axis increase in downward direction
    ///     (c) values on the (y) axis increase from left to right
    /// (2) the grid pixel size is 1 * 1 grid coordinate system 
    ///     units.
    /// </summary>
    class LineEdge
    {
        #region LineEdge Constructor

        /// <summary>
        /// Creates a new LineEdge instance joining the points
        /// [xStart,yStart] and [xEnd, yEnd]
        /// </summary>
        /// <param name="xStart">x coordinate of starting point</param>
        /// <param name="yStart">y coordinate of starting point</param>
        /// <param name="xEnd">x coordinate of ending point</param>
        /// <param name="yEnd">y coordinate of ending point</param>
        public LineEdge(int xStart, int yStart, int xEnd, int yEnd)
        {
            this.Reset(xStart,yStart,xEnd,yEnd);
        }

        public LineEdge(GridPixel startPoint, GridPixel endPoint)
        {
            this.Reset(startPoint.col, startPoint.row, endPoint.col, endPoint.row);
        }

        public void Reset(int xStart, int yStart, int xEnd, int yEnd)
        {
            // initialize private class members
            dx = xEnd - xStart;
            dy = yEnd - yStart;

            // other initializations
            m_xStart = xStart;
            m_yStart = yStart;
            m_xEnd = xEnd;
            m_yEnd = yEnd;

            this.initialize();
        }

        private void initialize()
        {
            //initializations
            adx = Math.Abs(dx);
            ady = Math.Abs(dy);
            sdx = Math.Sign(dx);
            sdy = Math.Sign(dy);

            if (adx > ady)
            {
                // x is the fast direction
                pdx = sdx;
                pdy = 0;
                ddx = sdx;
                ddy = sdy;
                ef = ady;
                es = adx;
            }
            else
            {
                // y is the fast direction
                pdx = 0;
                pdy = sdy;
                ddx = sdx;
                ddy = sdy;
                ef = adx;
                es = ady;
            }

            m_index = 0; //set to -1 to ensure writing of first pixel
            curX = m_xStart;
            curY = m_yStart;
            error = (int)(Math.Round(es / 2.0));
        }

        #endregion


        #region Public methods and properties

        /// <summary>
        /// returns the total number of pixels on
        /// the rasterized line edge
        /// </summary>
        public int NumPixels
        {
            get { return es; }
        }

        public int CurX
        {
            get { { return curX; } }
        }
        public int CurY
        {
            get { { return curY; } }
        }

        public override string ToString()
        {
            return string.Format("[{0},{1}] [{2},{3}]", m_xStart, m_yStart, m_xEnd, m_yEnd);
        }

        /// <summary>
        /// implementation of the main Bresenham line algorithm loop
        /// always make one step in fast direction
        /// if necessary, also take a step in slow direction
        /// based on the Wikipedia, 
        /// url[http://en.wikipedia.org/wiki/Bresenham's_line_algorithm]
        /// </summary>
        /// <returns>false if the loop had been previously finished,
        /// otherwise true</returns>
        public bool NextPixel()
        {
            if (m_index >= es)
            {
                return false;
            }

            //update error term
            error = error - ef;
            if (error < 0)
            {
                //make error term positive (>=0) again
                error = error + es;

                //step in both slow and fast direction (diagonal step)
                curX += ddx;
                curY += ddy;
            }
            else
            {
                //step in fast direction (parallel step)
                curX += pdx;
                curY += pdy;
            }

            ++m_index;
            return true;
        }

        #endregion


        #region Private members
        int dx;   //distance in x direction
        int dy;   //distance in y direction
        int adx;  //absolute value of distance in x direction
        int ady;  //absolute value of distance in y direction
        int sdx;  //signum of x distance (1 for (+), 0 for (0), -1 for (-))
        int sdy;  //signum of y distance (1 for (+), 0 for (0), -1 for (-))
        int pdx, pdy; //pd is the parallel step
        int ddx, ddy; //dd is the diagonal step
        int ef; //error step (fast direction)  
        int es; //error step (slow direction)
        int error; //the error term

        int m_xStart, m_yStart, m_xEnd, m_yEnd;

        int curX, curY; //current pixel x and y coordinates
        int m_index; //index to iterate through the pixels

        #endregion
    }
}

