﻿using System;
using System.IO;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Navigation;

namespace TestProject3
{
	public partial class Window1
	{
		public Window1()
		{
			this.InitializeComponent();
			
			// Insert code required on object creation below this point.
		}

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            OnMouseDoubleClick1_BeginStoryboard = 0;
        }
	}
}