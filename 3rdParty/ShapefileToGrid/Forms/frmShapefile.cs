using System;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace jk.ShapefileToGrid
{
    public partial class frmShapefile : Form
    {
        //private global members
        private MapWindow.Interfaces.IMapWin m_MapWin;
        private string m_ShpFileName;
        private object[] m_shpParams = new object[4];
        
        
        /// <summary>
        /// creates a new form
        /// </summary>
        /// <param name="IMapWin">reference to the current MapWindow
        /// GIS application (implements IMapWin interface)</param>
        public frmShapefile(MapWindow.Interfaces.IMapWin IMapWin)
        {
            //called by the winforms designer
            InitializeComponent();

            //set global variables and initial appearance
            m_MapWin = IMapWin;

            populateLayers();
        }

        #region PUBLIC PROPERTIES
        //public properties of this dialog visible to main application
        public string ShapeFileName
        {
            get
            {
                return m_ShpFileName;
            }
            set 
            {
                m_ShpFileName = value;
                populateLayers();
            }
        }

        public string FieldName
        {
            get 
            {
                return cmbFields.Text;
            }
            set
            {
                m_ShpFileName = value;
                populateFields(m_ShpFileName);
            }
        }
        #endregion

        
        // decide if to use the map layer or an external file
        private void rdUseExternal_CheckedChanged(object sender, EventArgs e)
        {
            if (rdUseExternal.Checked)
            {
                btnOpenFile.Enabled = true;
                cmbLayers.Enabled = false;

                if (lblFileName.Text != "")
                {       
                    populateFields(lblFileName.Text);
                }
            }
            else
            {
                btnOpenFile.Enabled = false;
                cmbLayers.Enabled = true;
                MapWinGIS.Shapefile shp = findShp(cmbLayers.Text);
                if (shp != null)
                {
                    populateFields();
                }
                else
                {
                    cmbLayers.Items.Clear();
                }
            }
        }

        // open a shapefile and populate the list of shapefile fields
        private void btnOpenFile_Click(object sender, EventArgs e)
        {
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            
            OpenFileDialog ofdialog = new OpenFileDialog();        
            ofdialog.Filter = sf.CdlgFilter;
            ofdialog.Title = "Choose a shapefile";
            ofdialog.ShowDialog(this);
            if (ofdialog.FileName == "") return;
           
            lblFileName.Text = ofdialog.FileName;
            m_ShpFileName = ofdialog.FileName;
            //create the list of fields and add them to combobox
            populateFields(m_ShpFileName);
        }


        // populates the list of fields
        private void cmbLayers_SelectedIndexChanged(object sender, EventArgs e)
        {
            MapWinGIS.Shapefile shp = findShp(cmbLayers.Text);
            if (shp != null)
            {
                populateFields();
            }
            else 
            {
                cmbLayers.Items.Clear();
            }
        }

        //the "next" button - collect user input and continue
        //to the grid form
        private void btnNext_Click(object sender, EventArgs e)
        {
            //MessageBox.Show("You entered values: " +
            //    "\n" + "Name of shapefile: " + m_ShpFileName +
            //    "\n" + "Name of field: " + cmbFields.Text +     
            //frmGrid gridForm = new frmGrid(m_MapWin, m_ShpFileName, cmbFields.Text, rdUseNewValue.Checked, 0);
            //gridForm.Show();
            this.Close();
        }

        #region private functions
        //auxiliary functions, used by the form

        /// <summary>
        /// populates the combo box with the layer names
        /// </summary>
        private void populateLayers()
        {
            // check if there are any layers present
            MapWindow.Interfaces.Layer curLayer;
           
            cmbLayers.Items.Clear();

            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    // check if it's a shapefile layer
                    if (curLayer.LayerType.ToString().ToLower().IndexOf("shapefile") >= 0)
                    {
                        cmbLayers.Items.Add(curLayer.Name);
                        if (m_MapWin.Layers.CurrentLayer == curLayer.Handle)
                        {
                            cmbLayers.SelectedIndex = cmbLayers.Items.Count - 1;
                        }
                    }
                }
            }
            // selected index in case of no layer active in the legend
            // also store the filename of underlying shapefile
            if (cmbLayers.Items.Count > 0)
            {
                cmbLayers.Enabled = true;
                if (cmbLayers.SelectedIndex < 0)
                {
                    cmbLayers.SelectedIndex = 0;
                }
            }
            else 
            {
                rdUseExternal.Checked = true;
            }
            
        }


        private void populateFields(string shpFileName)
        {
            cmbFields.Items.Clear();
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            MapWinGIS.Field curField;
            if (sf.Open(shpFileName,sf.GlobalCallback))
            {
                for (int i = 0; i < sf.NumFields; ++i)
                {
                    curField = sf.get_Field(i);
                    if (curField.Type == MapWinGIS.FieldType.DOUBLE_FIELD ||
                        curField.Type == MapWinGIS.FieldType.INTEGER_FIELD)
                    {
                        cmbFields.Items.Add(curField.Name);
                    }
                }
                sf.Close();
            }
            if (cmbFields.Items.Count > 0)
            {
                cmbFields.SelectedIndex = 0;
            }
        }

        //returns a shapefile object corresponding to the map layer name
        private MapWinGIS.Shapefile findShp(string layerName)
        {
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            MapWindow.Interfaces.Layer curLayer;
            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                curLayer = m_MapWin.Layers[i];
                if (curLayer != null)
                {
                    if (curLayer.Name == cmbLayers.Text)
                    {
                        sf = (MapWinGIS.Shapefile)curLayer.GetObject();
                        break;
                    }
                }
            }
            return sf;
        }

        private void populateFields()
        {
            MapWinGIS.Field curField;
            MapWinGIS.Shapefile sf = new MapWinGIS.Shapefile();
            MapWindow.Interfaces.Layer curLayer;
            cmbFields.Items.Clear();

            for (int i = 0; i < m_MapWin.Layers.NumLayers; ++i)
            {
                curLayer = m_MapWin.Layers[i];
                //the layer type is shapefile (ensured by PopulateLayers)
                if (curLayer != null)
                {
                    if (curLayer.Name == cmbLayers.Text)
                    {
                        sf = (MapWinGIS.Shapefile)curLayer.GetObject();
                        break;
                    }
                }
            }

            //add the fields in the shapefile to the list of fields
            //for simplification, only numeric fields can be added
            //because the resulting grid can only contain numeric values

            for (int i = 0; i < sf.NumFields; ++i)
            {
                curField = sf.get_Field(i);
                if (curField.Type == MapWinGIS.FieldType.DOUBLE_FIELD ||
                    curField.Type == MapWinGIS.FieldType.INTEGER_FIELD)
                {
                    cmbFields.Items.Add(curField.Name);
                }
            }
            if (cmbFields.Items.Count > 0)
            {
                cmbFields.SelectedIndex = 0;
                m_ShpFileName = sf.Filename;
            }
        }



        #endregion

        private void frmShapefile_Load(object sender, EventArgs e)
        {
            populateLayers();
        }
    }
}