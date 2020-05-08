namespace SALauncher
{
    partial class frmMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.picPlay = new System.Windows.Forms.PictureBox();
            this.webBrowser = new System.Windows.Forms.WebBrowser();
            this.picClose = new System.Windows.Forms.PictureBox();
            this.picProgress = new System.Windows.Forms.PictureBox();
            this.lblOption = new System.Windows.Forms.LinkLabel();
            ((System.ComponentModel.ISupportInitialize)(this.picPlay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picClose)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picProgress)).BeginInit();
            this.SuspendLayout();
            // 
            // picPlay
            // 
            this.picPlay.Image = global::SALauncher.Properties.Resources.play_2;
            this.picPlay.InitialImage = null;
            this.picPlay.Location = new System.Drawing.Point(438, 434);
            this.picPlay.Name = "picPlay";
            this.picPlay.Size = new System.Drawing.Size(193, 65);
            this.picPlay.TabIndex = 0;
            this.picPlay.TabStop = false;
            this.picPlay.Click += new System.EventHandler(this.picPlay_Click);
            this.picPlay.MouseLeave += new System.EventHandler(this.picPlay_MouseLeave);
            this.picPlay.MouseHover += new System.EventHandler(this.picPlay_MouseHover);
            // 
            // webBrowser
            // 
            this.webBrowser.Location = new System.Drawing.Point(46, 116);
            this.webBrowser.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser.Name = "webBrowser";
            this.webBrowser.Size = new System.Drawing.Size(576, 296);
            this.webBrowser.TabIndex = 1;
            // 
            // picClose
            // 
            this.picClose.BackColor = System.Drawing.Color.Transparent;
            this.picClose.Image = global::SALauncher.Properties.Resources.close_1;
            this.picClose.Location = new System.Drawing.Point(640, 7);
            this.picClose.Name = "picClose";
            this.picClose.Size = new System.Drawing.Size(22, 20);
            this.picClose.TabIndex = 2;
            this.picClose.TabStop = false;
            this.picClose.Click += new System.EventHandler(this.picClose_Click);
            this.picClose.MouseLeave += new System.EventHandler(this.picClose_MouseLeave);
            this.picClose.MouseHover += new System.EventHandler(this.picClose_MouseHover);
            // 
            // picProgress
            // 
            this.picProgress.Image = global::SALauncher.Properties.Resources.bar;
            this.picProgress.Location = new System.Drawing.Point(40, 462);
            this.picProgress.Name = "picProgress";
            this.picProgress.Size = new System.Drawing.Size(381, 26);
            this.picProgress.TabIndex = 3;
            this.picProgress.TabStop = false;
            // 
            // lblOption
            // 
            this.lblOption.AutoSize = true;
            this.lblOption.BackColor = System.Drawing.Color.Transparent;
            this.lblOption.Location = new System.Drawing.Point(374, 494);
            this.lblOption.Name = "lblOption";
            this.lblOption.Size = new System.Drawing.Size(44, 13);
            this.lblOption.TabIndex = 4;
            this.lblOption.TabStop = true;
            this.lblOption.Text = "Opções";
            this.lblOption.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lblOption_LinkClicked);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Info;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(670, 560);
            this.Controls.Add(this.lblOption);
            this.Controls.Add(this.picProgress);
            this.Controls.Add(this.picClose);
            this.Controls.Add(this.webBrowser);
            this.Controls.Add(this.picPlay);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmMain";
            ((System.ComponentModel.ISupportInitialize)(this.picPlay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picClose)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picProgress)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox picPlay;
        private System.Windows.Forms.WebBrowser webBrowser;
        private System.Windows.Forms.PictureBox picClose;
        private System.Windows.Forms.PictureBox picProgress;
        private System.Windows.Forms.LinkLabel lblOption;

    }
}

