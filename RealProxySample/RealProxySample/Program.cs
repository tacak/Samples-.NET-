using System;
using System.Runtime.Remoting.Proxies;
using System.Runtime.Remoting.Messaging;
using System.Reflection;

namespace RealProxySample
{
    class Class1
    {
        [STAThread]
        static void Main(string[] args)
        {
            MyProxyClass mpc = new MyProxyClass(typeof(SampleClass));
            SampleClass o = mpc.GetTransparentProxy() as SampleClass;
            o.Print("あいうえお");
            int n = o.Add(10, 20);
            Console.WriteLine(n.ToString());
            Console.ReadLine();
        }
    }

    public class SampleClass : MarshalByRefObject
    {
        public void Print(string s)
        {
            Console.WriteLine(s);
        }

        public int Add(int a, int b)
        {
            return a + b;
        }
    }

    public class MyProxyClass : RealProxy
    {
        private Object myObjectInstance = null;
        private Type myType = null;

        public MyProxyClass(Type argType) : base(argType)
        {
            myType = argType; myObjectInstance = Activator.CreateInstance(argType);
        }

        public override IMessage Invoke(IMessage message)
        {
            Console.WriteLine("メソッド呼び出し");

            IMethodMessage myMethodMessage = (IMethodMessage)message;
            Object returnValue = myType.InvokeMember(myMethodMessage.MethodName, BindingFlags.InvokeMethod, null, myObjectInstance, myMethodMessage.Args);
            ReturnMessage myReturnMessage = new ReturnMessage(returnValue, null, 0, ((IMethodCallMessage)message).LogicalCallContext, (IMethodCallMessage)message);

            return myReturnMessage;
        }
    }
}