/*+++
This project is licensed under the BSD 2-Clause License.
See the LICENSE file in the project root for license information.

Header file for the ATRC (Advanced Tagged Resource Configuration Library) library.
  This file contains the declarations for the ATRC library, which provides functionality
for reading, writing, and manipulating configuration files in a structured format.
  This library is designed to be used in both C and C++ projects, with a focus on
ease of use and flexibility.

Author(s): 
    Antonako1

Maintained at https://github.com/Antonako1/ATRC
---*/

#include <ATRC/ATRC.h>
#ifndef ATRC_HPP
#define ATRC_HPP
#ifdef __cplusplus

#include <string>
#include <vector>
#include <cstring>
#include <iostream>

// __STRDUP macro used for duplicating strings
#ifdef WIN32
#define __STRDUP(src) \
    _strdup(src)
#else
#define __STRDUP(src) \
    strdup(src)
#endif

namespace atrc {

    class _ATRC_FD;
    typedef _ATRC_FD* _PATRC_FD;


    /// @brief Proxy class for accessing keys in the ATRC_FD.
    /// @note This class allows for easy access to keys and variables using different operators.
    /// @example:
    /// @code
    /// _ATRC_FD fd("config.atrc");
    ///
    /// fd["block"]["key"] = "value"; // Sets the value of the key in the block
    /// std::cout << fd["block"]["key"]; // Gets the value of the key
    ///
    /// fd["var"] = "value"; // Sets the value of the variable
    /// std::cout << fd["var"]; // Gets the value of the variable
    ///
    /// fd["block"]["key"] >> "new_value"; // Append a new value to the key
    /// @endcode
    /// @warning If []-operator returns const char*, it must be freed with free()
    class PROXY_ATRC_FD {
        public:
            PROXY_ATRC_FD(_ATRC_FD& fd, const std::string& key);
            PROXY_ATRC_FD operator[](const std::string& subKey);
            operator std::string() const;
            operator const char*() const;
            PROXY_ATRC_FD& operator=(const std::string& value);
            PROXY_ATRC_FD& operator>>(const std::string& value);
            PROXY_ATRC_FD& operator>>(const char* value);
            friend std::ostream& operator<<(std::ostream& os, const PROXY_ATRC_FD& obj);
        private:
            _PATRC_FD fd;
            std::string key;
    };

    /// @brief C++ wrapper class for ATRC_FD.
    /// @note All methods are prefixed with an underscore to avoid conflicts with C functions.
    /// @note This class manages the ATRC_FD pointer and provides a C++ interface for the ATRC library.
    /// @note The class will automatically manage all memory and resources associated with the ATRC_FD.
    class _ATRC_FD {
    public:

        /// @brief Default constructor that creates an empty _ATRC_FD.
        _ATRC_FD() {
            m_fd = Create_Empty_ATRC_FD();
        }

        /// @brief Constructor that creates an _ATRC_FD from a file path.
        /// @param path The path to the file.
        /// @param mode The read mode (default is read-only).
        _ATRC_FD(const std::string& path, ReadMode mode = ATRC_READ_ONLY) {
            if(m_fd != nullptr) {
                m_fd = Create_ATRC_FD(path.c_str(), mode);
            } else {
                Read(m_fd, path.c_str(), mode);
            }
        }

        /// @brief Constructor that creates an _ATRC_FD from a file path.
        /// @param path The path to the file.
        /// @param mode The read mode (default is read-only).
        _ATRC_FD(const char* path, ReadMode mode = ATRC_READ_ONLY) {
            if(m_fd != nullptr) {
                m_fd = Create_ATRC_FD(path, mode);
            } else {
                Read(m_fd, path, mode);
            }
        }

        /// @brief Constructor that creates an ATRC_FD from an existing ATRC_FD.
        /// @param filedata The existing ATRC_FD to copy.
        /// @note This does copy the file data, so changes to the original ATRC_FD will affect this one.
        /// @note If you attach an ATRC_FD to this one and the class goes out of scope, it will destroy the original ATRC_FD too.
        /// @note If you want to copy the file data, use Copy() method.
        _ATRC_FD(PATRC_FD filedata) {
            m_fd = filedata;
        }

