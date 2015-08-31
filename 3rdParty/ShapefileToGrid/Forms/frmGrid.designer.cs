namespace jk.ShapefileToGrid
{
    partial class frmGrid
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmGrid));
            this.grpExtent = new System.Windows.Forms.GroupBox();
            this.lblNumberRows = new System.Windows.Forms.Label();
            this.lblNumberCols = new System.Windows.Forms.Label();
            this.lblRows1 = new System.Windows.Forms.Label();
            this.lblColumns1 = new System.Windows.Forms.Label();
            this.cmbExtent = new System.Windows.Forms.ComboBox();
            this.txtMaxX = new System.Windows.Forms.TextBox();
            this.lblMinY = new System.Windows.Forms.Label();
            this.txtMaxY = new System.Windows.Forms.TextBox();
            this.txtMinY = new System.Windows.Forms.TextBox();
            this.txtMinX = new System.Windows.Forms.TextBox();           
            this.lblMinX = new System.Windows.Forms.Label();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.txtCellSize = new System.Windows.Forms.TextBox();
            this.cmbCellSize = new System.Windows.Forms.ComboBox();
            this.grpCellSize = new System.Windows.Forms.GroupBox();
            this.lblCellSize = new System.Windows.Forms.Label();
            this.btnOpenFile = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.txtGridFile = new System.Windows.Forms.TextBox();
            this.cmbDataType = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.lblFileType = new System.Windows.Forms.Label();
            this.cmbFileType = new System.Windows.Forms.ComboBox();
            this.grpExtent.SuspendLayout();
            this.grpCellSize.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpExtent
            // 
            this.grpExtent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.grpExtent.Controls.Add(this.lblNumberRows);
            this.grpExtent.Controls.Add(this.lblNumberCols);
            this.grpExtent.Controls.Add(this.lblRows1);
            this.grpExtent.Controls.Add(this.lblColumns1);
            this.grpExtent.Controls.Add(this.cmbExtent);
            this.grpExtent.Controls.Add(this.txtMaxX);
            this.grpExtent.Controls.Add(this.lblMinY);
            this.grpExtent.Controls.Add(this.txtMaxY);
            this.grpExtent.Controls.Add(this.txtMinY);
            this.grpExtent.Controls.Add(this.txtMinX);
            this.grpExtent.Controls.Add(this.lblMinX);
            this.grpExtent.Location = new System.Drawing.Point(16, 127);
            this.grpExtent.Name = "grpExtent";
            this.grpExtent.Size = new System.Drawing.Size(355, 157);
            this.grpExtent.TabIndex = 2;
            this.grpExtent.TabStop = false;
            this.grpExtent.Text = "Select output grid extent";
            // 
            // lblNumberRows
            // 
            this.lblNumberRows.AutoSize = true;
            this.lblNumberRows.Location = new System.Drawing.Point(236, 134);
            this.lblNumberRows.Name = "lblNumberRows";
            this.lblNumberRows.Size = new System.Drawing.Size(0, 13);
            this.lblNumberRows.TabIndex = 13;
            // 
            // lblNumberCols
            // 
            this.lblNumberCols.AutoSize = true;
            this.lblNumberCols.Location = new System.Drawing.Point(73, 134);
            this.lblNumberCols.Name = "lblNumberCols";
            this.lblNumberCols.Size = new System.Drawing.Size(0, 13);
            this.lblNumberCols.TabIndex = 12;
            // 
            // lblRows1
            // 
            this.lblRows1.AutoSize = true;
            this.lblRows1.Location = new System.Drawing.Point(195, 134);
            this.lblRows1.Name = "lblRows1";
            this.lblRows1.Size = new System.Drawing.Size(37, 13);
            this.lblRows1.TabIndex = 11;
            this.lblRows1.Text = "Rows:";
            // 
            // lblColumns1
            // 
            this.lblColumns1.AutoSize = true;
            this.lblColumns1.Location = new System.Drawing.Point(17, 134);
            this.lblColumns1.Name = "lblColumns1";
            this.lblColumns1.Size = new System.Drawing.Size(50, 13);
            this.lblColumns1.TabIndex = 10;
            this.lblColumns1.Text = "Columns:";
            // 
            // cmbExtent
            // 
            this.cmbExtent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbExtent.FormattingEnabled = true;
            this.cmbExtent.Location = new System.Drawing.Point(20, 24);
            this.cmbExtent.Name = "cmbExtent";
            this.cmbExtent.Size = new System.Drawing.Size(334, 21);
            this.cmbExtent.TabIndex = 3;
            this.cmbExtent.SelectedIndexChanged += new System.EventHandler(this.cmbExtent_SelectedIndexChanged);
            // 
            // txtMaxX
            // 
            this.txtMaxX.Enabled = false;
            this.txtMaxX.Location = new System.Drawing.Point(239, 62);
            this.txtMaxX.Name = "txtMaxX";
            this.txtMaxX.Size = new System.Drawing.Size(96, 20);
            this.txtMaxX.TabIndex = 6;
            // 
            // lblMinY
            // 
            this.lblMinY.AutoSize = true;
            this.lblMinY.Location = new System.Drawing.Point(17, 99);
            this.lblMinY.Name = "lblMinY";
            this.lblMinY.Size = new System.Drawing.Size(34, 13);
            this.lblMinY.TabIndex = 7;
            this.lblMinY.Text = "Min.Y";
            // 
            // txtMaxY
            // 
            this.txtMaxY.Enabled = false;
            this.txtMaxY.Location = new System.Drawing.Point(239, 96);
            this.txtMaxY.Name = "txtMaxY";
            this.txtMaxY.Size = new System.Drawing.Size(96, 20);
            this.txtMaxY.TabIndex = 7;
            // 
            // txtMinY
            // 
            this.txtMinY.Enabled = false;
            this.txtMinY.Location = new System.Drawing.Point(60, 96);
            this.txtMinY.Name = "txtMinY";
            this.txtMinY.Size = new System.Drawing.Size(96, 20);
            this.txtMinY.TabIndex = 5;
            // 
            // txtMinX
            // 
            this.txtMinX.Enabled = false;
            this.txtMinX.Location = new System.Drawing.Point(60, 62);
            this.txtMinX.Name = "txtMinX";
            this.txtMinX.Size = new System.Drawing.Size(96, 20);
            this.txtMinX.TabIndex = 4;
            // 
            // lblMinX
            // 
            this.lblMinX.AutoSize = true;
            this.lblMinX.Location = new System.Drawing.Point(17, 65);
            this.lblMinX.Name = "lblMinX";
            this.lblMinX.Size = new System.Drawing.Size(37, 13);
            this.lblMinX.TabIndex = 3;
            this.lblMinX.Text = "Min. X";
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(159, 418);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(96, 29);
            this.btnCancel.TabIndex = 12;
            this.btnCancel.Text = "..Back";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(271, 418);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(100, 29);
            this.btnOK.TabIndex = 13;
            this.btnOK.Text = "Finish";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // txtCellSize
            // 
            this.txtCellSize.Enabled = false;
            this.txtCellSize.Location = new System.Drawing.Point(107, 63);
            this.txtCellSize.Name = "txtCellSize";
            this.txtCellSize.Size = new System.Drawing.Size(99, 20);
            this.txtCellSize.TabIndex = 2;
            this.txtCellSize.LostFocus += new System.EventHandler(this.cmbExtent_SelectedIndexChanged);
            this.txtCellSize.TextChanged += new System.EventHandler(this.txtCellSize_TextChanged);
            // 
            // cmbCellSize
            // 
            this.cmbCellSize.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbCellSize.FormattingEnabled = true;
            this.cmbCellSize.Location = new System.Drawing.Point(23, 23);
            this.cmbCellSize.Name = "cmbCellSize";
            this.cmbCellSize.Size = new System.Drawing.Size(332, 21);
            this.cmbCellSize.TabIndex = 1;
            this.cmbCellSize.SelectedIndexChanged += new System.EventHandler(this.cmbCellSize_SelectedIndexChanged);
            // 
            // grpCellSize
            // 
            this.grpCellSize.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.grpCellSize.Controls.Add(this.lblCellSize);
            this.grpCellSize.Controls.Add(this.cmbCellSize);
            this.grpCellSize.Controls.Add(this.txtCellSize);
            this.grpCellSize.Location = new System.Drawing.Point(16, 16);
            this.grpCellSize.Name = "grpCellSize";
            this.grpCellSize.Size = new System.Drawing.Size(355, 97);
            this.grpCellSize.TabIndex = 13;
            this.grpCellSize.TabStop = false;
            this.grpCellSize.Text = "Select grid cell size (choose from list or enter a number)";
            // 
            // lblCellSize
            // 
            this.lblCellSize.AutoSize = true;
            this.lblCellSize.Location = new System.Drawing.Point(26, 66);
            this.lblCellSize.Name = "lblCellSize";
            this.lblCellSize.Size = new System.Drawing.Size(77, 13);
            this.lblCellSize.TabIndex = 0;
            this.lblCellSize.Text = "Cell size value:";
            // 
            // btnOpenFile
            // 
            this.btnOpenFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOpenFile.Image = ((System.Drawing.Image)(resources.GetObject("btnOpenFile.Image")));
            this.btnOpenFile.Location = new System.Drawing.Point(343, 371);
            this.btnOpenFile.Name = "btnOpenFile";
            this.btnOpenFile.Size = new System.Drawing.Size(28, 24);
            this.btnOpenFile.TabIndex = 11;
            this.btnOpenFile.UseVisualStyleBackColor = true;
            this.btnOpenFile.Click += new System.EventHandler(this.btnOpenFile_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(33, 358);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(114, 13);
            this.label6.TabIndex = 15;
            this.label6.Text = "Choose grid file name..";
            // 
            // txtGridFile
            // 
            this.txtGridFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtGridFile.Location = new System.Drawing.Point(36, 374);
            this.txtGridFile.Name = "txtGridFile";
            this.txtGridFile.Size = new System.Drawing.Size(301, 20);
            this.txtGridFile.TabIndex = 10;
            // 
            // cmbDataType
            // 
            this.cmbDataType.FormattingEnabled = true;
            this.cmbDataType.Location = new System.Drawing.Point(214, 316);
            this.cmbDataType.Name = "cmbDataType";
            this.cmbDataType.Size = new System.Drawing.Size(137, 21);
            this.cmbDataType.TabIndex = 9;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(215, 296);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(127, 13);
            this.label7.TabIndex = 18;
            this.label7.Text = "Grid data type (precision):";
            // 
            // lblFileType
            // 
            this.lblFileType.AutoSize = true;
            this.lblFileType.Location = new System.Drawing.Point(33, 296);
            this.lblFileType.Name = "lblFileType";
            this.lblFileType.Size = new System.Drawing.Size(68, 13);
            this.lblFileType.TabIndex = 19;
            this.lblFileType.Text = "Grid file type:";
            // 
            // cmbFileType
            // 
            this.cmbFileType.FormattingEnabled = true;
            this.cmbFileType.Location = new System.Drawing.Point(36, 316);
            this.cmbFileType.Name = "cmbFileType";
            this.cmbFileType.Size = new System.Drawing.Size(136, 21);
            this.cmbFileType.TabIndex = 8;
            this.cmbFileType.SelectedIndexChanged += new System.EventHandler(this.cmbFileType_SelectedIndexChanged);
            // 
            // frmGrid
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(385, 463);
            this.Controls.Add(this.cmbFileType);
            this.Controls.Add(this.lblFileType);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.cmbDataType);
            this.Controls.Add(this.txtGridFile);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.btnOpenFile);
            this.Controls.Add(this.grpCellSize);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.grpExtent);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmGrid";
            this.Text = "Shapefile to Grid - Select grid properties";
            this.Load += new System.EventHandler(this.frmGrid_Load);
            this.grpExtent.ResumeLayout(false);
            this.grpExtent.PerformLayout();
            this.grpCellSize.ResumeLayout(false);
            this.grpCellSize.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox grpExtent;
        private System.Windows.Forms.Label lblMinX;
        private System.Windows.Forms.TextBox txtMinX;
        private System.Windows.Forms.TextBox txtMaxY;
        private System.Windows.Forms.TextBox txtMinY;
        private System.Windows.Forms.Label lblMinY;
        private System.Windows.Forms.TextBox txtMaxX;
        private System.Windows.Forms.ComboBox cmbExtent;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.ComboBox cmbCellSize;
        private System.Windows.Forms.TextBox txtCellSize;
        private System.Windows.Forms.GroupBox grpCellSize;
        private System.Windows.Forms.Label lblCellSize;
        private System.Windows.Forms.Button btnOpenFile;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtGridFile;
        private System.Windows.Forms.ComboBox cmbDataType;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label lblFileType;
        private System.Windows.Forms.ComboBox cmbFileType;
        private System.Windows.Forms.Label lblColumns1;
        private System.Windows.Forms.Label lblRows1;
        private System.Windows.Forms.Label lblNumberRows;
        private System.Windows.Forms.Label lblNumberCols;
    }
}