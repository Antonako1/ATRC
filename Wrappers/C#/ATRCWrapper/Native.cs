/*+++
This project is licensed under the BSD 2-Clause License.
See the LICENSE file in the project root for license information.

Header file for the ATRC (Advanced Tagged Resource Configuration Library) library.
  This file contains the declarations for the ATRC library, which provides functionality
for reading, writing, and manipulating configuration files in a structured format.
  This library is designed to be used in both C and C++, with this wrapper for C# projects, with a focus on
ease of use and flexibility.

Author(s): 
    Antonako1

Maintained at https://github.com/Antonako1/ATRC
---*/

using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices.Marshalling;

namespace ATRCNative
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct Variable
    {
        internal IntPtr Name;
        internal IntPtr Value;
        [MarshalAs(UnmanagedType.I1)] internal bool IsPublic;
        internal ulong line_number;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Variable_Arr
    {
        internal IntPtr Variables; // Pointer to Variable[]
        internal ulong VariableCount;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Key
    {
        internal IntPtr Name;
        internal IntPtr Value;
        internal ulong line_number;
        internal ulong enum_value;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Key_Arr
    {
        internal IntPtr Keys;
        internal ulong KeyCount;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Block
    {
        internal IntPtr Name;
        internal Key_Arr KeyArray;
        internal ulong line_number;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Block_Arr
    {
        internal IntPtr Blocks;
        internal ulong BlockCount;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct ATRC_FD
    {
        internal Variable_Arr VariableArray;
        internal Block_Arr BlockArray;
        internal IntPtr Filename;
        [MarshalAs(UnmanagedType.I1)] internal bool AutoSave;
        [MarshalAs(UnmanagedType.I1)] internal bool Writecheck;
    }

    internal static partial class ATRC_Native
    {

        
        internal const string LIB = "ATRC";

        [LibraryImport(LIB, EntryPoint = "Create_Empty_ATRC_FD")]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr Create_Empty_ATRC_FD();

        [LibraryImport(LIB, EntryPoint = "Create_ATRC_FD", StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr Create_ATRC_FD(string filename, ATRC.ReadMode mode);

        [LibraryImport(LIB, EntryPoint = "Destroy_ATRC_FD")]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial void Destroy_ATRC_FD(IntPtr fd);

        [LibraryImport(LIB, EntryPoint = "Read", StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool Read(IntPtr fd, string path, ATRC.ReadMode mode);

        [LibraryImport(LIB, EntryPoint = "ReadVariable", StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr ReadVariable(IntPtr fd, string varname);

        [LibraryImport(LIB, EntryPoint = "ReadKey", StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr ReadKey(IntPtr fd, string block, string key);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool DoesExistBlock(IntPtr fd, string block);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool DoesExistVariable(IntPtr fd, string varname);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool DoesExistKey(IntPtr fd, string block, string key);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool IsPublic(IntPtr fd, string varname);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr InsertVar_S(string line, IntPtr args);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool AddBlock(IntPtr fd, string blockname);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool RemoveBlock(IntPtr fd, string blockname);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool AddVariable(IntPtr fd, string varname, string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool RemoveVariable(IntPtr fd, string varname);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool ModifyVariable(IntPtr fd, string varname, string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool AddKey(IntPtr fd, string block, string key, string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool RemoveKey(IntPtr fd, string block, string key);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool ModifyKey(IntPtr fd, string block, string key, string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool WriteCommentToTop(IntPtr fd, string comment);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool WriteCommentToBottom(IntPtr fd, string comment);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial ulong GetEnumValue(IntPtr fd, string block, string key);

        [LibraryImport(LIB)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial void ATRC_FREE_MEMORY(IntPtr ptr);

        [LibraryImport(LIB)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr Copy_ATRC_FD(IntPtr ptr);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf16)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial IntPtr atrc_to_list(char separator, string value);

        [LibraryImport(LIB)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial void atrc_free_list(IntPtr list);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static partial bool atrc_to_bool(string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial ulong atrc_to_uint64_t(string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial long atrc_to_int64_t(string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial double atrc_to_double(string value);

        [LibraryImport(LIB, StringMarshalling = StringMarshalling.Utf8)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        internal static partial double atrc_math_exp(string value);
    }
}



namespace ATRC
{
    /// <summary>
    /// Enumeration for the read modes of ATRC files.
    /// </summary>
    public enum ReadMode
    {
        /// <summary>
        /// Reads file if it exists
        /// </summary>
        ATRC_READ_ONLY,

        /// <summary>
        /// Reads file if it exists, otherwise creates a new file
        /// </summary>
        ATRC_CREATE_READ,

        /// <summary>
        /// Deletes the file if it exists and creates a new one
        /// </summary>
        ATRC_FORCE_READ
    }
}