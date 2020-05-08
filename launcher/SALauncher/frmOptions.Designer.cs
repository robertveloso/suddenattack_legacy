namespace SALauncher
{
    partial class frmOptions
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmOptions));
            this.chkWindow = new System.Windows.Forms.CheckBox();
            this.chkLog = new System.Windows.Forms.CheckBox();
            this.txtIdUser = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnOk = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // chkWindow
            // 
            this.chkWindow.AutoSize = true;
            this.chkWindow.Location = new System.Drawing.Point(6, 19);
            this.chkWindow.Name = "chkWindow";
            this.chkWindow.Size = new System.Drawing.Size(163, 17);
            this.chkWindow.TabIndex = 0;
            this.chkWindow.Text = "Abrir Cliente em Modo Janela";
            this.chkWindow.UseVisualStyleBackColor = true;
            // 
            // chkLog
            // 
            this.chkLog.AutoSize = true;
            this.chkLog.Location = new System.Drawing.Point(6, 42);
            this.chkLog.Name = "chkLog";
            this.chkLog.Size = new System.Drawing.Size(83, 17);
            this.chkLog.TabIndex = 1;
            this.chkLog.Text = "Gravar LOG";
            this.chkLog.UseVisualStyleBackColor = true;
            // 
            // txtIdUser
            // 
            this.txtIdUser.Location = new System.Drawing.Point(69, 62);
            this.txtIdUser.Name = "txtIdUser";
            this.txtIdUser.Size = new System.Drawing.Size(100, 20);
            this.txtIdUser.TabIndex = 2;
            this.txtIdUser.Text = "1";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.chkWindow);
            this.groupBox1.Controls.Add(this.chkLog);
            this.groupBox1.Controls.Add(this.txtIdUser);
            this.groupBox1.Location = new System.Drawing.Point(12, 11);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(179, 106);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Opções do Cliente";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 65);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "ID Usuário";
            // 
            // btnOk
            // 
            this.btnOk.Location = new System.Drawing.Point(12, 123);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(179, 31);
            this.btnOk.TabIndex = 4;
            this.btnOk.Text = "OK";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // frmOptions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(204, 166);
            this.Controls.Add(this.btnOk);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmOptions";
            this.Text = "Opções";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnOk;
        public System.Windows.Forms.CheckBox chkWindow;
        public System.Windows.Forms.CheckBox chkLog;
        public System.Windows.Forms.TextBox txtIdUser;
    }
}