using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace STEP_B_Support_Tool
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            // 変換エンジンの確認
            if (!File.Exists("STEPBEDIT.dll"))
            {
                MessageBox.Show("変換エンジン ( STEPBEDIT.dll ) が見つかりません。");
                return;
            }

            // 辞書ファイルの確認
            if (!File.Exists("dictionary.txt"))
            {
                MessageBox.Show("辞書ファイル ( dictionary.txt ) が見つかりません。");
                return;
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
