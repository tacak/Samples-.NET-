using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;

namespace STEP_B_Support_Tool
{
    public partial class Form1 : Form
    {
        string strUndoBuf;

        [DllImport("STEPBEDIT.dll")]
        static extern void FormatStepB(IntPtr lpBuffer, int bufsize, int ret);

        const int bufsize = 1000000;
        IntPtr buf;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int ret;
            try
            {
                ret = Int32.Parse(txtReturn.Text);
                if (ret < 0)
                {
                    ret = 0;
                    txtReturn.Text = "0";
                }
            }
            catch(Exception){
                MessageBox.Show("改行許容文字数に、数値に設定できない数が指定されたため、60 を設定します");
                ret = 60;
                txtReturn.Text = "60";
            }

            // 一時ファイルを作る
            using (TextWriter streamWriter = new StreamWriter("tmpfile", false, System.Text.Encoding.GetEncoding(932)))
            {
                streamWriter.Write(textBox1.Text);
            }

            // 整形前に戻すボタンを有効にする
            strUndoBuf = textBox1.Text;
            btnUndo.Enabled = true;

            // STEP B 的な整形
            FormatStepB(buf, bufsize, ret);
            textBox1.Text = Marshal.PtrToStringUni(buf);

            // 一時ファイルを消す
            File.Delete("tmpfile");

            // 2010/1/20 [削除] C++側での置換対応により削除
            // 辞書ファイルを元に製品名の整形
            //ProductNameNormalize();

            //クリップボードにコピーするかどうか
            if (chkClipBoard.Checked)
            {
                try
                {
                    Clipboard.SetText(textBox1.Text);
                }
                catch (ExternalException)
                {
                    //MessageBox.Show(ee.Message);
                }
            }
        }

        /* 2010/1/20 [削除] C++側での置換対応により削除 ここから
        void ProductNameNormalize(){
            using (TextReader streamReader = new StreamReader("dictionary.txt"))
            {
                string str = textBox1.Text;
                string OkName, NgName;
                while (streamReader.Peek() >= 0)
                {
                    OkName = streamReader.ReadLine();
                    NgName = streamReader.ReadLine();
                    while (!NgName.Equals("") && streamReader.Peek() >= 0)
                    {
                        str = MyReplace(str, NgName, OkName, true);
                        NgName = streamReader.ReadLine();
                    }
                }
                textBox1.Text = str;
            }
        }
        2010/1/20 [削除] C++側での置換対応により削除 ここまで */

        protected override bool ProcessDialogKey(Keys keyData)
        {
            int pos;

            switch (keyData)
            {
                case Keys.A | Keys.Control:
                    if (this.ActiveControl is TextBox)
                    {
                        TextBox txt = (TextBox)this.ActiveControl;
                        txt.SelectionStart = 0;
                        txt.SelectionLength = txt.Text.Length;
                        return true;
                    }
                    break;
                case Keys.F5:
                    pos = textBox1.SelectionStart;
                    textBox1.Text = textBox1.Text.Substring(0, pos) + "/@@" + textBox1.Text.Substring(pos);
                    textBox1.SelectionStart = pos + 3;
                    break;
                case Keys.F6:
                    pos = textBox1.SelectionStart;
                    textBox1.Text = textBox1.Text.Substring(0, pos) + "@@/" + textBox1.Text.Substring(pos);
                    textBox1.SelectionStart = pos + 3;
                    break;
            }

            return base.ProcessDialogKey(keyData);
        }

        /* 2010/1/20 [削除] C++側での置換対応により削除 ここから
        public string MyReplace(string str, string oldValue, string newValue, bool isIgnoreUpperLower)
        {
            string ret = str.Clone().ToString();
            if (isIgnoreUpperLower)
            {
                string strLower = ret.ToLower();
                string oldValueLower = oldValue.ToLower();
                int index = 0;
                while (index != -1)
                {
                    index = strLower.IndexOf(oldValueLower, index);
                    if (index == -1)
                        break;

                    string replaceTarget = ret.Substring(index, oldValue.Length);
                    ret = ret.Replace(replaceTarget, newValue);
                    strLower = ret.ToLower();
                }
            }
            else
            {
                ret = str.Replace(oldValue, newValue).ToString();
            }
            return ret;
        }
        2010/1/20 [削除] C++側での置換対応により削除 ここまで */

        private void Form1_Load(object sender, EventArgs e)
        {
            // 整形前に戻すバッファを初期化
            strUndoBuf = "";
            btnUndo.Enabled = false;

            // 作業領域を確保
            buf = Marshal.AllocHGlobal(bufsize);

            // 改行許容文字数を設定
            txtReturn.Text = "60";
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            // メモリ解放
            Marshal.FreeHGlobal(buf);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            //strUndoBuf = textBox1.Text;
            //MessageBox.Show("onChange");
        }

        private void btnUndo_Click(object sender, EventArgs e)
        {
            textBox1.Text = strUndoBuf;
            textBox1.SelectionStart = 0;
            btnUndo.Enabled = false;
        }
    }
}
