using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FileTimeStump
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // パスを定数で定義する
            string strFilePath = textBox1.Text;

            // 作成日時を取得する
            DateTime dtCreate = System.IO.File.GetCreationTime(strFilePath);

            // 更新日時を取得する
            DateTime dtUpdate = System.IO.File.GetLastWriteTime(strFilePath);

            // アクセス日時を取得する
            DateTime dtAccess = System.IO.File.GetLastAccessTime(strFilePath);

            // 取得したタイムスタンプを表示する
            string msg;
            msg = "作成日時 : " + dtCreate.ToString() + "\r\n";
            msg += "更新日時 : " + dtUpdate.ToString() + "\r\n";
            msg += "アクセス日時 : " + dtAccess.ToString();
            MessageBox.Show(msg);
        }
    }
}
