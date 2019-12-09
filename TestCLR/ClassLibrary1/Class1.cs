using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ClassLibrary1
{
    [Guid("5806880D-0A95-4587-A087-1B14673E2B0F")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class Class1
    {
        public string[] GetStr()
        {
            string[] str = new string[3];

            str[0] = "aaa";
            str[1] = "bbb";
            str[2] = "ccc";

            return str;
        }

        public string[] TestFunc(string hoge)
        {
            string[] str = new string[3];

            str[0] = hoge;
            str[1] = hoge + hoge;
            str[2] = hoge + hoge + hoge;

            return str;
        }

        public int TestFunc2(int n)
        {
            return n * 2;
        }

        public int[,] TestFunc3(int n)
        {
            int [,] nn = new int[3,1];

            nn[0,0] = n;
            nn[1,0] = n * 2;
            nn[2,0] = n * 3;

            return nn;
        }

        public IntPtr[] TestFunc4(string hoge)
        {
            string[] str = new string[3];

            str[0] = hoge;
            str[1] = hoge + hoge;
            str[2] = hoge + hoge + hoge;

            IntPtr[] p = new IntPtr[3];
            p[0] = Marshal.StringToHGlobalUni(str[0]);
            p[1] = Marshal.StringToHGlobalUni(str[1]);
            p[2] = Marshal.StringToHGlobalUni(str[2]);

            return p;
        }
    }
}
