using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace ReplaceEventHandler
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            MessageBox.Show("button1_Click EventHandler!");
        }

        private void button1_Click_New(object sender, EventArgs e)
        {
            MessageBox.Show("button1_Click_New EventHandler!");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Type t;
            Type h;
            Type target;
            MethodInfo mi;
            Delegate d;
            EventInfo ei;

            t = this.GetType();
            target = button1.GetType();
            h = typeof(EventHandler);

            mi = t.GetMethod(
              "button1_Click",
              BindingFlags.NonPublic | BindingFlags.Instance);
            ei = target.GetEvent(
              "Click",
              BindingFlags.Instance | BindingFlags.Public);
            d = Delegate.CreateDelegate(h, this, mi);
            ei.RemoveEventHandler(button1, d);

            mi = t.GetMethod(
              "button1_Click_New",
              BindingFlags.NonPublic | BindingFlags.Instance);
            ei = target.GetEvent(
              "Click",
              BindingFlags.Instance | BindingFlags.Public);
            d = Delegate.CreateDelegate(h, this, mi);
            ei.AddEventHandler(button1, d);

            MessageBox.Show("Replace!");
        }
    }
}