        /// @brief Destructor that cleans up the ATRC_FD.
        ~_ATRC_FD() {
            _Destroy();
        }

        /// @brief Destroys the ATRC_FD and cleans up resources.
        void _Destroy() {
            if (m_fd != nullptr) {
                Destroy_ATRC_FD(m_fd);
                m_fd = nullptr;
            }
        }

        /// @brief Returns the file data structures pointer.
        /// @return The file data structures pointer.
        /// @note This does not copy the file data
        /// @note If the class goes out of scope, it will destroy the pointer returned by this.
        /// @note If you want to copy the file data, use the GetCopy() method.
        PATRC_FD _ToCStruct() {
            return m_fd;
        }

        /// @brief Copies the file data from another ATRC_FD into this class.
        /// @param filedata The source ATRC_FD to copy from.
        /// @note Creates a deep copy of the file data.
        /// @note The copy will not be destroyed when this class goes out of scope.
        /// @note If the class already has an ATRC_FD, it will destroy it before copying.
        void _Copy(PATRC_FD filedata) {
            if (m_fd != nullptr) {
                Destroy_ATRC_FD(m_fd);
            }
            m_fd = Copy_ATRC_FD(filedata);
        }

        /// @brief Creates a copy of the file data.
        /// @return A copy of the file data. _ATRC_FD() on failure with filename as "ERROR"
        /// @note This creates a new ATRC_FD with the same data as the original.
        /// @note The original ATRC_FD will not be destroyed when this class goes out of scope.
        _ATRC_FD GetCopy() {
            PATRC_FD copy = Copy_ATRC_FD(m_fd);
            if (copy == nullptr) {
                _ATRC_FD res = _ATRC_FD();
                res._SetFilename("ERROR");
                return res;
            }
            return _ATRC_FD(copy);
        }

        /// @brief Nullifies the ATRC_FD pointer.
        /// @warning Use with caution. This does NOT destroy the ATRC_FD, it just nullifies the pointer to the ATRC_FD struct!
        void _Nullify_PATRC_FD() {
            m_fd = nullptr;
        }

        /// @brief Reads the file again with the specified read mode.
        /// @param mode The read mode (default is read-only).
        /// @return True if successful, false otherwise.
        bool _ReadAgain(ReadMode mode = ATRC_READ_ONLY) {
            if (m_fd != nullptr) {
                if(m_fd->Filename == nullptr) {
                    return false;
                }
                return Read(m_fd, m_fd->Filename, mode);
            }
            return false;
        }

        /// @brief Reads a file.
        /// @param path The path to the file.
        /// @param mode The read mode (default is read-only).
        /// @return True if successful, false otherwise.
        bool _Read(std::string& path, ReadMode mode = ATRC_READ_ONLY) {
            if (m_fd != nullptr) {
                return Read(m_fd, path.c_str(), mode);
            }
            return false;
        }

        /// @brief Reads a variable from the configuration.
        /// @param varname The name of the variable to read.
        /// @return The value of the variable, or an empty string if not found.
        std::string _ReadVariable(const std::string& varname) {
            if (m_fd != nullptr) {
                char* value = ReadVariable(m_fd, varname.c_str());
                std::string result(value);
                ATRC_FREE_MEMORY(value);
                return result;
            }
            return "";
        }

        /// @brief Reads a key from a block.
        /// @param block The block name.
        /// @param key The key name.
        /// @return The value of the key, or an empty string if not found.
        std::string _ReadKey(const std::string& block, const std::string& key) {
            if (m_fd != nullptr) {
                char* value = ReadKey(m_fd, block.c_str(), key.c_str());
                std::string result(value);
                ATRC_FREE_MEMORY(value);
                return result;
            }
            return "";
        }

        /// @brief Checks if a block exists.
        /// @param block The block name.
        /// @return True if the block exists, false otherwise.
        bool _DoesExistBlock(const std::string& block) {
            if (m_fd != nullptr) {
                return DoesExistBlock(m_fd, block.c_str());
            }
            return false;
        }

        /// @brief Checks if a variable exists.
        /// @param varname The variable name.
        /// @return True if the variable exists, false otherwise.
        bool _DoesExistVariable(const std::string& varname) {
            if (m_fd != nullptr) {
                return DoesExistVariable(m_fd, varname.c_str());
            }
            return false;
        }

