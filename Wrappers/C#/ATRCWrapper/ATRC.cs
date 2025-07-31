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
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace ATRC
{
    /// <summary>
    /// Managed wrapper for an ATRC file descriptor that automatically handles memory cleanup.
    /// </summary>
    public class ATRC : IDisposable
    {

        private IntPtr _fd = IntPtr.Zero;
        private bool _disposed;

        /// <summary>
        /// Initializes a new empty ATRC file (no file path associated).
        /// </summary>
        public ATRC()
        {
            _fd = ATRCNative.ATRC_Native.Create_Empty_ATRC_FD();
            if (_fd == IntPtr.Zero)
                throw new InvalidOperationException("Failed to create empty ATRC file descriptor.");
        }

        /// <summary>
        /// Sets whether the ATRC file should automatically save changes.
        /// </summary>
        /// <param name="autoSave">New autosave status</param>
        public void SetAutoSave(bool autoSave)
        {
            if (_fd != IntPtr.Zero)
            {
                ATRCNative.ATRC_FD managedFd = Marshal.PtrToStructure<ATRCNative.ATRC_FD>(_fd);
                managedFd.AutoSave = autoSave;
                Marshal.StructureToPtr(managedFd, _fd, false);
            }
        }

        /// <summary>
        /// Retrieves the current autosave status of the ATRC file.
        /// </summary>
        /// <returns>True if autosave is enabled, false otherwise.</returns>
        public bool GetAutoSave()
        {
            if (_fd != IntPtr.Zero)
            {
                ATRCNative.ATRC_FD managedFd = Marshal.PtrToStructure<ATRCNative.ATRC_FD>(_fd);
                return managedFd.AutoSave;
            }
            return false;
        }

        /// <summary>
        /// Sets whether the ATRC file should perform write checks.
        /// </summary>
        /// <param name="writeCheck">New write check status</param>
        public void Writecheck(bool writeCheck)
        {
            if (_fd != IntPtr.Zero)
            {
                ATRCNative.ATRC_FD managedFd = Marshal.PtrToStructure<ATRCNative.ATRC_FD>(_fd);
                managedFd.Writecheck = writeCheck;
                Marshal.StructureToPtr(managedFd, _fd, false);
            }
        }

        /// <summary>
        /// Retrieves the current write check status of the ATRC file.
        /// </summary>
        /// <returns>True if write check is enabled, false otherwise.</returns>
        public bool Writecheck()
        {
            if (_fd != IntPtr.Zero)
            {
                ATRCNative.ATRC_FD managedFd = Marshal.PtrToStructure<ATRCNative.ATRC_FD>(_fd);
                return managedFd.Writecheck;
            }
            return false;
        }

        /// <summary>
        /// Initializes a new ATRC file using a file path and read mode.
        /// </summary>
        /// <param name="filename">Path to the file.</param>
        /// <param name="mode">Read mode (e.g., ReadOnly, CreateRead, ForceRead).</param>
        public ATRC(string filename, ReadMode mode)
        {
            _fd = ATRCNative.ATRC_Native.Create_ATRC_FD(filename, mode);
            if (_fd == IntPtr.Zero)
                throw new InvalidOperationException($"Failed to create ATRC file descriptor for file '{filename}'.");
        }

        /// <summary>
        /// Reads a variable from the ATRC file.
        /// If the variable does not exist, it returns null.
        /// </summary>
        /// <param name="name">Name of the variable to read</param>
        /// <returns>The value of the variable, or null if it does not exist</returns>
        /// <exception cref="InvalidOperationException"></exception>
        public string? ReadVariable(string name)
        {
            string? res = string.Empty;
            if (_fd != IntPtr.Zero)
            {
                IntPtr ptr = ATRCNative.ATRC_Native.ReadVariable(_fd, name);
                if (ptr != IntPtr.Zero)
                {
                    if (ptr == IntPtr.Zero)
                    {
                        throw new InvalidOperationException($"Failed to read variable '{name}' from ATRC file.");
                    }
                    res = Marshal.PtrToStringUTF8(ptr);
                    ATRC.ATRC_FREE_MEMORY(ptr);
                }

            }
            return res;
        }

        /// <summary>
        /// Reads a key from a specified block in the ATRC file.
        /// If the key does not exist, it returns null.
        /// </summary>
        /// <param name="block">Block the key is located in</param>
        /// <param name="key">Name of the key to read</param>
        /// <returns>The value of the key, or null if it does not exist</returns>
        public string? ReadKey(string block, string key)
        {
            string? res = string.Empty;
            if (_fd != IntPtr.Zero)
            {
                IntPtr ptr = ATRCNative.ATRC_Native.ReadKey(_fd, block, key);
                if (ptr != IntPtr.Zero)
                {
                    res = Marshal.PtrToStringUTF8(ptr);
                    ATRC.ATRC_FREE_MEMORY(ptr);
                }
            }
            return res;
        }

        /// <summary>
        /// Frees the memory allocated by the native ATRC library.
        /// </summary>
        /// <param name="ptr">pointer to allocated memory</param>
        private static void ATRC_FREE_MEMORY(IntPtr ptr)
        {
            if (ptr != IntPtr.Zero)
            {
                ATRCNative.ATRC_Native.ATRC_FREE_MEMORY(ptr);
            }
        }

        /// <summary>
        /// Frees unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (_fd != IntPtr.Zero)
                {
                    ATRCNative.ATRC_Native.Destroy_ATRC_FD(_fd);
                    _fd = IntPtr.Zero;
                }

                _disposed = true;
            }
        }

        /// <summary>
        /// Finalizer to ensure resources are cleaned up if Dispose is not called.
        /// </summary>
        ~ATRC()
        {
            Dispose(disposing: false);
        }

        /// <summary>
        ///  Reads an ATRC file from the specified filename with the given read mode.
        /// </summary>
        /// <param name="filename">The name of the file to read.</param>
        /// <param name="mode">The read mode to use.</param>
        /// <returns>True if the read was successful; otherwise, false.</returns>
        public bool Read(string filename, ReadMode mode)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.Read(_fd, filename, mode);
            }
            return false;
        }

        /// <summary>
        /// Reads a block from the ATRC file.
        /// If the block does not exist, it returns null.
        /// </summary>
        /// <param name="block">Name of the block to read.</param>
        /// <returns>The value of the block, or null if it does not exist.</returns>
        public bool DoesExistBlock(string block)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.DoesExistBlock(_fd, block);
            }
            return false;
        }

        /// <summary>
        /// Checks if a key exists in a specified block of the ATRC file.
        /// If the block does not exist, it returns false.
        /// </summary>
        /// <param name="block">Name of the block to check.</param>
        /// <param name="key">Name of the key to check.</param>
        /// <returns>True if the key exists; otherwise, false.</returns>
        public bool DoesExistKey(string block, string key)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.DoesExistKey(_fd, block, key);
            }
            return false;
        }

        /// <summary>
        /// Checks if a variable exists in the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="name">Name of the variable to check.</param>
        /// <returns>True if the variable exists; otherwise, false.</returns>
        public bool DoesExistVariable(string name)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.DoesExistVariable(_fd, name);
            }
            return false;
        }

        /// <summary>
        /// Checks if a variable is public in the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="varname">Name of the variable to check.</param>
        /// <returns>True if the variable is public; otherwise, false.</returns>
        public bool IsPublic(string varname)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.IsPublic(_fd, varname);
            }
            return false;
        }

        /// <summary>
        /// Inserts a variable into the specified line with the given arguments.
        /// </summary>
        /// <param name="line">Line containing inject markings</param>
        /// <param name="args">Arguments to insert</param>
        /// <returns>New line with variables inserted, or null if the operation failed.</returns>
        public string? InsertVar_S(string line, List<string> args)
        {
            string? resStr = string.Empty;
            if (_fd != IntPtr.Zero)
            {
                IntPtr[] argPtrs = new IntPtr[args.Count + 1];
                for (int i = 0; i < args.Count; i++)
                {
                    argPtrs[i] = Marshal.StringToCoTaskMemUTF8(args[i]);
                }
                argPtrs[args.Count] = IntPtr.Zero; // Null terminator

                IntPtr argArrayPtr = Marshal.AllocCoTaskMem(IntPtr.Size * argPtrs.Length);
                Marshal.Copy(argPtrs, 0, argArrayPtr, argPtrs.Length);

                IntPtr resPtr = ATRCNative.ATRC_Native.InsertVar_S(line, argArrayPtr);
                resStr = Marshal.PtrToStringUTF8(resPtr);

                // Free all allocated memory
                foreach (var ptr in argPtrs)
                {
                    if (ptr != IntPtr.Zero)
                        Marshal.FreeCoTaskMem(ptr);
                }
                Marshal.FreeCoTaskMem(argArrayPtr);
                ATRC_FREE_MEMORY(resPtr);

            }
            return resStr;
        }

        /// <summary>
        /// Adds a new block to the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="block">Name of the block to add.</param>
        /// <returns>True if the block was added successfully; otherwise, false.</returns>
        public bool AddBlock(string block)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.AddBlock(_fd, block);
            }
            return false;
        }

        /// <summary>
        /// Removes a block from the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="block">Name of the block to remove.</param>
        /// <returns>True if the block was removed successfully; otherwise, false.</returns>
        public bool RemoveBlock(string block)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.RemoveBlock(_fd, block);
            }
            return false;
        }

        /// <summary>
        /// Adds a new variable to the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="varname">Name of the variable to add.</param>
        /// <param name="value">Value of the variable to add.</param>
        /// <returns>True if the variable was added successfully; otherwise, false.</returns>
        public bool AddVariable(string varname, string value)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.AddVariable(_fd, varname, value);
            }
            return false;
        }

        /// <summary>
        /// Removes a variable from the ATRC file.
        /// If the variable does not exist, it returns false.
        /// </summary>
        /// <param name="varname">Variable name.</param>
        /// <returns>True if the variable was removed successfully; otherwise, false.</returns>
        public bool RemoveVariable(string varname)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.RemoveVariable(_fd, varname);
            }
            return false;
        }

        /// <summary>
        /// Modifies the value of an existing variable in the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="varname">Name of the variable to modify.</param>
        /// <param name="newValue">New value for the variable.</param>
        /// <returns>True if the variable was modified successfully; otherwise, false.</returns>
        public bool ModifyVariable(string varname, string newValue)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.ModifyVariable(_fd, varname, newValue);
            }
            return false;
        }

        /// <summary>
        /// Adds a new key to a specified block in the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="block">Name of the block to add the key to.</param>
        /// <param name="key">Name of the key to add.</param>
        /// <param name="value">Value of the key to add.</param>
        /// <returns>True if the key was added successfully; otherwise, false.</returns>
        public bool AddKey(string block, string key, string value)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.AddKey(_fd, block, key, value);
            }
            return false;
        }

        /// <summary>
        /// Removes a key from a specified block in the ATRC file.
        /// If the block or key does not exist, it returns false.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="block"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool RemoveKey(string block, string key)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.RemoveKey(_fd, block, key);
            }
            return false;
        }

        /// <summary>
        /// Modifies the value of an existing key in a specified block in the ATRC file.
        /// If the block or key does not exist, it returns false.
        /// </summary>
        /// <param name="block">Block name.</param>
        /// <param name="key">Key name.</param>
        /// <param name="newValue">New value for the key.</param>
        /// <returns>True if the key was modified successfully; otherwise, false.</returns>
        public bool ModifyKey(string block, string key, string newValue)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.ModifyKey(_fd, block, key, newValue);
            }
            return false;
        }

        /// <summary>
        /// Writes a comment to the top of the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="comment">Comment to write.</param>
        /// <returns>True if the comment was written successfully; otherwise, false.</returns>
        public bool WriteCommentToTop(string comment)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.WriteCommentToTop(_fd, comment);
            }
            return false;
        }

        /// <summary>
        /// Writes a comment to the bottom of the ATRC file.
        /// If the file descriptor is invalid, it returns false.
        /// </summary>
        /// <param name="comment">Comment to write.</param>
        /// <returns>True if the comment was written successfully; otherwise, false.</returns>
        public bool WriteCommentToBottom(string comment)
        {
            if (_fd != IntPtr.Zero)
            {
                return ATRCNative.ATRC_Native.WriteCommentToBottom(_fd, comment);
            }
            return false;
        }

        /// <summary>
        /// Retrieves the value of an enum from a specified block and key in the ATRC file.
        /// If the block or key does not exist, it returns -1/ulong.MaxValue. 
        /// </summary>
        /// <param name="block"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public ulong GetEnumValue(string block, string key)
        {
            ulong value = ulong.MaxValue; // Default value if not found
            if (_fd != IntPtr.Zero)
            {
                value = ATRCNative.ATRC_Native.GetEnumValue(_fd, block, key);
            }
            return value;
        }

        /// <summary>
        /// Copies data from one ATRC file descriptor to another.
        /// This method is used to create a copy of an ATRC file descriptor. 
        /// </summary>
        /// <param name="fd">File descriptor to copy.</param>
        /// <returns>New ATRC file descriptor. Null if the copy failed.</returns>
        public static ATRC? Copy_ATRC_FD(ATRC fd)
        {
            if (fd == null || fd._fd == IntPtr.Zero)
                return null;

            IntPtr newFd = ATRCNative.ATRC_Native.Copy_ATRC_FD(fd._fd);
            if (newFd == IntPtr.Zero)
                return null;

            return new ATRC(newFd);
        }

        /// <summary>
        /// Creates a new ATRC instance from an existing file descriptor.
        /// This constructor is used internally to create an ATRC instance from a native file descriptor.
        /// </summary>
        /// <param name="fd">File descriptor to wrap.</param>
        /// <exception cref="InvalidOperationException">Invalid file descriptor.</exception>
        private ATRC(IntPtr fd)
        {
            if (_fd != IntPtr.Zero)
            {
                Dispose();
            }
            _fd = fd;
            if (_fd == IntPtr.Zero)
                throw new InvalidOperationException("Failed to create ATRC file descriptor from IntPtr.");
        }
    }


    /// <summary>
    ///  ATRC Standard Library for common operations.
    /// </summary>
    public static class ATRC_STD
    {
        /// <summary>
        /// Converts a string to a list of strings, splitting by the specified separator.
        /// If the input string is null or empty, it returns an empty list.
        /// </summary>
        /// <param name="value">Value to split.</param>
        /// <param name="separator">Character used to split the string.</param>
        /// <returns>List of split strings.</returns>
        public static List<string> ToList(string value, char separator = ',')
        {
            if (string.IsNullOrEmpty(value))
                return new List<string>();
            List<string> result = new List<string>();
            foreach (var item in value.Split(separator))
            {
                result.Add(item.Trim());
            }
            return result;
        }

        /// <summary>
        /// Converts a string to a boolean value.
        /// If the string cannot be converted, it returns false.
        /// Accepts "true", "false", "on", "off", "1", "0", and case-insensitive variations.
        /// </summary>
        /// <param name="value">A string representing a boolean value.</param>
        /// <returns>A boolean value.</returns>
        public static bool ToBool(string value)
        {
            return ATRCNative.ATRC_Native.atrc_to_bool(value);
        }

        /// <summary>
        /// Converts a string to an unsigned long integer.
        /// If the string cannot be converted, it returns 0.
        /// </summary>
        /// <param name="value">A string representing an unsigned long integer.</param>
        /// <returns>An unsigned long integer.</returns>
        public static ulong ToULong(string value)
        {
            return ATRCNative.ATRC_Native.atrc_to_uint64_t(value);
        }

        /// <summary>
        /// Converts a string to a long integer.
        /// If the string cannot be converted, it returns 0.
        /// </summary>
        /// <param name="value">A string representing a long integer.</param>
        /// <returns>A long integer.</returns>
        public static long ToLong(string value)
        {
            return ATRCNative.ATRC_Native.atrc_to_int64_t(value);
        }

        /// <summary>
        /// Converts a string to a double.
        /// If the string cannot be converted, it returns 0.0.
        /// </summary>
        /// <param name="value">A string representing a double.</param>
        /// <returns>A double.</returns>
        public static double ToDouble(string value)
        {
            return ATRCNative.ATRC_Native.atrc_to_double(value);
        }

        /// <summary>
        /// Parses a mathematical expression from a string and returns the result as a double.
        /// If the expression is invalid, it returns NaN.
        /// </summary>
        /// <param name="value">Mathematical expression to parse.</param>
        /// <returns>Result of the expression or NaN if invalid.</returns>
        public static double ParseMathExp(string value)
        {
            return ATRCNative.ATRC_Native.atrc_math_exp(value);
        }
    }
}
