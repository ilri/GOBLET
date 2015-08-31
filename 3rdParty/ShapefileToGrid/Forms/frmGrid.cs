using System;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace jk.ShapefileToGrid
{
    public partial class frmGrid : Form
    {
        /// <summary>
        /// Creates a new form object for setting grid properties
        /// (field type will be "double" data type)
        /// </summary>
        /// <param name="IMapWin"></param>
        /// <param name="shpFileName"></param>
        public frmGrid(MapWindow.Interfaces.IMapWin IMapWin,
            string shpFileName) : this(IMapWin, shpFileName, MapWinGIS.FieldType.DOUBLE_FIELD)
        {
            //frmGrid(IMapWin, shpFileName, MapWinGIS.FieldType.DOUBLE_FIELD);
        }
          
        /// <summary>
        /// Creates a new form object for setting properties of the
        /// new grid
        /// </summary>
        /// <param name="IMapWin">The MapWindow interface</param>
        /// <param name="shpFileName">File name of the shapefile</param>
        /// <param name="fieldType">Type of field containing data</param>
        public frmGrid(MapWindow.Interfaces.IMapWin IMapWin,
            string shpFileName, MapWinGIS.FieldType fieldType)
        {
            
            InitializeComponent();

            //setup event handler
            this.txtCellSize.TextChanged += new EventHandler(txtCellSize_TextChanged);
            
            //initialize the global variables and default values
            m_MapWin = IMapWin;
            m_ShpFileName = shpFileName;
            m_FieldType = fieldType;
            m_DefaultCellSize = calcDefaultCellsize();
            m_DefaultNodataValue = -9999F;
            m_GridHeader = new MapWinGIS.GridHeader();
            
            //populate the combo boxes
            populateGridList();
            populateExtentLayers();
            populateDataTypes();
            populateFileTypes();
        }

        #region PRIVATE GLOBAL VARIABLES
        
        private MapWindow.Interfaces.IMapWin m_MapWin;

        private MapWinGIS.GridHeader m_GridHeader;

        private string m_ShpFileName;
        private MapWinGIS.FieldType m_FieldType; //type of shapefile field
        private string m_ShortName; //short name of shapefile

        //default values of cellsize and extent
        private double m_DefaultCellSize;
        private double m_DefaultNodataValue;

        //messages to display, TODO: use ResourceManager
        private string msgSameAs = "same as ";
        private string msgSpecifiedBelow = "as specified below.. ";

        #endregion

        #region PUBLIC PROPERTIES
        public MapWinGIS.GridHeader GridHeader
        {
            get 
            { 
                return m_GridHeader; 
            }
        }
        public MapWinGIS.GridDataType GridDataType
        {
            get { return (MapWinGIS.GridDataType)cmbDataType.SelectedItem; }
        }
        public string GridFileName
        {
            get { return txtGridFile.Text; }
        }
        public MapWinGIS.GridFileType GridFileType
        {
            get { return (MapWinGIS.GridFileType)cmbFileType.SelectedItem; }
        }

        #endregion

        #region PRIVATE FUNCTIONS

        /// <summary>
        /// populates the list of grid layers to supply cellsize value
        /// is selected
        /// </summary>
        private void populateGridList()
        {
            MapWindow.Interfaces.Layer curLayer;
            
            cmbCellSize.Items.Clear();
            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    // check if it's a grid layer
                    if (curLayer.LayerType == MapWindow.Interfaces.eLayerType.Grid)
                    {
                        cmbCellSize.Items.Add(msgSameAs + curLayer.Name);
                        if (m_MapWin.Layers.CurrentLayer == curLayer.Handle)
                        {
                            cmbCellSize.SelectedIndex = cmbCellSize.Items.Count - 1;
                        }
                    }
                }
            }
            cmbCellSize.Items.Add(msgSpecifiedBelow); //add "as specified below" option

            // selected index in case of no layer active in the legend
            if (cmbCellSize.Items.Count > 0)
            {
                if (cmbCellSize.SelectedIndex < 0)
                {
                    cmbCellSize.SelectedIndex = 0;
                }
            }
        }
        
        
        
        /// <summary>
        /// adds the names of all layers in the current map to
        /// the "extent" combobox and chooses one if it is the same
        /// as the shapefile chosen in previous dialog
        /// </summary>
        private void populateExtentLayers()
        {
            // check if there are any layers present
            MapWindow.Interfaces.Layer curLayer;
            string shortFileName = m_ShpFileName.Substring(m_ShpFileName.LastIndexOf(@"\") + 1);
            cmbExtent.Items.Clear();

            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    // check if it's the same layer as the processed shapefile        
                    cmbExtent.Items.Add(msgSameAs + curLayer.Name);
                    if (curLayer.FileName == m_ShpFileName)
                    {
                        cmbExtent.SelectedIndex = cmbExtent.Items.Count - 1;
                    }
                }
            }
            
            // in other case, add the shapefile name to list
            if (cmbExtent.SelectedIndex < 0)
            {
                cmbExtent.Items.Add(msgSameAs + shortFileName);
                for (int i = 0; i < cmbExtent.Items.Count; ++i)
                {
                    if (cmbExtent.Items[i].ToString() == msgSameAs + shortFileName)
                    {
                        m_ShortName = shortFileName;
                        cmbExtent.SelectedIndex = i;
                        break;
                    }
                }
               
            }

            //always add "as specified below.." option
            cmbExtent.Items.Add(msgSpecifiedBelow); //add "as specified below" option

            // selected index in case of no layer active in the legend
            if (cmbExtent.Items.Count > 0)
            {
                if (cmbExtent.SelectedIndex < 0)
                {
                    cmbExtent.SelectedIndex = 0;
                }
            }
        }

        /// <summary>
        /// initializes the list of available grid data types
        /// </summary>
        private void populateDataTypes()
        {
            MapWinGIS.GridDataType grDataType = calcGridDataType(m_FieldType);
            cmbDataType.Items.Add(MapWinGIS.GridDataType.ShortDataType);
            cmbDataType.Items.Add(MapWinGIS.GridDataType.LongDataType);
            cmbDataType.Items.Add(MapWinGIS.GridDataType.FloatDataType);
            cmbDataType.Items.Add(MapWinGIS.GridDataType.DoubleDataType);
            cmbDataType.SelectedItem = grDataType;
        }

        /// <summary>
        /// initializes the list of grid file types suitable for writing
        /// </summary>
        private void populateFileTypes()
        {
            cmbFileType.Items.Add(MapWinGIS.GridFileType.Ascii);
            cmbFileType.Items.Add(MapWinGIS.GridFileType.Binary);
            cmbFileType.Items.Add(MapWinGIS.GridFileType.GeoTiff);
            cmbFileType.SelectedItem = MapWinGIS.GridFileType.GeoTiff;
        }

        // updates values in the "grid extent" box
        // (min. X, min. Y, max. X, max. Y, columns, rows)
        /// <summary>
        /// updates values in the "grid extent" box
        /// (min. X, min. Y, max. X, max. Y, num.columns, num.rows.
        /// The extent values are rounded to nearest whole cellsize
        /// </summary>
        /// <param name="extents">the extents of new grid</param>
        /// <param name="roundValues">set to TRUE if the xMin and yMin values
        /// should be rounded to whole cell size.</param>
        private void UpdateExtentBox(MapWinGIS.Extents extents)
        {
            txtMinX.Text = extents.xMin.ToString();
            txtMinY.Text = extents.yMin.ToString();
            txtMaxX.Text = extents.xMax.ToString();
            txtMaxY.Text = extents.yMax.ToString();

            UpdateExtentBox();
        }

        /// <summary>
        /// Overloaded version of the function
        /// Sets the "grid extent" box to the same extention as the grid header
        /// </summary>
        /// <param name="header"></param>
        private void UpdateExtentBox(MapWinGIS.GridHeader header)
        {
            double xMin = header.XllCenter - (header.dX / 2.0);
            double yMin = header.YllCenter - (header.dY / 2.0);
            double nCols = header.NumberCols;
            double nRows = header.NumberRows;
            double xMax = xMin + header.dX * nCols;
            double yMax = yMin + header.dY * nRows;

            txtMinX.Text = xMin.ToString();
            txtMinY.Text = yMin.ToString();
            txtMaxX.Text = xMax.ToString();
            txtMaxY.Text = yMax.ToString();
            lblNumberCols.Text = nCols.ToString();
            lblNumberRows.Text = nRows.ToString();
        }

        // updates values in the "grid extent" box when the 
        // cell size box is changed and "grid extent box" is not
        // "as specified below"
        private void UpdateExtentBox()
        {
            double cellSize;
            double xMin, yMin, xMax, yMax;
            double nCols, nRows;
           
            if ( !Utils.string2double(txtCellSize.Text, out cellSize) ) return;
            if ( !Utils.string2double(txtMinX.Text, out xMin) ) return;
            if ( !Utils.string2double(txtMinY.Text, out yMin) ) return;
            if ( !Utils.string2double(txtMaxX.Text, out xMax) ) return;
            if ( !Utils.string2double(txtMaxY.Text, out yMax) ) return;

            xMin = ((int)(xMin / cellSize)) * cellSize;
            yMin = ((int)(yMin / cellSize)) * cellSize; 
            nCols = ((int)((xMax - xMin) / cellSize));
            nRows = ((int)((yMax - yMin) / cellSize));
            if ((xMax - xMin) % cellSize > 0)
            {
                nCols = nCols + 1;
            }
            if ((yMax - yMin) % cellSize > 0)
            {
                nRows = nRows + 1;
            }
            xMax = xMin + nCols * cellSize;
            yMax = yMin + nRows * cellSize;

            txtMinX.Text = xMin.ToString();
            txtMinY.Text = yMin.ToString();
            txtMaxX.Text = xMax.ToString();
            txtMaxY.Text = yMax.ToString();
            lblNumberCols.Text = nCols.ToString();
            lblNumberRows.Text = nRows.ToString();
        }

        // updates the internal Grid header object which stores
        // the extent and cellsize of the new grid
        private void UpdateGridHeader(double xMin, double yMin, double xMax, double yMax, 
            double dX, double dY, MapWinGIS.GridHeader hdr)
        {
            if (hdr == null)
            {
                hdr = new MapWinGIS.GridHeader();
            }

            double noData = m_DefaultNodataValue;
            double halfX = dX / 2F;
            double halfY = dY / 2F;
            double xllCenter = xMin + halfX;
            double yllCenter = yMin + halfX;
            int nCols = (int) ( Math.Abs( xMax - xMin ) / dX );
            int nRows = (int) ( Math.Abs( yMax - yMin ) / dY );
            
            hdr.dX = dX;
            hdr.dY = dY; 
            hdr.NumberCols = nCols;
            hdr.NumberRows = nRows;
            hdr.XllCenter = xllCenter;
            hdr.YllCenter = yllCenter;
            hdr.NodataValue = noData;
        }

        // calculates extent (bounding box) of shapefile layer -
        // the extent is slightly larger than original shapefile extent:
        // values are rounded to whole pixels
        private MapWinGIS.Extents calcShapefileExtent(MapWinGIS.Shapefile shp)
        {
            double cellSize = m_DefaultCellSize;
            
            Utils.string2double(txtCellSize.Text, out cellSize);

            MapWinGIS.Extents ext = shp.Extents;
            
            double xMin = ((int)(ext.xMin / cellSize)) * cellSize;
            double yMin = ((int)(ext.yMin / cellSize)) * cellSize;

            int nCols = ((int)( (ext.xMax - ext.xMin) / cellSize )) + 1;
            lblNumberCols.Text = nCols.ToString();

            int nRows = ((int)( (ext.yMax - ext.yMin) / cellSize )) + 1;
            lblNumberRows.Text = nRows.ToString();

            double xMax = xMin + nCols * cellSize;
            double yMax = yMin + nRows * cellSize;

            MapWinGIS.Extents newExtents = new MapWinGIS.Extents();
            newExtents.SetBounds(xMin, yMin, 0, xMax, yMax, 0);
            return newExtents;
        }

        // calculate a default cellsize (grid-max.500 rows or columns)
        // using the input shapefile extent
        private double calcDefaultCellsize()
        {
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            double result = 0F;
            if (sf.Open(m_ShpFileName, null))
            {
                double sfX = sf.Extents.xMax - sf.Extents.xMin;
                double sfY = sf.Extents.yMax - sf.Extents.yMin;
                double minExt = Math.Max(sfX, sfY);
                result = Math.Floor(minExt / 250F);
            }
            sf.Close();
            return result;
        }

        // gets the corresponding grid data type from shapefile field data type
        private MapWinGIS.GridDataType calcGridDataType(MapWinGIS.FieldType fieldType)
        {
            switch (fieldType)
            {
                case MapWinGIS.FieldType.DOUBLE_FIELD:
                    return MapWinGIS.GridDataType.DoubleDataType;
                case MapWinGIS.FieldType.INTEGER_FIELD:
                    return MapWinGIS.GridDataType.LongDataType;
                default:
                    return MapWinGIS.GridDataType.FloatDataType;
            }
        }

        //returns the filter used for opening grid in the file dialog
        private string calcDialogFilter(MapWinGIS.GridFileType grType)
        {
            switch (grType)
            {
                case MapWinGIS.GridFileType.Ascii:
                    return "Ascii text (*.asc)|*.asc;*.txt";
                case MapWinGIS.GridFileType.Binary:
                    return "USU Binary (*.bgd)|*.bgd";
                case MapWinGIS.GridFileType.GeoTiff:
                    return "GeoTiff (*.tif)|*.tif";
                default:
                    return "Ascii text (*.asc)|*.asc;*.txt";
            }
        }

        //returns the appropriate grid extension
        private string calcGridExtension(MapWinGIS.GridFileType grType)
        {
            switch (grType)
            {
                case MapWinGIS.GridFileType.Ascii:
                    return ".asc";
                case MapWinGIS.GridFileType.Binary:
                    return ".bgd";
                case MapWinGIS.GridFileType.GeoTiff:
                    return ".tif";
                default:
                    return ".asc";
            }
        }

        #endregion

        #region EVENT HANDLERS

        /// <summary>
        /// sets value of the cellsize textbox according to the
        /// selected grid layer
        /// </summary>
        private void cmbCellSize_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbCellSize.Items.Count == 0) return;

            double cellSize = 0;
            MapWindow.Interfaces.Layer curLayer;

            // if the user choses "as specified below", use the value in textbox
            if (cmbCellSize.Text == msgSpecifiedBelow)
            {
                if (!Utils.string2double(txtCellSize.Text, out cellSize))
                {
                    cellSize = m_DefaultCellSize;
                    txtCellSize.Text = cellSize.ToString();
                }
                txtCellSize.Enabled = true;
                txtCellSize.Focus();
                return;
            }
            else
            {
                txtCellSize.Enabled = false;
            }

            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                //the layer type for choosing cellsize is always Grid (ensured by PopulateLayers)
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    if (curLayer.Name == cmbCellSize.Text.Substring(msgSameAs.Length))
                    {
                        if (curLayer.LayerType == MapWindow.Interfaces.eLayerType.Grid)
                        {
                            cellSize = curLayer.GetGridObject.Header.dX;
                            txtCellSize.Text = cellSize.ToString();
                            break;
                        }
                    }
                }
            }
            UpdateExtentBox(); //update the 'extent' box
        }


        // displays the grid extent!
        private void cmbExtent_SelectedIndexChanged(object sender, EventArgs e)
        {
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            MapWindow.Interfaces.Layer curLayer;
            MapWinGIS.Grid grd = new MapWinGIS.Grid();
            MapWinGIS.Extents extents;
            double cellSize = m_DefaultCellSize;

            //get the cell size to value from textbox
            Utils.string2double(txtCellSize.Text, out cellSize);

            //handle the "as specified below" option
            if (cmbExtent.Text == msgSpecifiedBelow)
            {
                txtMinX.Enabled = true;
                txtMinY.Enabled = true;
                txtMaxX.Enabled = true;
                txtMaxY.Enabled = true;

                return;
            }
            else
            {
                txtMinX.Enabled = false;
                txtMinY.Enabled = false;
                txtMaxX.Enabled = false;
                txtMaxY.Enabled = false;
            }

            //handle the case when shapefile loaded from disk and selected
            if (cmbExtent.Text == msgSameAs + m_ShortName)
            {
                if (sf.Open(m_ShpFileName,null))
                {
                    extents = calcShapefileExtent(sf);
                    UpdateExtentBox(extents);
                }
                sf.Close();
            }

            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                //an existing layer is used to determine the extent
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    if (msgSameAs + curLayer.Name == cmbExtent.Text)
                    {
                        switch (curLayer.LayerType)
                        {
                            //same extention as an existing grid
                            case MapWindow.Interfaces.eLayerType.Grid:
                                grd = curLayer.GetGridObject;
                                UpdateExtentBox(grd.Header);
                                break;

                            //same extention as an existing shapefile
                            case MapWindow.Interfaces.eLayerType.LineShapefile:
                            case MapWindow.Interfaces.eLayerType.PolygonShapefile:
                            case MapWindow.Interfaces.eLayerType.PointShapefile:
                                sf = (MapWinGIS.Shapefile)curLayer.GetObject();
                                extents = calcShapefileExtent(sf);
                                UpdateExtentBox(extents);
                                break;

                            default:
                                break;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// set the grid file type (must be one of types supported for writing)
        /// </summary>
        private void cmbFileType_SelectedIndexChanged(object sender, EventArgs e)
        {
            MapWinGIS.GridFileType grFileType = this.GridFileType;
            string ext = calcGridExtension(grFileType);
            string grFileName = txtGridFile.Text;

            if (MapWinUtility.Strings.IsEmpty(grFileName))
            {
                grFileName = MapWinUtility.modFile.FilenameNoExt(m_ShpFileName) + "_grid" + ext;
            }
            else
            {
                grFileName = MapWinUtility.modFile.FilenameNoExt(grFileName) + ext;
            }
            
            txtGridFile.Text = grFileName;

            //for .bgd grid, ensure "float" data type is selected
            if (grFileType == MapWinGIS.GridFileType.Binary)
            {
                cmbDataType.SelectedItem = MapWinGIS.GridDataType.FloatDataType;
            }
        }

        private void frmGrid_Load(object sender, EventArgs e)
        {
            
        }

        //select the grid file name:
        private void btnOpenFile_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfdialog = new SaveFileDialog();
            sfdialog.OverwritePrompt = true;
            MapWinGIS.GridFileType grFileType = this.GridFileType;

            if (txtGridFile.Text.Length > 0)
            {
                sfdialog.FileName = MapWinUtility.modFile.FilenameNoExt(m_ShpFileName) + 
                    "_grid" + calcGridExtension(grFileType);
            }

            sfdialog.Filter = calcDialogFilter(grFileType);

            if (sfdialog.ShowDialog(this) == DialogResult.OK)
            {
                if (MapWinUtility.Strings.IsEmpty(sfdialog.FileName))
                {
                    return;
                }
                else
                {
                    txtGridFile.Text = sfdialog.FileName;
                }
            }
        }

        // check user input and set the values of private fields
        // prompts the user to fill in any missing data
        private void btnNext_Click(object sender, EventArgs e)
        {
            MapWinGIS.GridHeader hdr = new MapWinGIS.GridHeader();
            double cellSize = 0F;
            double minX = 0F;
            double minY = 0F;
            double maxX = 0F;
            double maxY = 0F;

            Utils.string2double(txtCellSize.Text, out cellSize);
            if (cellSize <= 0)
            {
                MapWinUtility.Logger.Msg("The grid cell size must be a positive number.");
                txtCellSize.Focus();
                return;
            }
            if (!Utils.string2double(txtMinX.Text, out minX))
            {
                MapWinUtility.Logger.Msg("The minimum X value must be a number.");
                txtMinX.Focus();
                return;
            }
            if (!Utils.string2double(txtMinY.Text, out minY))
            {
                MapWinUtility.Logger.Msg("The minimum Y value must be a number.");
                txtMinY.Focus();
                return;
            }
            if (!Utils.string2double(txtMaxX.Text, out maxX))
            {
                MapWinUtility.Logger.Msg("The maximum X value must be a number");
                txtMaxX.Focus();
                return;
            }
            
            if (!Utils.string2double(txtMaxY.Text, out maxY))
            {
                MapWinUtility.Logger.Msg("The number of rows must be a positive integer.");
                txtMaxY.Focus();
                return;
            }

            if (MapWinUtility.Strings.IsEmpty(txtGridFile.Text))
            {
                MapWinUtility.Logger.Msg("Please select the result grid filename.");
                btnOpenFile.Focus();
                return;
            }

            //check data type - .bgd format only supports float data type
            if (this.GridDataType != MapWinGIS.GridDataType.FloatDataType &&
                this.GridFileType == MapWinGIS.GridFileType.Binary)
            {
                cmbDataType.SelectedItem = MapWinGIS.GridDataType.FloatDataType;
            }

            //update the "grid header" object with values from textboxes
            UpdateGridHeader(minX, minY, maxX, maxY, cellSize, cellSize, m_GridHeader);
        }

        #endregion


        //updates the number of columns / rows value
        private void txtCellSize_TextChanged(object sender, EventArgs e)
        {
            if (txtCellSize.Enabled == true && txtCellSize.Text != "")
            {
                UpdateExtentBox();
            }
        }
    }  
}