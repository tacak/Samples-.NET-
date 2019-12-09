using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace TestEXE
{
    public partial class Form1 : Form
    {
        //[DllImport("TestDLL.dll", CharSet = CharSet.Auto)]
        //private unsafe static extern bool TestFunc(void *ptr);

        //[DllImport("TestDLL.dll", EntryPoint = "TestFunc3")]
        //public static extern int TestFunc3(
        //    [MarshalAs(UnmanagedType.LPStr)]out string msg);

        [DllImport("TestDLL.dll", EntryPoint = "TestFunc3")]
        public static extern int TestFunc3(
            out IntPtr msg);

        [DllImport("TestDLL.dll")]
        public static extern int TestFunc4(
            [MarshalAs(UnmanagedType.LPStr)]out string msg);

        [DllImport("TestDLL.dll", EntryPoint = "TestFunc5")]
        public static extern int TestFunc5(out IntPtr msg);


        [DllImport("TestDLL.dll")]
        public extern static int TestFunc(ref System.Text.StringBuilder msg);

        [DllImport("TestDLL.dll", CharSet = CharSet.Auto)]
        private unsafe static extern bool TestFunc2(ref String str);

        [DllImport("kernel32.dll",
                    CharSet = System.Runtime.InteropServices.CharSet.Auto)]
        private unsafe static extern int FormatMessage(int dwFlags,
                                                       ref IntPtr lpSource,
                                                       int dwMessageId,
                                                       int dwLanguageId,
                                                       ref String lpBuffer, int nSize,
                                                       IntPtr* Arguments);

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            System.Text.StringBuilder msg = new System.Text.StringBuilder();
            msg.Append((char)0);
            TestFunc(ref msg); 

            /*
            IntPtr ptr = IntPtr.Zero;
            IntPtr work;

            //ptr = Marshal.AllocCoTaskMem(20);
            TestFunc(ref ptr);

            for (int i = 0; i < 1; i++)
            {
                work = (IntPtr)Marshal.ReadInt32(ptr);
                MessageBox.Show(Marshal.PtrToStringAnsi(work));
            }
            */
        }

        private void button2_Click(object sender, EventArgs e)
        {
            TestF();
        }

        public unsafe void TestF2()
        {
           }

        public unsafe void TestF()
        {
            int FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100;
            int FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200;
            int FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;
            int messageSize = 255;
            String lpMsgBuf = "";
            int dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS;
            IntPtr ptrlpSource = new IntPtr();
            IntPtr prtArguments = new IntPtr();
            int retVal = FormatMessage(dwFlags, ref ptrlpSource, 2, 0,
                                       ref lpMsgBuf, messageSize, &prtArguments);

            //MessageBox.Show((&lpMsgBuf).ToString());
            
            MessageBox.Show(lpMsgBuf);

        }

        private void button3_Click(object sender, EventArgs e)
        {
            String str = "";

            TestFunc2(ref str);
            MessageBox.Show(str);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            IntPtr msg = IntPtr.Zero;
            TestFunc3(out msg);
            MessageBox.Show(Marshal.PtrToStringAnsi(msg));

            //msg = "";
            //TestFunc4(out msg);
            //MessageBox.Show(msg);
        }
    }
}