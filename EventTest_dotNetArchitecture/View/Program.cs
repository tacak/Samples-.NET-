using System;
using Document;
using EventTest;

namespace trial
{
    public class View
    {
        private void OnUpdate(object sender, System.EventArgs e)
        {
            System.Console.WriteLine("sum = {0}", ((Document)sender).Value());
        }
        public static void Main()
        {
            CSEvent c = new CSEvent();
            View v = new View();
            Document d = new Document();

            d.UpdateAllViews += v.OnUpdate;
            for (int i = 0; i < 5; ++i)
            {
                d.Add(i);
            }

            Console.WriteLine("=========================");
            c.sampleEvent += new EventHandler(c_sampleEvent);
            for (int i = 0; i < 5; ++i)
            {
                c.Add(i);
            }

            Console.WriteLine("=========================");
            c.sampleEvent2 += new CSEvent.SampleEventHandler(c_sampleEvent2);
            for (int i = 0; i < 5; ++i)
            {
                c.Add2(i);
            }
        }

        static void c_sampleEvent2(object sender, EventArgs e, int n)
        {

            System.Console.WriteLine("sampleEvent2 = {0} , {1}", ((CSEvent)sender).Value, n);
        }

        static void c_sampleEvent(object sender, EventArgs e)
        {
            System.Console.WriteLine("sampleEvent2 = {0}", ((CSEvent)sender).Value);
        }
    }
}