using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace jk.ShapefileToGrid
{
    public class ShapefileToGridPlugin : MapWindow.Interfaces.IPlugin
    {
        public MapWindow.Interfaces.IMapWin _mapWin;
        private const string _toolbarName = "ShapefileToGrid";
        private const string _btnNameShp2Grid = "Shapefile to grid";
        private const string _toolTipShp2Grid = "Convert a shapefile to grid";

        #region Implementation of IPlugin
        /// <summary>
        /// Called when the MapWindow loads a new project.
        /// </summary>
        /// <param name="ProjectFile">The filename of the project file being loaded.</param>
        /// <param name="SettingsString">The settings string that was saved in the project file for this plugin.</param>
        public void ProjectLoading(string ProjectFile, string SettingsString)
        {

        }

        /// <summary>
        /// Method used by plugins to communicate with other plugins.
        /// </summary>
        /// <param name="msg">The messsage being recieved.</param>
        /// <param name="Handled">Reference parameter.  Set thist to true if this plugin handles recieving the message.  When set to true, no other plugins will receive the message.</param>
        public void Message(string msg, ref bool Handled)
        {

        }

        /// <summary>
        /// Method that is called when the MapWindow extents change.
        /// </summary>
        public void MapExtentsChanged()
        {

        }

        /// <summary>
        /// This method is called when a plugin is unloaded.  The plugin should remove all toolbars, buttons and menus that it added.
        /// </summary>
        public void Terminate()
        {
            _mapWin.Toolbar.RemoveToolbar(_toolbarName);
        }

        /// <summary>
        /// Occurs when the user presses a mouse button on the MapWindow map display.
        /// </summary>
        /// <param name="Button">The integer representation of the button pressed by the user.  This parameter uses the vb6 mouse button constants (vbLeftButton, etc.).</param>
        /// <param name="Shift">The integer representation of which shift/alt/control keys are pressed. This parameter uses the vb6 shift constants.</param>
        /// <param name="x">X coordinate in pixels.</param>
        /// <param name="y">Y coordinate in pixels.</param>
        /// <param name="Handled">Reference parameter.  When set to true, no other plugins will receive this event.</param>
        public void MapMouseDown(int Button, int Shift, int x, int y, ref bool Handled)
        {
        }

        /// <summary>
        /// ProjectSaving is called when the MapWindow saves a project.  This is a good chance for the 
        /// plugin to save any custom settings and create a SettingsString to place in the project file.
        /// </summary>
        /// <param name="ProjectFile">The name of the project file being saved.</param>
        /// <param name="SettingsString">Reference parameter.  The settings string that will be saved in the project file for this plugin.</param>
        public void ProjectSaving(string ProjectFile, ref string SettingsString)
        {
        }

        /// <summary>
        /// Occurs when a user clicks on a toolbar button or menu item.
        /// </summary>
        /// <param name="ItemName">The name of the item clicked on.</param>
        /// <param name="Handled">Reference parameter.  Setting Handled to true prevents other plugins from receiving this event.</param>
        public void ItemClicked(string ItemName, ref bool Handled)
        {
            try
            {
                switch (ItemName)
                {
                    case _btnNameShp2Grid:
                        
                        Handled = true;

                        bool flg = false;
                        ShapeGridInfo convertInfo = new ShapeGridInfo(_mapWin);
                        flg = convertInfo.GetShapeInformation();

                        if (flg == true)
                        {
                            convertInfo.ShowInputValues();
                            //_mapWin.Toolbar["_toolbarName"].Enabled = false;
                            MessageBox.Show("Calculation will start!");
                            convertInfo.DoShapefileToGrid2();
                            //_mapWin.Toolbar["_toolbarName"].Enabled = true;
                        }

                        break;

                    case "":
                        break;

                    default:
                        break;
                }
            }
            catch (System.Exception ex)
            {
                //TODO
                _mapWin.ShowErrorDialog(ex);
            }
        }

        /// <summary>
        /// Called when 1 or more layers are added to the MapWindow.
        /// </summary>
        /// <param name="Layers">Array of layer objects containing references to all layers added.</param>
        public void LayersAdded(MapWindow.Interfaces.Layer[] Layers)
        {
            //MapWinUtility.Logger.Msg("layer added. NumLayers: " + _mapWin.Layers.NumLayers.ToString());
        }

        /// <summary>
        /// Called when a layer is selected or made to be the active layer.
        /// </summary>
        /// <param name="Handle">The layer handle of the newly selected layer.</param>
        public void LayerSelected(int Handle)
        {

        }

        /// <summary>
        /// Occurs after a user selects a rectangular area in the MapWindow.  Normally this implies 
        /// selection.
        /// </summary>
        /// <param name="Bounds">The rectangle selected.</param>
        /// <param name="Handled">Reference parameter.  Setting Handled to true prevents other plugins from 
        /// receiving this event.</param>
        public void MapDragFinished(System.Drawing.Rectangle Bounds, ref bool Handled)
        {

        }

        /// <summary>
        /// This event is called when a plugin is loaded or turned on in the MapWindow.
        /// </summary>
        /// <param name="MapWin">The interface to use to access the MapWindow.</param>
        /// <param name="ParentHandle">The window handle of the MapWindow form.  This handle is useful for 
        /// making the MapWindow the owner of plugin forms.</param>
        public void Initialize(MapWindow.Interfaces.IMapWin MapWin, int ParentHandle)
        {
            try
            {
                _mapWin = MapWin;

                MapWindow.Interfaces.Toolbar t = MapWin.Toolbar;
                //Resources:
                clsResources cResources = new clsResources();

                t.AddToolbar(_toolbarName);
                MapWindow.Interfaces.ToolbarButton b = t.AddButton(_btnNameShp2Grid, _toolbarName, "", "");
                b.BeginsGroup = true;
                b.Tooltip = _toolTipShp2Grid;
                b.Category = _toolbarName;
   
                System.Drawing.Icon oIcon = cResources.GetEmbeddedIcon("shp2grid6.ico");
                b.Picture = new System.Drawing.Icon(oIcon, new System.Drawing.Size(16, 16));

                //Clean up:
                t = null;
                b = null;
            }
            catch (Exception ex)
            {
                _mapWin.ShowErrorDialog(ex);
            }
        }

        /// <summary>
        /// Occurs when a user releases a mouse button on the MapWindow main map display.
        /// </summary>
        /// <param name="Button">An integer representation of which button(s) were released.  Uses vb6 button constants.</param>
        /// <param name="Shift">An integer representation of the shift/alt/ctrl keys that were pressed at the time the mouse button was released.  Uses vb6 shift constants.</param>
        /// <param name="x">X coordinate in pixels.</param>
        /// <param name="y">Y coordinate in pixels.</param>
        /// <param name="Handled">Reference parameter.  Prevents other plugins from getting this event.</param>
        public void MapMouseUp(int Button, int Shift, int x, int y, ref bool Handled)
        {

        }

        /// <summary>
        /// Occurs when a user double clicks on the legend.
        /// </summary>
        /// <param name="Handle">The handle of the legend group or item that was clicked on.</param>
        /// <param name="Location">Enumerated.  The location clicked on.</param>
        /// <param name="Handled">Reference parameter.  When set to true it prevents additional plugins from getting this event.</param>
        public void LegendDoubleClick(int Handle, MapWindow.Interfaces.ClickLocation Location, ref bool Handled)
        {

        }

        /// <summary>
        /// Occurs when a user presses a mouse button on the legend.
        /// </summary>
        /// <param name="Handle">Layer or group handle that was clicked.</param>
        /// <param name="Button">The integer representation of the button used.  Uses vb6 mouse button constants.</param>
        /// <param name="Location">The part of the legend that was clicked.</param>
        /// <param name="Handled">Reference parameter.  Prevents other plugins from getting this event when set to true.</param>
        public void LegendMouseDown(int Handle, int Button, MapWindow.Interfaces.ClickLocation Location, ref bool Handled)
        {

        }

        /// <summary>
        /// Occurs when the user releases a mouse button over the legend.
        /// </summary>
        /// <param name="Handle">The handle of the group or layer.</param>
        /// <param name="Button">The integer representation of the button released.  Uses vb6 button constants.</param>
        /// <param name="Location">Enumeration.  Specifies if a group, layer or neither was clicked on.</param>
        /// <param name="Handled">Reference parameter.  Prevents other plugins from getting this event.</param>
        public void LegendMouseUp(int Handle, int Button, MapWindow.Interfaces.ClickLocation Location, ref bool Handled)
        {

        }

        /// <summary>
        /// Occurs when a layer is removed from the MapWindow.
        /// </summary>
        /// <param name="Handle">The handle of the layer being removed.</param>
        public void LayerRemoved(int Handle)
        {
            //MapWinUtility.Logger.Msg("layer removed. NumLayers: " + _mapWin.Layers.NumLayers.ToString());
        }

        /// <summary>
        /// Occurs when a user moves the mouse over the MapWindow main display.
        /// </summary>
        /// <param name="ScreenX">X coordinate in pixels.</param>
        /// <param name="ScreenY">Y coordinate in pixels.</param>
        /// <param name="Handled">Reference parameter.  Prevents other plugins from getting this event.</param>
        public void MapMouseMove(int ScreenX, int ScreenY, ref bool Handled)
        {

        }

        /// <summary>
        /// Occurs when the "Clear all layers" button is pressed in the MapWindow.
        /// </summary>
        public void LayersCleared()
        {

        }

        /// <summary>
        /// Occurs when shapes have been selected in the MapWindow.
        /// </summary>
        /// <param name="Handle">The handle of the layer that was selected on.</param>
        /// <param name="SelectInfo">Information about all the shapes that were selected.</param>
        public void ShapesSelected(int Handle, MapWindow.Interfaces.SelectInfo SelectInfo)
        {

        }

        /// <summary>
        /// Date that the plugin was built.
        /// </summary>
        public string BuildDate
        {
            get {return System.IO.File.GetLastAccessTime(System.Reflection.Assembly.GetExecutingAssembly().Location).ToString(); }
        }

        /// <summary>
        /// Description of the plugin.
        /// </summary>
        public string Description
        {
            get
            {
                return "This plug-in will convert shapefile to grid. " +
              "It works with polygon, line and point shapefiles. ";
            }
        }

        /// <summary>
        /// Author of the plugin.
        /// </summary>
        public string Author
        {
            get { return "Jiri Kadlec"; }
        }
        /// <summary>
        /// Name of the plugin.
        /// </summary>
        public string Name
        {
            get { return "Shapefile to grid"; }
        }

        /// <summary>
        /// Version of the plugin.
        /// </summary>
        public string Version
        {
            get
            {
                System.Diagnostics.FileVersionInfo f = System.Diagnostics.FileVersionInfo.GetVersionInfo(System.Reflection.Assembly.GetExecutingAssembly().Location);
                return f.FileMajorPart.ToString() + "." + f.FileMinorPart.ToString() + "." + f.FileBuildPart.ToString();
            }
        }

        /// <summary>
        /// Serial number of the plugin.
        /// The serial number and the name are tied together.  For each name there is a corresponding serial number.
        /// </summary>
        public string SerialNumber
        {
            get { return "This is Open Source"; }
        }
        #endregion
    }
}
