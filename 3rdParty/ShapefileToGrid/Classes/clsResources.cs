using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using MapWinUtility;

namespace jk.ShapefileToGrid
{
    public class clsResources
    {
        private System.Reflection.Assembly _assembly;
        private string _namespace;

        /// <summary>
        /// Constructor
        /// </summary>
        public clsResources()
        {
            _assembly = System.Reflection.Assembly.GetExecutingAssembly();
            getNamespace();
        }
        public clsResources(bool useEntryAssembly)
        {
            _assembly = System.Reflection.Assembly.GetEntryAssembly();
            getNamespace();
        }
        /// <summary>
        /// Destructor
        /// </summary>
        ~clsResources()
        {
            _assembly = null;
        }
        private void getNamespace()
        {
            _namespace = _assembly.GetName().Name.ToString() + ".Resources";
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="transparant"></param>
        /// <returns></returns>
        public Bitmap GetEmbeddedBitmap(string name, bool transparant)
        {
            try
            {
                Bitmap bm = null;
                string resourceName = _namespace + "." + name;
                Stream picture_stream = _assembly.GetManifestResourceStream(resourceName);
                if (picture_stream != null)
                {
                    bm = new Bitmap(picture_stream);
                    if (transparant) bm.MakeTransparent();
                    picture_stream.Close();
                }
                else
                {                    
                    GetListOfEmbeddedResources();
                    throw new Exception("GetEmbeddedBitmap has no stream: " + resourceName);
                }
                return bm;
            }
            catch (Exception ex)
            {
                MessageBox.Show("ERROR in GetEmbeddedBitmap: " + ex.ToString());
                throw (ex);
            }
        }

        public Icon GetEmbeddedIcon(string name)
        {
            try
            {
                Icon ic = null;
                string resourceName = _namespace + "." + name;
                Stream picture_stream = _assembly.GetManifestResourceStream(resourceName);
                if (picture_stream != null)
                {
                    ic = new Icon(picture_stream);
                    picture_stream.Close();
                }
                else
                {                    
                    GetListOfEmbeddedResources();
                    throw new Exception("GetEmbeddedIcon has no stream: " + resourceName);
                }
                return ic;
            }
            catch (Exception ex)
            {
                MessageBox.Show("ERROR in GetEmbeddedIcon: " + ex.ToString());
                throw (ex);
            }

        }

        public void GetListOfEmbeddedResources()
        {
            string logfilename = @"c:\resourcesOf" + _namespace.Replace(" ", "-") + ".log";
            Logger.StartToFile(logfilename,false, false,false);
            Logger.Dbg("In GetListOfEmbeddedResources");
            Logger.Dbg("Resources of " + _namespace);
            string[] names = _assembly.GetManifestResourceNames();
            
            foreach(string name in names)
            {
                Logger.Dbg(name);
            }
            MessageBox.Show(logfilename + " created.");
            Logger.Flush();
        }        
    }
}