        /// @brief Checks if a key exists in a block.
        /// @param block The block name.
        /// @param key The key name.
        /// @return True if the key exists, false otherwise.
        bool _DoesExistKey(const std::string& block, const std::string& key) {
            if (m_fd != nullptr) {
                return DoesExistKey(m_fd, block.c_str(), key.c_str());
            }
            return false;
        }

        /// @brief Checks if a variable is public.
        /// @param varname The variable name to check.
        /// @return True if the variable is public, false otherwise.
        bool _IsPublic(const std::string& varname) {
            if (m_fd != nullptr) {
                return IsPublic(m_fd, varname.c_str());
            }
            return false;
        }

        /// @brief Inserts variables into a string.
        /// @param line The string containing inject markings to insert variables into.
        /// @param args The variables to insert.
        /// @return True if successful, false otherwise.
        /// @note Even though in C, args needs to be NULL-terminated, in C++ it is not required.
        bool _InsertVar(std::string& line, std::vector<std::string>& args) {
            if (m_fd != nullptr) {
                std::string result = _InsertVar_S(line, args);
                line = result;
                return true;
            }
            return false;
        }
        
        /// @brief Inserts variables into a string.
        /// @param line The string containing inject markings to insert variables into.
        /// @param args The variables to insert.
        /// @return The resulting string with variables inserted.
        /// @note Even though in C, args needs to be NULL-terminated, in C++ it is not required.
        std::string _InsertVar_S(const std::string& line, std::vector<std::string>& args) {
            if (m_fd != nullptr) {
                const char** args_cstr = new const char*[args.size() + 1];
                for (size_t i = 0; i < args.size(); ++i) {
                    args_cstr[i] = args[i].c_str();
                }
                args_cstr[args.size()] = nullptr;

                char* result = InsertVar_S(line.c_str(), args_cstr);
                delete[] args_cstr;

                std::string result_str(result);
                ATRC_FREE_MEMORY(result);
                return result_str;
            }
            return "";
        }


        /// @brief Adds a block to the ATRC_FD.
        /// @param blockname The block name.
        /// @return True if successful, false otherwise.
        bool _AddBlock(const std::string& blockname) {
            if (m_fd != nullptr) {
                return AddBlock(m_fd, blockname.c_str());
            }
            return false;
        }

        /// @brief Removes a block from the ATRC_FD.
        /// @param blockname The block name.
        /// @return True if successful, false otherwise.
        bool _RemoveBlock(const std::string& blockname) {
            if (m_fd != nullptr) {
                return RemoveBlock(m_fd, blockname.c_str());
            }
            return false;
        }

        /// @brief Adds a variable to the ATRC_FD.
        /// @param varname The variable name.
        /// @param value The variable value.
        /// @return True if successful, false otherwise.
        bool _AddVariable(const std::string& varname, const std::string& value) {
            if (m_fd != nullptr) {
                return AddVariable(m_fd, varname.c_str(), value.c_str());
            }
            return false;
        }

        /// @brief Removes a variable.
        /// @param varname The variable name.
        /// @return True if successful, false otherwise.
        bool _RemoveVariable(const std::string& varname) {
            if (m_fd != nullptr) {
                return RemoveVariable(m_fd, varname.c_str());
            }
            return false;
        }

        /// @brief Modifies a variable.
        /// @param varname The variable name.
        /// @param value The new value.
        /// @return True if successful, false otherwise.
        bool _ModifyVariable(const std::string& varname, const std::string& value) {
            if (m_fd != nullptr) {
                return ModifyVariable(m_fd, varname.c_str(), value.c_str());
            }
            return false;
        }

        /// @brief Adds a key to a block.
        /// @param block The block name.
        /// @param key The key name.
        /// @param value The key value.
        /// @return True if successful, false otherwise.
        bool _AddKey(const std::string& block, const std::string& key, const std::string& value) {
            if (m_fd != nullptr) {
                return AddKey(m_fd, block.c_str(), key.c_str(), value.c_str());
            }
            return false;
        }

        /// @brief Removes a key from a block.
        /// @param block The block name.
        /// @param key The key name.
        /// @return True if successful, false otherwise.
        bool _RemoveKey(const std::string& block, const std::string& key) {
            if (m_fd != nullptr) {
                return RemoveKey(m_fd, block.c_str(), key.c_str());
            }
            return false;
        }

