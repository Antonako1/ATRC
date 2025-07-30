using System;
using System.Runtime.InteropServices;
using ATRC;

class Program
{
    static void Main()
    {
        // Print current directory
        using var fd = new ATRC.ATRC("../../../../../example.atrc", ATRC.ReadMode.ATRC_READ_ONLY);
        string? res = fd.ReadVariable("test");
        System.Console.WriteLine(res ?? "Variable not found or empty.");
    }
}
