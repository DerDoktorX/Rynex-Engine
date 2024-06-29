

using System;

namespace Rynex {
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main Constructor!");
        }
    
        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }
    
        public void PrintCustemMessage(string masseg)
        {
            Console.WriteLine($"C# says: {masseg}");
        }
    }
}
