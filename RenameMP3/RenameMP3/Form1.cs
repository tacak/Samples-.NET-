using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace RenameMP3
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string strNewName;
            string[] dirs = Directory.GetDirectories(textBox1.Text);
            foreach (string s in dirs)
            {
                strNewName = s.Replace("】 ", "】");
                if (!strNewName.Equals(s))
                {
                    Directory.Move(s, strNewName);
                }
            }
        }
    }
}
