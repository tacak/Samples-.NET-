using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace ControlIDTest
{
    class MyButton : Button
    {
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                CLIENTCREATESTRUCT cs;

                cs.hWindowMenu = IntPtr.Zero;
                cs.idFirstChild = 256;
                cp.Param = cs;
                return cp;
            }
        }
    }
}
