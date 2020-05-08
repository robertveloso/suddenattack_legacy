using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;
using System.Net;
using System.Collections;
using System.Security.Cryptography;

namespace SALauncher
{
    public partial class frmMain : Form
    {
        private String iniPath = ".\\launcher.ini";
        private String exePath = ".\\SuddenAttack.exe";
        private String noticeURL = "http://127.0.0.1/notice.php";
        private String updateURL = "http://127.0.0.1/SA/";
        private String serverIP = "127.0.0.1";
        private String serverPort = "12000";
        private frmOptions Options = new frmOptions();


        [DllImport("kernel32.dll", EntryPoint = "GetPrivateProfileString")]
        private static extern Int32 GetPrivateProfileString(String section, String key, String def, StringBuilder retVal, Int32 size, String filePath);

        [DllImport("kernel32.dll", EntryPoint = "WritePrivateProfileString")]
        private static extern bool WritePrivateProfileString(String section, String key, String val, String filePath);

        public frmMain()
        {
            InitializeComponent();
            this.BootLauncher();
        }

        // Funções
        private Int32 IniReadValue(String Section, String Key, ref String Value )
        {
            StringBuilder temp = new StringBuilder(255);
            Int32 result = GetPrivateProfileString(Section, Key, "", temp, 255, this.iniPath);
            Value = temp.ToString();
            return result;
        }

        private void IniWriteValue(String Section, String Key, String Value)
        {
            WritePrivateProfileString(Section, Key, Value, this.iniPath);
        }

        private void BootLauncher()
        {
            this.LoadConfigs();
            webBrowser.Url = new System.Uri(this.noticeURL);
        }

        private void LoadConfigs()
        {
            if (!File.Exists(this.iniPath))
            {
                IniWriteValue("Launcher", "NoticeURL", this.noticeURL);
                IniWriteValue("Launcher", "UpdateURL", this.updateURL);
                IniWriteValue("Launcher", "ServerIP", this.serverIP);
                IniWriteValue("Launcher", "ServerPort", this.serverPort);
            }
            else
            {
                String read = "";

                if (IniReadValue("Launcher", "NoticeURL", ref read) == 0)
                    IniWriteValue("Launcher", "NoticeURL", this.noticeURL);
                else
                    this.noticeURL = read;

                if (IniReadValue("Launcher", "UpdateURL", ref read) == 0)
                    IniWriteValue("Launcher", "UpdateURL", this.updateURL);
                else
                    this.updateURL = read;

                if (IniReadValue("Launcher", "ServerIP", ref read) == 0)
                    IniWriteValue("Launcher", "ServerIP", this.serverIP);
                else
                    this.serverIP = read;

                if (IniReadValue("Launcher", "ServerPort", ref read) == 0)
                    IniWriteValue("Launcher", "ServerPort", this.serverPort);
                else
                    this.serverPort = read;
            }
        }

        // Eventos
        private void picClose_Click(object sender, EventArgs e)
        {
            this.picClose.Image = global::SALauncher.Properties.Resources.close_2;
            this.Close();
        }

        private void picClose_MouseHover(object sender, EventArgs e)
        {
            this.picClose.Image = global::SALauncher.Properties.Resources.close_3;
        }

        private void picClose_MouseLeave(object sender, EventArgs e)
        {
            this.picClose.Image = global::SALauncher.Properties.Resources.close_1;
        }

        private void picPlay_Click(object sender, EventArgs e)
        {
            this.picPlay.Image = global::SALauncher.Properties.Resources.play_1;
            if (!File.Exists(this.exePath))
            {
                MessageBox.Show("Não foi possível inicializar o jogo");
            }
            else
            {
                String idUser = this.Options.txtIdUser.Text;
                String window = (this.Options.chkWindow.Checked) ? "1" : "0";
                String log = (this.Options.chkLog.Checked) ? "1" : "0";
                String serverIp = this.serverIP;
                String serverPort = this.serverPort;

                ProcessStartInfo command = new ProcessStartInfo();
                command.FileName = exePath;
                command.Arguments = " +UserNO \"" + this.Options.txtIdUser.Text + "\" -rez engine.rez -rez Game -windowtitle \"SuddentAttak\" +windowed " + window + " +errorlog " + log + " +UserID \"ID1\" +UseF12 1 +skiptitle 1 +ServerIP \"" + serverIp + "\" +ServerPort " + serverPort;
                
                Process.Start(@command);
            }
            this.Close();
        }

        private void picPlay_MouseHover(object sender, EventArgs e)
        {
            this.picPlay.Image = global::SALauncher.Properties.Resources.play_4;
        }

        private void picPlay_MouseLeave(object sender, EventArgs e)
        {
            this.picPlay.Image = global::SALauncher.Properties.Resources.play_2;
        }

        private void lblOption_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            this.Options.Show();
        }
    }
}