        /// @brief Modifies a key in a block.
        /// @param block The block name.
        /// @param key The key name.
        /// @param value The new value.
        /// @return True if successful, false otherwise.
        bool _ModifyKey(const std::string& block, const std::string& key, const std::string& value) {
            if (m_fd != nullptr) {
                return ModifyKey(m_fd, block.c_str(), key.c_str(), value.c_str());
            }
            return false;
        }

        /// @brief Writes a comment to the top of the file.
        /// @param comment The comment to write.
        /// @return True if successful, false otherwise.
        bool _WriteCommentToTop(const std::string& comment) {
            if (m_fd != nullptr) {
                return WriteCommentToTop(m_fd, comment.c_str());
            }
            return false;
        }

        /// @brief Writes a comment to the bottom of the file.
        /// @param comment The comment to write.
        /// @return True if successful, false otherwise.
        bool _WriteCommentToBottom(const std::string& comment) {
            if (m_fd != nullptr) {
                return WriteCommentToBottom(m_fd, comment.c_str());
            }
            return false;
        }

        /// @brief Gets the enum value for a specific block and key.
        /// @param block The block name.
        /// @param key The key name.
        /// @return The enum value.
        uint64_t _GetEnumValue(const std::string& block, const std::string& key) {
            return GetEnumValue(m_fd, block.c_str(), key.c_str());
        }

        /// @brief Checks the status of the ATRC_FD.
        /// @return True if the ATRC_FD is valid, false otherwise.
        bool _CheckStatus() {
            return m_fd != nullptr;
        }

        /// @brief Gets the filename of the ATRC_FD.
        /// @return The filename of the ATRC_FD.
        std::string _GetFilename() {
            if (m_fd != nullptr && m_fd->Filename != nullptr) {
                return std::string(m_fd->Filename);
            }
            return "";
        }

        /// @brief Gets the auto-save status of the ATRC_FD.
        /// @return True if auto-save is enabled, false otherwise.
        bool _GetAutoSave() const {
            return m_fd != nullptr && m_fd->AutoSave;
        }

        /// @brief Sets the auto-save status of the ATRC_FD.
        /// @param autosave True to enable auto-save, false to disable it.
        void _SetAutoSave(bool autosave) {
            if (m_fd != nullptr) {
                m_fd->AutoSave = autosave;
            }
        }

        /// @brief Gets the write check status of the ATRC_FD.
        /// @return True if write check is enabled, false otherwise.
        bool _GetWriteCheck() const {
            return m_fd != nullptr && m_fd->Writecheck;
        }

        /// @brief Sets the write check status of the ATRC_FD.
        /// @param writecheck True to enable write check, false to disable it.
        void _SetWriteCheck(bool writecheck) {
            if (m_fd != nullptr) {
                m_fd->Writecheck = writecheck;
            }
        }

        /// @brief Gets the filename of the ATRC_FD.
        /// @return The filename of the ATRC_FD.
        std::string _GetFilename() const {
            if (m_fd != nullptr && m_fd->Filename != nullptr) {
                return std::string(m_fd->Filename);
            }
            return "";
        }

        /// @brief Gets a proxy for a specific key in the ATRC_FD.
        /// @param key The key to access.
        /// @return A proxy for the specified key.
        /// @note This allows for easy access to keys and variables using the [] operator.
        /// @note See the PROXY_ATRC_FD class for more details on different operators aswell and their usage.
        PROXY_ATRC_FD operator[](const std::string& key) {
            return PROXY_ATRC_FD(*this, key);
        }

        /// @brief Gets a proxy for a specific key in the ATRC_FD.
        /// @param key The key to access.
        /// @return A proxy for the specified key.
        /// @note This allows for easy access to keys and variables using the [] operator.
        /// @note See the PROXY_ATRC_FD class for more details on different operators aswell and their usage.
        PROXY_ATRC_FD operator[](const std::string& key) const {
            return PROXY_ATRC_FD(const_cast<_ATRC_FD&>(*this), key);
        }
    private:
        /// @brief Private method. Used only internally. This COULD corrupt HEAP memory if used incorrectly.
        /// @param filename New filename to set.
        void _SetFilename(const std::string& filename) {
            if (m_fd != nullptr) {
                if (m_fd->Filename != nullptr) {
                    ATRC_FREE_MEMORY(m_fd->Filename);
                }
                m_fd->Filename = __STRDUP(filename.c_str());
            }
        }
        PATRC_FD m_fd;
    };

