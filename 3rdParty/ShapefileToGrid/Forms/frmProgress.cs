using System;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.Threading;


namespace jk.ShapefileToGrid
{
    /// <summary>
    /// The form to show progress. This form implements the ICallback
    /// interface. It enables updating the progress bar also from a
    /// separate thread.
    /// </summary>
    public partial class frmProgress : Form, MapWinGIS.ICallback
    {
        public frmProgress()
        {
            InitializeComponent();
        }

        #region ICallback Members

        /// <summary>
        /// writes any error messages to the status textbox
        /// </summary>
        /// <param name="KeyOfSender"></param>
        /// <param name="ErrorMsg"></param>
        public void Error(string KeyOfSender, string ErrorMsg)
        {
            if (this.InvokeRequired == false)
            {
                string msg = ErrorMsg;
                
                this.Text = "Error: " + msg;
                MessageBox.Show("Error: " + msg);
            }
            else
            {
                ErrorDelegate errDeleg = new ErrorDelegate(Error);
                this.BeginInvoke(errDeleg, new object[] { KeyOfSender, ErrorMsg });
            }
        }

        /// <summary>
        /// writes messages about calculation status and progress
        /// </summary>
        /// <param name="KeyOfSender"></param>
        /// <param name="Percent"></param>
        /// <param name="Message"></param>
        public void Progress(string KeyOfSender, int Percent, string StatusMsg)
        {
            if (this.InvokeRequired == false)
            {
                string msg = StatusMsg;
                int perc = Percent;

                this.Text = msg + perc.ToString() + " % done!";
                calcProgressBar.Value = perc;
            }
            else
            {   
                ProgressDelegate progDeleg = new ProgressDelegate(Progress);
                this.BeginInvoke(progDeleg, new object[] { KeyOfSender, Percent, StatusMsg });
            }
        }

        #endregion ICALLBACK MEMBERS


        // DELEGATE for ICallback.Progress()
        #region DELEGATES

        delegate void ProgressDelegate
            (string keyOfSender, int percent, string Message);
        delegate void ErrorDelegate
            (string keyOfSender, string ErrorMsg);

        #endregion DELEGATES
    }

}