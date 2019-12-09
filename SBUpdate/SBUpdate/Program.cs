using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Diagnostics;
using System.Reflection;

namespace SBUpdate
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            // パスの取得
            Assembly myAssembly = Assembly.GetEntryAssembly();
            string strDir = Path.GetDirectoryName(myAssembly.Location);

            // ファイル数などの設定
            const uint cnt = 7;
            string[] strFileList = new string[cnt];
            int i;

            // STEPB.exe が起動している場合は、終了させる
            Process[] ps = Process.GetProcessesByName("STEPB.exe");
            foreach (Process p in ps)
            {
                //クローズメッセージを送信する
                p.CloseMainWindow();
            }

            System.Threading.Thread.Sleep(300);

            // コピー元ファイルの設定
            strFileList[0] = @"\\pepe\public\kimura\STEP B Support Tool\dictionary.txt";
            strFileList[1] = @"\\pepe\public\kimura\STEP B Support Tool\history.txt";
            strFileList[2] = @"\\pepe\public\kimura\STEP B Support Tool\Readme.txt";
            strFileList[3] = @"\\pepe\public\kimura\STEP B Support Tool\SBUpdate.exe";
            strFileList[4] = @"\\pepe\public\kimura\STEP B Support Tool\STEPB.exe";
            strFileList[5] = @"\\pepe\public\kimura\STEP B Support Tool\STEPBEDIT.dll";
            strFileList[6] = @"\\pepe\public\kimura\STEP B Support Tool\辞書ファイルの書き方.txt";

            for (i = 0; i < cnt; i++)
            {
                // 既に同名のファイルが存在していても上書きする
                File.Copy(strFileList[i],strDir, true);
            }

            // STEPB.exe を起動する
            Process.Start(strDir + @"\STEPB.exe");
        }
    }
}