    typedef _ATRC_FD* _PATRC_FD;


    /*+++
    
    PROXY_ATRC_FD class method implementations.
    
    ---*/
    PROXY_ATRC_FD::PROXY_ATRC_FD(_ATRC_FD& fd, const std::string& key) {
        this->fd = &fd;
        this->key = key;
    }
    PROXY_ATRC_FD PROXY_ATRC_FD::operator[](const std::string& subKey) {
        std::string combined_key = key + "]" + subKey;
        PROXY_ATRC_FD res = PROXY_ATRC_FD(*fd, combined_key);
        if (res == 0) {
            std::cerr << "312_Error: PROXY_ATRC_FD is null." << std::endl;
        }
        return res;
    }
    PROXY_ATRC_FD::operator std::string() const {
        uint64_t x = key.find("]");
        try {
            if (x == std::string::npos) {
                return fd->_ReadVariable(key);
            }
            else {
                std::string block = key.substr(0, x);
                std::string key_ = key.substr(x + 1, key.size() - x - 1);
                return fd->_ReadKey(block, key_);
            }
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return "";
    }
    PROXY_ATRC_FD::operator const char*() const {
        uint64_t x = key.find("]");
        try {
            if (x == std::string::npos) {
                std::string res_str = fd->_ReadVariable(key);
                const char *res = __STRDUP(res_str.c_str());
                if (!res) {
                    std::cerr << "513_Memory allocation failed for PROXY_ATRC_FD." << std::endl;
                    return "";
                }
                return res;
            }
            else {
                std::string block = key.substr(0, x);
                std::string key_ = key.substr(x + 1, key.size() - x - 1);
                std::string res_str = fd->_ReadKey(block, key_);
                if(res_str.empty()) {
                    return "";
                }
                const char *res = __STRDUP(res_str.c_str());
                if(!res) {
                    std::cerr << "512_Memory allocation failed for PROXY_ATRC_FD." << std::endl;
                    return "";
                }
                return res;
            }
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return "";
    }
    PROXY_ATRC_FD& PROXY_ATRC_FD::operator=(const std::string& value) {
        uint64_t x = key.find("]");
        try {
            if (x == std::string::npos) {
                if(fd->_GetWriteCheck() || fd->_DoesExistVariable(key)) {
                    fd->_ModifyVariable(key, value);
                }
            }
            else {
                std::string block = key.substr(0, x);
                std::string key_ = key.substr(x + 1, key.size() - x - 1);
                if(fd->_GetWriteCheck() || fd->_DoesExistKey(block, key_)) {
                    fd->_ModifyKey(block, key_, value);
                }
            }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return *this;
    }
    PROXY_ATRC_FD& PROXY_ATRC_FD::operator>>(const std::string& value) {
        uint64_t x = key.find("]");
        if (x == std::string::npos) {
            std::string existing = fd->_ReadVariable(key);
            fd->_ModifyVariable(key, existing + value);
        } else {
            std::string block = key.substr(0, x);
            std::string key_ = key.substr(x + 1, key.size() - x - 1);
            std::string existing = fd->_ReadKey(block, key_);
            fd->_ModifyKey(block, key_, existing + value);
        }
        return *this;
    }
    PROXY_ATRC_FD& PROXY_ATRC_FD::operator>>(const char* value) {
        return operator>>(std::string(value));
    }

    std::ostream& operator<<(std::ostream& os, const PROXY_ATRC_FD& obj) {
        uint64_t x = obj.key.find("]");
        if (x == std::string::npos)
            os << obj.fd->_ReadVariable(obj.key);
        else {
            std::string block = obj.key.substr(0, x);
            std::string key_ = obj.key.substr(x + 1);
            os << obj.fd->_ReadKey(block, key_);
        }
        return os;
    }

} // namespace atrc

#endif
#endif // ATRC_HPP