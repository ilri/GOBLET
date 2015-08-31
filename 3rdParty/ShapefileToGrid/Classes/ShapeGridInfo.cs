using System;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Text;

namespace jk.ShapefileToGrid
{
    /// <summary>
    /// this class contains information necessary for 
    /// processing the shapefile and creating the grid.
    /// </summary>
    class ShapeGridInfo
    {
        public ShapeGridInfo()
        {

        }
        
        public ShapeGridInfo(MapWindow.Interfaces.IMapWin IMapWin)
        {
            m_MapWin = IMapWin;
            shapeForm = new frmShapefile(m_MapWin);
        }

        #region PRIVATE MEMBERS
        private MapWindow.Interfaces.IMapWin m_MapWin;
        private frmShapefile shapeForm;

        private string m_ShpFileName = String.Empty;
        private string m_FieldName = String.Empty;
       
        private string m_GridFileName;
        private MapWinGIS.GridDataType m_GridDataType;
        private MapWinGIS.GridFileType m_GridFileType;
        private MapWinGIS.GridHeader m_GridHeader;

        private jk.ShapefileToGrid.frmProgress m_ProgressForm; //the form to show progress

        #endregion


        #region PROPERTIES
        public string ShpFileName
        {
            get { return m_ShpFileName; }
            set { m_ShpFileName = value; }
        }
        public string GridFileName
        {
            get { return m_GridFileName; }
            set { m_GridFileName = value; }
        }

        public string FieldName
        {
            get { return m_FieldName; }
            set { m_FieldName = value; }
        }
        public MapWinGIS.GridHeader GridHeader
        {
            get { return m_GridHeader; }
            set { m_GridHeader = value; }
        }
        public MapWinGIS.GridDataType GridDataType
        {
            get { return m_GridDataType; }
            set { m_GridDataType = value; }
        }
        public MapWinGIS.GridFileType GridFileType
        {
            get { return m_GridFileType; }
            set { m_GridFileType = value; }
        }

        #endregion

        #region PUBLIC METHODS
        
        /// <summary>
        /// shows the form for getting information about the shapefile
        /// and retrieves user input (field name, shapefile name)
        /// returns true if successful
        /// </summary>
        public bool GetShapeInformation()
        {
            if (shapeForm == null)
            {
                shapeForm = new frmShapefile(m_MapWin);
            }

            // show the shapefile dialog
            if (shapeForm.ShowDialog() == DialogResult.OK)
            {
                m_ShpFileName = shapeForm.ShapeFileName;
                m_FieldName = shapeForm.FieldName;
                return GetGridInformation();
            }
            else
            {
                return false;
            }
        }

        public bool GetGridInformation()
        {
        //now show the grid form
            bool result = false;
            frmGrid gridForm = new frmGrid(m_MapWin, m_ShpFileName);
            
            if (gridForm.ShowDialog() == DialogResult.OK)
            {
                m_GridHeader = gridForm.GridHeader;
                m_GridDataType = gridForm.GridDataType;
                m_GridFileName = gridForm.GridFileName;
                m_GridFileType = gridForm.GridFileType;
                result = true;
            }
            else 
            { 
                //the user clicked the BACK button - go back to 'select shapefile' dialog
                result = GetShapeInformation();
            }
            return result;          
        }

        /// <summary>
        /// This starts the actual "shapefile to grid" calculation.
        /// Displays a Progress form showing calculation progress and
        /// status.
        /// </summary>
        public void DoShapefileToGrid2()
        {
            m_ProgressForm = new frmProgress();
            m_ProgressForm.Show();
            
            Rasterizer ras = new Rasterizer();
            bool flg = false;
            flg = ras.ShapefileToGrid2(m_ShpFileName, m_GridFileName, m_GridFileType, m_GridDataType,
                m_FieldName, m_GridHeader, m_ProgressForm);
            m_ProgressForm.Close();
            if (flg == false)
            {
                MapWinUtility.Logger.Msg("ERROR in shapefile to grid calculation");
            }

            //add grid to current map
            Microsoft.VisualBasic.MsgBoxResult res;
            res = MapWinUtility.Logger.Msg("Add grid to the map?", Microsoft.VisualBasic.MsgBoxStyle.YesNo,"Grid calculation finished");

            if (res == Microsoft.VisualBasic.MsgBoxResult.Yes)
            {
                DisplayGrid(m_GridFileName);
            }

        }

        /// <summary>
        /// adds the new grid as a new layer to the map
        /// </summary>
        private void DisplayGrid(string grFileName)
        {
            MapWindow.Interfaces.Layer newLayer;
            MapWinGIS.GridColorScheme cS = new MapWinGIS.GridColorScheme();

            try
            {
                newLayer = m_MapWin.Layers.Add(grFileName);
                MapWinGIS.Grid gr = newLayer.GetGridObject;
                cS = (MapWinGIS.GridColorScheme)newLayer.ColoringScheme;
                int i;
                for (i = 0; i < cS.NumBreaks; ++i)
                {
                    cS.get_Break(i).ColoringType = MapWinGIS.ColoringType.Gradient;
                }

                m_MapWin.Layers.RebuildGridLayer(newLayer.Handle, gr, cS);
            }
            catch (Exception ex)
            {
                MapWinUtility.Logger.Msg("ERROR adding grid to the map. " + ex.Message);
            }
        }

        /// <summary>
        /// just shows a message of the user-input values (for debugging purpose)
        /// </summary>
        public void ShowInputValues()
        {
            MapWinUtility.Logger.Msg("shapefile name: " + ShpFileName +
                            "\nField name: " + FieldName +
                            "\nGrid cols: " + GridHeader.NumberCols.ToString() +
                            "\nGrid rows: " + GridHeader.NumberRows.ToString() +
                            "\nGrid xMin: " + (GridHeader.XllCenter - GridHeader.dX / 2).ToString() +
                            "\nGrid yMin: " + (GridHeader.YllCenter - GridHeader.dY / 2).ToString() +
                            "\nGrid cellsize: " + GridHeader.dX.ToString() +
                            "\nGrid Nodata value: " + GridHeader.NodataValue.ToString() +
                            "\nGrid file name: " + GridFileName +
                            "\nGrid data type: " + GridDataType +
                            "\nGrid file type: " + GridFileType,
                            "Input values for 'Shapefile to Grid' function");
        }

        #endregion
    }
}
