using System;
using System.Collections.Generic;
using System.Text;

namespace EventTest
{
    public class CSEvent
    {
        private int value_;

        public delegate void SampleEventHandler(object sender, System.EventArgs e, int n);

        public event EventHandler sampleEvent;
        public event SampleEventHandler sampleEvent2;

        public int Value{
            get{
                return value_;
            }
            set{
                if(value <= 20000){
                    value_ = value;
                }
                else{
                    value_ = 20000;
                }
            }
        }

        public void Add(int n)
        {
            value_ += n;
            sampleEvent(this, null);
        }

        public void Add2(int n)
        {
            value_ += n;
            sampleEvent2(this, null, 100);
        }
    }
    /*
    public class Class1
    {
        protected int value_;

        public event EventHandler UpdateAllViews;

        public void Add(int n)
        {
            value_ += n;
            UpdateAllViews(this, null);
        }
    }
     */
}
