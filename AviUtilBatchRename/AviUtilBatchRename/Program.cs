using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace AviUtilBatchRename
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("話数を入力してください");

                return;
            }

            string strCurDir = new FileInfo(Environment.CommandLine.Replace("\"", "")).DirectoryName;
            DirectoryInfo current = new DirectoryInfo(strCurDir);

            // 既存ファイルを _batchNN.aup にリネーム
            int cnt = 0;
            FileInfo fWork;
            int iMax = Int32.Parse(args[0]) * 2;
            for (int i = 0; i < iMax; i++)
            {
                fWork = new FileInfo(strCurDir + @"\batch" + i.ToString() + ".aup");
                fWork.MoveTo("_" + fWork.Name);
            }

            // 話数分ループ
            cnt = 1;
            for (int i = 0; i < iMax; i++)
            {
                if (i < iMax / 2)
                {
                    fWork = new FileInfo(strCurDir + @"\_batch" + i.ToString() + ".aup");
                    fWork.MoveTo(@"batch" + (i * 2) + ".aup");
                }
                else
                {
                    fWork = new FileInfo(strCurDir + @"\_batch" + i.ToString() + ".aup");
                    fWork.MoveTo(@"batch" + cnt.ToString() + ".aup");
                    cnt += 2;
                }
            }

        }
    }
}
