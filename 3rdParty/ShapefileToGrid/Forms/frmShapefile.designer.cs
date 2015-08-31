namespace jk.ShapefileToGrid
{
    partial class frmShapefile
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmShapefile));
            this.label1 = new System.Windows.Forms.Label();
            this.btnOpenFile = new System.Windows.Forms.Button();
            this.cmbLayers = new System.Windows.Forms.ComboBox();
            this.rdUseExternal = new System.Windows.Forms.RadioButton();
            this.rdUseLoaded = new System.Windows.Forms.RadioButton();
            this.cmbFields = new System.Windows.Forms.ComboBox();
            this.btnClose = new System.Windows.Forms.Button();
            this.btnNext = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lblFileName = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(16, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(355, 40);
            this.label1.TabIndex = 0;
            this.label1.Text = "This tool will create a new grid (raster) file from an existing polygon, point or" +
                " line shapefile";
            // 
            // btnOpenFile
            // 
            this.btnOpenFile.Enabled = false;
            this.btnOpenFile.Image = ((System.Drawing.Image)(resources.GetObject("btnOpenFile.Image")));
            this.btnOpenFile.Location = new System.Drawing.Point(154, 74);
            this.btnOpenFile.Name = "btnOpenFile";
            this.btnOpenFile.Size = new System.Drawing.Size(28, 24);
            this.btnOpenFile.TabIndex = 3;
            this.btnOpenFile.UseVisualStyleBackColor = true;
            this.btnOpenFile.Click += new System.EventHandler(this.btnOpenFile_Click);
            // 
            // cmbLayers
            // 
            this.cmbLayers.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbLayers.FormattingEnabled = true;
            this.cmbLayers.Location = new System.Drawing.Point(34, 46);
            this.cmbLayers.Name = "cmbLayers";
            this.cmbLayers.Size = new System.Drawing.Size(301, 21);
            this.cmbLayers.TabIndex = 1;
            this.cmbLayers.SelectedIndexChanged += new System.EventHandler(this.cmbLayers_SelectedIndexChanged);
            // 
            // rdUseExternal
            // 
            this.rdUseExternal.Location = new System.Drawing.Point(10, 78);
            this.rdUseExternal.Name = "rdUseExternal";
            this.rdUseExternal.Size = new System.Drawing.Size(140, 20);
            this.rdUseExternal.TabIndex = 2;
            this.rdUseExternal.Text = "Use a file from disk..";
            this.rdUseExternal.UseVisualStyleBackColor = true;
            this.rdUseExternal.CheckedChanged += new System.EventHandler(this.rdUseExternal_CheckedChanged);
            // 
            // rdUseLoaded
            // 
            this.rdUseLoaded.AutoSize = true;
            this.rdUseLoaded.Checked = true;
            this.rdUseLoaded.Location = new System.Drawing.Point(10, 22);
            this.rdUseLoaded.Name = "rdUseLoaded";
            this.rdUseLoaded.Size = new System.Drawing.Size(113, 17);
            this.rdUseLoaded.TabIndex = 0;
            this.rdUseLoaded.TabStop = true;
            this.rdUseLoaded.Text = "Use a loaded layer";
            this.rdUseLoaded.UseVisualStyleBackColor = true;
            // 
            // cmbFields
            // 
            this.cmbFields.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbFields.FormattingEnabled = true;
            this.cmbFields.Location = new System.Drawing.Point(34, 30);
            this.cmbFields.Name = "cmbFields";
            this.cmbFields.Size = new System.Drawing.Size(301, 21);
            this.cmbFields.TabIndex = 4;
            // 
            // btnClose
            // 
            this.btnClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnClose.Location = new System.Drawing.Point(169, 269);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(96, 25);
            this.btnClose.TabIndex = 6;
            this.btnClose.Text = "Close";
            this.btnClose.UseVisualStyleBackColor = true;
            // 
            // btnNext
            // 
            this.btnNext.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnNext.Location = new System.Drawing.Point(275, 269);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(96, 25);
            this.btnNext.TabIndex = 5;
            this.btnNext.Text = "Next..";
            this.btnNext.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.lblFileName);
            this.groupBox1.Controls.Add(this.btnOpenFile);
            this.groupBox1.Controls.Add(this.cmbLayers);
            this.groupBox1.Controls.Add(this.rdUseLoaded);
            this.groupBox1.Controls.Add(this.rdUseExternal);
            this.groupBox1.Location = new System.Drawing.Point(36, 50);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(335, 105);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Select a shapefile layer";
            // 
            // lblFileName
            // 
            this.lblFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lblFileName.AutoSize = true;
            this.lblFileName.Location = new System.Drawing.Point(188, 80);
            this.lblFileName.Name = "lblFileName";
            this.lblFileName.Size = new System.Drawing.Size(0, 13);
            this.lblFileName.TabIndex = 4;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.cmbFields);
            this.groupBox2.Location = new System.Drawing.Point(36, 175);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(335, 74);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Select the attribute field for grid cell values";
            // 
            // frmShapefile
            // 
            this.AcceptButton = this.btnNext;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnClose;
            this.ClientSize = new System.Drawing.Size(383, 308);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnNext);
            this.Controls.Add(this.btnClose);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmShapefile";
            this.Text = "Shapefile to grid";
            this.Load += new System.EventHandler(this.frmShapefile_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RadioButton rdUseExternal;
        private System.Windows.Forms.RadioButton rdUseLoaded;
        private System.Windows.Forms.ComboBox cmbLayers;
        private System.Windows.Forms.Button btnOpenFile;
        private System.Windows.Forms.ComboBox cmbFields;
        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.Button btnNext;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblFileName;

    }
}