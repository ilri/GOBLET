namespace jk.ShapefileToGrid
{
    partial class frmProgress
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmProgress));
            this.calcProgressBar = new System.Windows.Forms.ProgressBar();
            this.SuspendLayout();
            // 
            // calcProgressBar
            // 
            this.calcProgressBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.calcProgressBar.Location = new System.Drawing.Point(-3, 0);
            this.calcProgressBar.Name = "calcProgressBar";
            this.calcProgressBar.Size = new System.Drawing.Size(295, 30);
            this.calcProgressBar.TabIndex = 0;
            // 
            // frmProgress
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 27);
            this.Controls.Add(this.calcProgressBar);
            this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmProgress";
            this.ShowIcon = false;
            this.Text = "Shapefile to grid - Starting calculation";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ProgressBar calcProgressBar;
    }
}

