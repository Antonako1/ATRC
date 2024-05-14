using System.Text.RegularExpressions;
namespace ATRC
{
    public partial class Filehandler{

        public struct ATRCVariable{
            public string Name;
            public string Value;
            public string[] ArrayValues;
            public bool IsArray;
            public bool IsPrivate; // Used only when reading in ATRCFileData class
            public ATRCVariable(){
                IsPrivate = false;
                Name = "";
                Value = "";
                ArrayValues = new string[0];
                IsArray = false;
            }
        }
        public struct ATRCKey{
            public string Name;
            public string Value;
            public string[] ArrayValues;
            public bool IsArray;
        }
        public class ATRCBlock{
            public string Name;
            public ATRCKey[] Keys;
        }
        public partial class ATRCFileData{
            public string Filename { get; set; } = "";
            public ATRCVariable[] Variables { get; set; } = [];
            public ATRCBlock[] Blocks { get; set; } = [];

            // ==========================
            //  Block functions
            // ==========================
            /// <summary>
            /// Create a new block, will be appended to the end of the file
            /// </summary>
            /// <param name="block">block to be added</param>
            /// <exception cref="System.IO.IOException">block already exists</exception>
            public void AddBlock(string block){
                if(block != null && block.Contains('%') || block.Contains('!') || block.Contains(','))
                        throw new System.IO.IOException("Block name cannot contain reserved characters (%, !, ,)");
                if(Blocks.Any(x => x.Name == block))
                    throw new System.IO.IOException("Block " + block + " already exists");
                // Created block will be appended to the end of the file
                // We also have to check if the block already exists

                // Create a new block
                ATRCBlock newBlock = new ATRCBlock();
                newBlock.Name = block;
                newBlock.Keys = []; // Initialize Keys array
                var tempList = Blocks.ToList();
                tempList.Add(newBlock);
                Blocks = tempList.ToArray();

                this.SaveToFile(block, null, null, 0);
            }

            /// <summary>
            /// Removes the given block from the file
            /// </summary>
            /// <param name="block">block to be removed</param>
            public void RemoveBlock(string block){
                // Check if the block exists
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                ATRCBlock _key = Blocks.First(x => x.Name == block);
                int idx = Array.IndexOf(Blocks, _key);
                List<ATRCBlock> tmp = [.. Blocks];
                tmp.RemoveAt(idx);
                Blocks = tmp.ToArray();
                this.SaveToFile(block, null, null, 1);
            }
            /// <summary>
            /// Checks if block exists
            /// </summary>
            /// <param name="block">Block to search</param>
            /// <returns>True if exists</returns>
            public bool BlockExists(string block){
                if (!Blocks.Any(x => x.Name == block)){
                    return false;
                }
                return true;
            }
            // ==========================
            //  Variable functions
            // ==========================
            /// <summary>
            /// Reads all variables from the file, public and private
            /// </summary>
            /// <returns>ATRCVariable[]</returns>
            private ATRCVariable[] ReadVariables(){
                List<ATRCVariable> variables = [];
                int index = 0;
                foreach (string _line in File.ReadLines(this.Filename)) {
                    index++;
                    string _line_trim = _line.Trim(); // Trim the line
                    if(_line_trim.StartsWith('!')) continue; // Skip lines that are comments
                    // Variable definition found
                    if(_line_trim.StartsWith('%') || _line_trim.StartsWith("<%")){
                        int _found_index = _line_trim.IndexOf('='); // Find the index of the = character
                        if (_found_index == -1) throw new System.IO.IOException("Invalid variable definition at line " + index);

                        string _variable_name = "";
                        string _value_string = "";
                        string[] _value_string_array = [];
                        if(_line_trim.StartsWith("<%")){ // Check if the variable is private
                            _variable_name = _line_trim.AsSpan(2, _line_trim.IndexOf('%', 2) - 2).ToString(); // No idea why this works with -2
                        } else { // If the variable is public
                            _variable_name = _line_trim.AsSpan(1, _line_trim.IndexOf('%', 1) - 1).ToString();
                        }

                        object result = ParserValue(_line, index);
                        if(result is string){
                            _value_string = (string)result;
                        }
                        else if(result is string[]){
                            _value_string_array = (string[])result;
                        }
                        else throw new System.IO.IOException("Invalid variable declaration - Value type not recognized at line " + index);

                        ATRCVariable newVariable = new ATRCVariable();

                        newVariable.Name = _variable_name;
                        newVariable.Value = _value_string;
                        newVariable.ArrayValues = _value_string_array;
                        newVariable.IsArray = _value_string_array.Length > 0;
                        newVariable.IsPrivate = _line_trim.StartsWith("<%");
                        variables.Add(newVariable);
                    } else continue;
                }
                return variables.ToArray();
            }

            /// <summary>
            /// Add a variable to the file
            /// </summary>
            /// <param name="name">Variable name</param>
            /// <param name="value">Variable value</param>
            /// <exception cref="System.IO.IOException"></exception>
            public void AddVariable(string name, object value){
                if(name != null && name.Contains('%') || name.Contains('!') || name.Contains(','))
                    throw new System.IO.IOException("Key or variable name cannot contain reserved characters (%, !, ,)");
                // First, we will check if the given variable exists
                ATRCVariable[] _variables = this.ReadVariables();
                if (_variables.Any(x => x.Name == name)){
                    throw new System.IO.IOException("Variable " + name + " already exists");
                }

                string _value = ObjectToSTR(value); // This will be saved to the file
                // from this point onwards, it will be parsed. Arrays will look like this %var1%,%var2%,te\,st
                string _parse_result = _value;
                if(ParseToATRC_vars.Count > 0){
                    _variables = this.ReadVariables(); // We need to read the variables
                    string[] __tmp = ParseToATRC_vars.ToArray();
                    bool array = false;
                    foreach(string var in __tmp){
                        if(!_variables.Any(x => x.Name == var)){
                            throw new System.IO.IOException("Variable " + var + " does not exist");
                        }
                        if(_variables.Any(x => x.Name  == var && x.IsPrivate)){
                            throw new System.IO.IOException("Variable " + var + " is private and cannot be accessed");
                        }
                        ATRCVariable _variable__new = _variables.First(x => x.Name == var);
                        if(_variable__new.IsArray){
                            _parse_result = _parse_result.Replace("%"+var+"%", string.Join(",", _variable__new.ArrayValues));
                            array = true;
                        } else {
                            _parse_result = _parse_result.Replace("%"+var+"%", _variable__new.Value);
                        }
                    }
                }

                ATRCVariable _variable__ = new();
                if(value is string) {
                    _variable__.Value = _parse_result;
                    _variable__.IsArray = false;
                }
                else if(value is string[]){
                    _variable__.ArrayValues = Regex.Split(_parse_result, @"(?<!\\),");
                    _variable__.IsArray = true;
                }
                else
                    throw new System.IO.IOException("Value must be a string or a string array");

                _variable__.Name = name;
                // _variable__.IsArray = _variable__.ArrayValues.Length > 0;
                

                List<ATRCVariable> tmp = [.. Variables];
                tmp.Add(_variable__);
                Variables = tmp.ToArray();

                this.SaveToFile(null, name, _value, 2, false);
            }
            /// <summary>
            /// Read array variable
            /// </summary>
            /// <param name="name">Variable name</param>
            /// <returns>string[]</returns>
            /// <exception cref="System.IO.IOException"></exception>
            public string[] A_ReadVariable(string name){
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    throw new System.IO.IOException("Variable " + name + " does not exist");
                }
                if(_variables.Any(x => x.Name  == name && x.IsPrivate)){
                    throw new System.IO.IOException("Variable " + name + " is private and cannot be modified");
                }

                return _variables.First(x => x.Name == name).ArrayValues;
            }
            /// <summary>
            /// Read string variable
            /// </summary>
            /// <param name="name">Variable name</param>
            /// <returns>string</returns>
            /// <exception cref="System.IO.IOException"></exception>
            public string S_ReadVariable(string name){
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    throw new System.IO.IOException("Variable " + name + " does not exist");
                }
                if(_variables.Any(x => x.Name  == name && x.IsPrivate)){
                    throw new System.IO.IOException("Variable " + name + " is private and cannot be modified");
                }

                return _variables.First(x => x.Name == name).Value;
            }

            /// <summary>
            /// Modifies the given variable with the given value
            /// </summary>
            /// <param name="name">Variable name</param>
            /// <param name="value">New value, string or string[]</param>
            /// <exception cref="System.IO.IOException">Will throw exception if variable does not exist, or is private</exception>
            public void ModifyVariable(string name, object value){
                // First, we will check if the given variable exists
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    throw new System.IO.IOException("Variable " + name + " does not exist");
                }
                if(_variables.Any(x => x.Name  == name && x.IsPrivate)){
                    throw new System.IO.IOException("Variable " + name + " is private and cannot be modified");
                }

                ATRCVariable _variable__ = _variables.First(x => x.Name == name);
                string _value = ObjectToSTR(value); // This will be saved to the file
                // from this point onwards, it will be parsed. Arrays will look like this %var1%,%var2%,te\,st
                string _parse_result = _value;
                if(ParseToATRC_vars.Count > 0){
                    _variables = this.ReadVariables(); // We need to read the variables
                    string[] __tmp = ParseToATRC_vars.ToArray();
                    bool array = false;
                    foreach(string var in __tmp){
                        if(!_variables.Any(x => x.Name == var)){
                            throw new System.IO.IOException("Variable " + var + " does not exist");
                        }
                        if(_variables.Any(x => x.Name  == var && x.IsPrivate)){
                            throw new System.IO.IOException("Variable " + var + " is private and cannot be accessed");
                        }
                        ATRCVariable _variable__new = _variables.First(x => x.Name == var);
                        if(_variable__new.IsArray){
                            _parse_result = _parse_result.Replace("%"+var+"%", string.Join(",", _variable__new.ArrayValues));
                            array = true;
                        } else {
                            _parse_result = _parse_result.Replace("%"+var+"%", _variable__new.Value);
                        }
                    }
                }

                int idx = Array.FindIndex(Variables, v => v.Name == _variable__.Name && v.Value == _variable__.Value);
                string _val = "";
                string[] _val_arr = [];
                if(value is string){
                    _variable__.Value = _parse_result;
                }
                else if(value is string[])
                    _variable__.ArrayValues = Regex.Split(_parse_result, @"(?<!\\),");
                else
                    throw new System.IO.IOException("Value must be a string or a string array");
                _variable__.IsArray = _variable__.ArrayValues.Length > 0;

                List<ATRCVariable> tmp = [.. Variables];

                tmp.RemoveAt(idx);
                tmp.Add(_variable__);
                Variables = tmp.ToArray();

                this.SaveToFile(null, name, _value, 4);
            }
            /// <summary>
            /// Removes the given variable from the file
            /// </summary>
            /// <param name="name">Variable to be removed</param>
            /// <exception cref="System.IO.IOException">Will throw exception if variable does not exist, or is private</exception>
            public void RemoveVariable(string name){
                // First, we will check if the given variable exists
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    throw new System.IO.IOException("Variable " + name + " does not exist");
                }
                if(_variables.Any(x => x.Name  == name && x.IsPrivate)){
                    throw new System.IO.IOException("Variable " + name + " is private and cannot be removed");
                }

                ATRCVariable _variable__ = _variables.First(x => x.Name == name);
                if(!_variable__.IsPrivate){
                    int idx = Array.IndexOf(Variables, _variable__);
                    
                    List<ATRCVariable> tmp = [.. Variables];
                    tmp.RemoveAt(idx);
                    Variables = tmp.ToArray();
                }
                

                this.SaveToFile(null, name, null, 5);
            }
            /// <summary>
            /// Checks if variable exists
            /// </summary>
            /// <param name="name">Variable name</param>
            /// <returns>True if exists, false if not</returns>
            public bool VariableExists(string name){
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    return false;
                }
                return true;
            }
            /// <summary>
            /// Checks if variable is private if it exists
            /// </summary>
            /// <param name="name"></param>
            /// <returns>False if it doesn't exist, or true/false if it is private or public</returns>
            public bool IsPrivateVariable(string name){
                ATRCVariable[] _variables = this.ReadVariables();
                if (!_variables.Any(x => x.Name == name)){
                    return false;
                }
                return _variables.First(x => x.Name == name).IsPrivate;
            }
            // ==========================
            //  Key functions
            // ==========================
            /// <summary>
            /// Adds a key to the given block
            /// </summary>
            /// <param name="block">Block where the key will be added</param>
            /// <param name="key">Key name that will be added</param>
            /// <param name="value">String or string array. </param>
            /// <exception cref="System.IO.IOException"></exception>
            public void AddKey(string block, string key, object value){
                if(block != null && block.Contains('%') || block.Contains('!') || block.Contains(','))
                        throw new System.IO.IOException("Block name cannot contain reserved characters (%, !, ,)");
                if(key != null && key.Contains('%') || key.Contains('!') || key.Contains(','))
                    throw new System.IO.IOException("Key or variable name cannot contain reserved characters (%, !, ,)");
                // First, we will check if the given block exists and if it contains the given key already
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                if (Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " already exists in block " + block);
                }
                string _value = ObjectToSTR(value); // This will be saved to the file
                // from this point onwards, it will be parsed. Arrays will look like this %var1%,%var2%,te\,st
                string _parse_result = _value;
                if(ParseToATRC_vars.Count > 0){
                    ATRCVariable[] _variables = this.ReadVariables(); // We need to read the variables
                    string[] __tmp = ParseToATRC_vars.ToArray();
                    bool array = false;
                    foreach(string var in __tmp){
                        if(!_variables.Any(x => x.Name == var)){
                            throw new System.IO.IOException("Variable " + var + " does not exist");
                        }
                        if(_variables.Any(x => x.Name  == var && x.IsPrivate)){
                            throw new System.IO.IOException("Variable " + var + " is private and cannot be accessed");
                        }
                        ATRCVariable _variable__ = _variables.First(x => x.Name == var);
                        if(_variable__.IsArray){
                            _parse_result = _parse_result.Replace("%"+var+"%", string.Join(",", _variable__.ArrayValues));
                            array = true;
                        } else {
                            _parse_result = _parse_result.Replace("%"+var+"%", _variable__.Value);
                        }
                    }
                }
                // Once we have checked that the block exists and the key does not exist, we can add the key
                ATRCKey newKey = new ATRCKey();
                if(value is string) {
                    newKey.Value = _parse_result;
                    newKey.IsArray = false;
                }
                else if(value is string[]){
                    newKey.ArrayValues = Regex.Split(_parse_result, @"(?<!\\),");
                    newKey.IsArray = true;
                }
                else
                    throw new System.IO.IOException("Value must be a string or a string array");

                newKey.Name = key;
                // newKey.IsArray = newKey.ArrayValues.Length > 0;

                List<ATRCKey> tmp = [.. Blocks.First(x => x.Name == block).Keys];
                tmp.Add(newKey);
                Blocks.First(x => x.Name == block).Keys = tmp.ToArray();
                this.SaveToFile(block, key, _value, 6);
            }

            /// <summary>
            /// Reads the given key from the given block
            /// </summary>
            /// <param name="block">From what block to search</param>
            /// <param name="key">Key name</param>
            /// <returns>string[]</returns>
            /// <exception cref="System.IO.IOException"></exception>
            public string[] A_ReadKey(string block, string key){
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                if (!Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " does not exist in block " + block);
                }
                return Blocks.First(x => x.Name == block).Keys.First(x => x.Name == key).ArrayValues;
            }
            /// <summary>
            /// Reads the given key from the given block
            /// </summary>
            /// <param name="block">From what block to search</param>
            /// <param name="key">Key name</param>
            /// <returns>string</returns>
            /// <exception cref="System.IO.IOException"></exception>
            public string S_ReadKey(string block, string key){
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                if (!Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " does not exist in block " + block);
                }
                return Blocks.First(x => x.Name == block).Keys.First(x => x.Name == key).Value;
            }
            /// <summary>
            /// Modifies the given key with the given value
            /// </summary>
            /// <param name="block">Block where the key is</param>
            /// <param name="key">key name</param>
            /// <param name="value">New key value</param>
            public void ModifyKey(string block, string key, object value){
                // First, we will have to look if the blocks and keys exist, not efficient but it will do for now
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                if (!Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " does not exist in block " + block);
                }

                ATRCKey _key = Blocks.First(x => x.Name == block).Keys.First(x => x.Name == key);
                int idx = Array.IndexOf(Blocks.First(x => x.Name == block).Keys, _key);


                
                string _value = ObjectToSTR(value); // This will be saved to the file
                // from this point onwards, it will be parsed. Arrays will look like this %var1%,%var2%,te\,st
                string _parse_result = _value;
                if(ParseToATRC_vars.Count > 0){
                    ATRCVariable[] _variables = this.ReadVariables(); // We need to read the variables
                    string[] __tmp = ParseToATRC_vars.ToArray();
                    bool array = false;
                    foreach(string var in __tmp){
                        if(!_variables.Any(x => x.Name == var)){
                            throw new System.IO.IOException("Variable " + var + " does not exist");
                        }
                        if(_variables.Any(x => x.Name  == var && x.IsPrivate)){
                            throw new System.IO.IOException("Variable " + var + " is private and cannot be accessed");
                        }
                        ATRCVariable _variable__ = _variables.First(x => x.Name == var);
                        if(_variable__.IsArray){
                            _parse_result = _parse_result.Replace("%"+var+"%", string.Join(",", _variable__.ArrayValues));
                            array = true;
                        } else {
                            _parse_result = _parse_result.Replace("%"+var+"%", _variable__.Value);
                        }
                    }
                }
                
                if(value is string){
                    _key.Value = (string)_parse_result;
                    _key.IsArray = false;
                } else if(value is string[]){
                    _key.ArrayValues = Regex.Split(_parse_result, @"(?<!\\),");
                    _key.IsArray = true;
                } else
                    throw new System.IO.IOException("Value must be a string or a string array");

                List<ATRCKey> tmp = [.. Blocks.First(x => x.Name == block).Keys];
                tmp.RemoveAt(idx);
                tmp.Add(_key);
                Blocks.First(x => x.Name == block).Keys = tmp.ToArray();



                this.SaveToFile(block, key, _value, 8);
            }
            
            /// <summary>
            /// Removes the given key from the given block
            /// </summary>
            /// <param name="block">Block where the key will be removed from</param>
            /// <param name="key">Key name</param>
            public void RemoveKey(string block, string key){
                // Check if neither the block nor the key exists
                if (!Blocks.Any(x => x.Name == block)){
                    throw new System.IO.IOException("Block " + block + " does not exist");
                }
                if (!Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " does not exist in block " + block);
                }

                ATRCKey _key = Blocks.First(x => x.Name == block).Keys.First(x => x.Name == key);
                int idx = Array.IndexOf(Blocks.First(x => x.Name == block).Keys, _key);
                List<ATRCKey> tmp = [.. Blocks.First(x => x.Name == block).Keys];
                tmp.RemoveAt(idx);
                Blocks.First(x => x.Name == block).Keys = tmp.ToArray();

                this.SaveToFile(block, key, null, 9);
            }
            /// <summary>
            /// Move key from src block to dst block
            /// </summary>
            /// <param name="fromBlock">source</param>
            /// <param name="toBlock">destination</param>
            /// <param name="key">key name</param>
            /// <exception cref="System.IO.IOException"></exception>
            public void MoveKey(string fromBlock, string toBlock, string key){
                // First, we will have to look if the blocks and keys exist, not efficient but it will do for now
                if (!Blocks.Any(x => x.Name == fromBlock)){
                    throw new System.IO.IOException("Block " + fromBlock + " does not exist");
                }
                if (!Blocks.Any(x => x.Name == toBlock)){
                    throw new System.IO.IOException("Block " + toBlock + " does not exist");
                }
                if (!Blocks.First(x => x.Name == fromBlock).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " does not exist in block " + fromBlock);
                }
                if (Blocks.First(x => x.Name == toBlock).Keys.Any(x => x.Name == key)){
                    throw new System.IO.IOException("Key " + key + " exists in block " + toBlock);
                }

                ATRCKey _key = Blocks.First(x => x.Name == fromBlock).Keys.First(x => x.Name == key);
                int idx = Array.IndexOf(Blocks.First(x => x.Name == fromBlock).Keys, _key);
                List<ATRCKey> tmp = [.. Blocks.First(x => x.Name == fromBlock).Keys];
                tmp.RemoveAt(idx);
                Blocks.First(x => x.Name == fromBlock).Keys = tmp.ToArray();

                // Now we will add the key to the new block
                idx = Array.IndexOf(Blocks.First(x => x.Name == toBlock).Keys, _key);
                tmp = [.. Blocks.First(x => x.Name == toBlock).Keys];
                tmp.Add(_key);
                Blocks.First(x => x.Name == toBlock).Keys = tmp.ToArray();
                // We'll have to send the toBlock as the value, for this the value parsing 
                // will be turned off
                this.SaveToFile(fromBlock, key, toBlock, 10);
            }

            /// <summary>
            /// Checks if key exists
            /// </summary>
            /// <param name="block">Key's block</param>
            /// <param name="key">Key's name</param>
            /// <returns>True if exists, false if not</returns>
            public bool KeyExists(string block, string key){
                if (!Blocks.Any(x => x.Name == block)){
                    return false;
                }
                if (!Blocks.First(x => x.Name == block).Keys.Any(x => x.Name == key)){
                    return false;
                }
                return true;
            }
            /// <summary>
            /// Saves the given value to the file
            /// </summary>
            /// <param name="block">Can be null, if variable is given</param>
            /// <param name="name">Can be null, Variable or key name</param>
            /// <param name="value">Can be null, Variable or key value</param>
            /// <param name="action">
            /// 0 = Add block
            /// 1 = Remove block
            /// 2 = Add variable
            /// 4 = Modify variable
            /// 5 = Remove variable
            /// 6 = Add key
            /// 8 = Modify key
            /// 9 = Remove key
            /// 10 = Move key
            /// </param>
            private void SaveToFile(string? block, string? name, object? value, int action, bool extraInfo = false){
                // We don't have to check whether anything exists
                // We will just modify the file
                // File location is stored in Filename

                // File contens will be here, which we will need to read and modify and then save
                try{
                    // Block or name can't contain reserved special characters
                    List<string> lines = [];

                    if(value != null && action != 10){
                        
                    }

                    int index = 0;
                    switch(action){
                        case 0:
                            using (StreamWriter sw = File.AppendText(this.Filename)) {
                                sw.WriteLine("\n[" + block + "]");
                            }
                            break;
                        case 1: 
                            // We will loop through the file and remove the block, parsing the block name to ATRC format
                            bool _block_removed = false;
                            bool _block_removed_next_found = false;
                            foreach (string line in File.ReadLines(this.Filename)) {
                                lines.Add(line); // Add the original lines
                                string _line_trim = line.Trim(); // Trim the line
                                string _parse_line = this.ParseLine(_line_trim);
                                if(_block_removed) {
                                    if(_line_trim.StartsWith('%')){
                                        continue; // Skip lines that are vars
                                    }
                                    if(!_line_trim.StartsWith('[') && !_block_removed_next_found) { // Skip and remove lines that are not blocks
                                        lines.Remove(line); 
                                        continue;
                                    } else {
                                        _block_removed_next_found = true;
                                    }
                                    // We will keep removing lines until the end of the file, or another block is found
                                    
                                } else {
                                    if(!_line_trim.StartsWith('[')) continue; // Skip lines that are not blocks
                                    if(_parse_line == "[" + block + "]"){ // If we are in the correct block
                                        _block_removed = true;
                                        lines.Remove(line); // Remove the block
                                    }
                                }
                            }
                            // Write the modified lines back to the file
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 2: 
                            bool _variable_added = false;
                            foreach (string line in File.ReadLines(this.Filename)) {
                                if(_variable_added){
                                    lines.Add(line); // Add the original lines
                                    continue;
                                }
                                lines.Add("%" + name + "%=" + value);
                                _variable_added = true;
                                lines.Add(line); // Add the original lines
                            }
                            // Write the modified lines back to the file
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 4: 
                            bool _variable_modified = false;
                            foreach (string line in File.ReadLines(this.Filename)) {
                                index++;
                                lines.Add(line); // Add the original lines
                                if(_variable_modified) continue; // Skip if the key was already added
                                string _line_trim = line.Trim(); // Trim the line
                                if(_line_trim.StartsWith("<%")) continue; // Since we do not allow the modifying of private variables, we will skip them
                                if(!_line_trim.StartsWith('%')) continue; // Skip lines that are not vars
                                string _parse_line = this.ParseLine(_line_trim);
                                // We found a variable, we will check if it is the correct one
                                int _start_looking_for_value = _parse_line.IndexOf('=');
                                if(_start_looking_for_value == -1) throw new System.IO.IOException("Invalid variable declaration - No value assigned at line " + index);
                                string _variable_name = _parse_line.AsSpan(1, _parse_line.IndexOf('%', 1) - 1).ToString(); 
                                // Now that we have our public variable name, we will compare the two
                                if(_variable_name != name) continue; // Skip if the variable is not the correct one
                                // We have found the correct one
                                lines.Remove(line); // Remove the old variable
                                lines.Add("%" + name + "%" + "=" + value); // Add the new variable
                                _variable_modified = true; // Set the variable to modified
                            }
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 5: 
                            bool _variable_deleted = false;
                            foreach (string line in File.ReadLines(this.Filename)) {
                                lines.Add(line); // Add the original lines
                                if(_variable_deleted) continue; // Skip if the key was already added
                                string _line_trim = line.Trim(); // Trim the line
                                if(_line_trim.StartsWith("<%")) continue; // Since we do not allow the modifying of private variables, we will skip them
                                if(!_line_trim.StartsWith('%')) continue; // Skip lines that are not vars
                                string _parse_line = this.ParseLine(_line_trim);
                                // We found a variable, we will check if it is the correct one
                                int _start_looking_for_value = _parse_line.IndexOf('=');
                                if(_start_looking_for_value == -1) throw new System.IO.IOException("Invalid variable declaration - No value assigned at line " + index);
                                string _variable_name = _parse_line.AsSpan(1, _parse_line.IndexOf('%', 1) - 1).ToString(); 
                                // Now that we have our public variable name, we will compare the two
                                if(_variable_name != name) continue; // Skip if the variable is not the correct one
                                // We have found the correct one
                                lines.Remove(line); // Remove the old variable
                                _variable_deleted = true; // Set the variable to removed
                            }
                            File.WriteAllLines(this.Filename, lines);
                              break;
                        case 6: 
                            // We will parse all of this to ATRC format
                            block = block;
                            bool _key_added = false;
                            foreach (string line in File.ReadLines(this.Filename)) {
                                lines.Add(line); // Add the original lines
                                if(_key_added) continue; // Skip if the key was already added
                                string _line_trim = line.Trim(); // Trim the line
                                if(!_line_trim.StartsWith('[')) continue; // Skip lines that are not blocks
                                string _parse_line = this.ParseLine(_line_trim);
                                if(_parse_line == "[" + block + "]"){ // If we are in the correct block
                                    lines.Add(name + "=" + value); // Add the new key
                                    _key_added = true;
                                } 
                            }

                            // Write the modified lines back to the file
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 8: 
                            bool _key_modified = false;
                            string _current_block_2 = ""; // Will keep track of the current block

                            foreach (string line in File.ReadLines(this.Filename)) {
                                index++;
                                lines.Add(line); // Add the original lines
                                if(_key_modified) continue; // Skip if the key was already added
                                string _line_trim = line.Trim(); // Trim the line
                                if(!_line_trim.StartsWith('[') && !_line_trim.StartsWith(name+"=")) continue; // Skip lines that are not block or key
                                _line_trim = this.ParseLine(_line_trim);
                                if(_line_trim == "[" + block + "]"){ // If we are in the correct block
                                    _current_block_2 = _line_trim;
                                }

                                if(_line_trim.StartsWith(name + "=") && _current_block_2 == "["+block+"]"){ // If we are in the correct block
                                    lines.Remove(line); // Remove the old key
                                    lines.Add(name + "=" + value); // Add the new key
                                    _key_modified = true;
                                }
                            }
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 9: 
                            bool _key_removed = false;
                            string _current_block_3 = ""; // Will keep track of the current block

                            foreach (string line in File.ReadLines(this.Filename)) {
                                index++;
                                lines.Add(line); // Add the original lines
                                if(_key_removed) continue; // Skip if the key was already added
                                string _line_trim = line.Trim(); // Trim the line
                                if(!_line_trim.StartsWith('[') && !_line_trim.StartsWith(name+"=")) continue; // Skip lines that are not block or key
                                _line_trim = this.ParseLine(_line_trim);
                                if(_line_trim == "[" + block + "]"){ // If we are in the correct block
                                    _current_block_3 = _line_trim;
                                }

                                if(_line_trim.StartsWith(name + "=") && _current_block_3 == "["+block+"]"){ // If we are in the correct block
                                    lines.Remove(line); // Remove the old key
                                    _key_removed = true;
                                }
                            }
                            File.WriteAllLines(this.Filename, lines);
                            break;
                        case 10:
                            int _from_block_index = -1; // This will keep track of the block index
                            int _to_block_index = -1; // This will also keep track of the block index
                            int _key_index = -1; // This will keep track of the key index
                            bool _key_moved = false;
                            string _fromBlock = block;
                            string _toBlock = (string)value;
                            string _key = name;
                            string _original_key_line = "";
                            string _current_block = "";
                            // First, we will have to find the block indexes and key index
                            foreach (string line in File.ReadLines(this.Filename)) {
                                lines.Add(line); // Add the original lines
                                index++;
                                string _line_trim = line.Trim(); // Trim the line
                                if(_line_trim.StartsWith('!')) continue; // Skip lines that are comments
                                if(!_line_trim.StartsWith('[') && !_line_trim.StartsWith(_key + "=")) continue; // Skip lines that are not blocks
                                string _parse_line = this.ParseLine(_line_trim);
                                if(_parse_line.StartsWith('[')) _current_block = _parse_line;

                                if(_parse_line == "[" + _fromBlock + "]"){ // If we are in the correct block
                                    _from_block_index = index; // We have found the block index
                                }
                                if(_parse_line == "[" + _toBlock + "]"){ // If we are in the correct block
                                    _to_block_index = index; // We have found the block index
                                }
                                if(_parse_line.StartsWith(_key + "=") && _current_block == "["+_fromBlock+"]"){ // If we are in the correct block
                                    _key_index = index; // We have found the key index
                                    _original_key_line = line;
                                    lines.Remove(line); // Remove the key from the original block
                                }
                            }

                            // Now we will loop through the file, moving everything
                            lines.Remove(_original_key_line); // Remove the key from the original block
                            lines.Insert(
                                _to_block_index < _from_block_index ? _to_block_index : _to_block_index-1,
                                _original_key_line);
                            // Write the modified lines back to the file
                            File.WriteAllLines(this.Filename, lines);
                            break;
                    }
                }
                catch (Exception ex) {
                    // Handle exceptions here
                    Console.WriteLine("An error occurred: " + ex.Message);
                }
            }
            /// <summary>
            /// Turn object to string, with it being parsed to ATRC format
            /// </summary>
            /// <param name="value"></param>
            /// <returns></returns>
            /// <exception cref="System.IO.IOException"></exception>
            private string ObjectToSTR(object value){
                ParseToATRC_vars.Clear();
                string _string_value = "";
                string[] _array_value = [];
                
                if(value is string) {
                    _string_value = (string)value;
                    _string_value = this.ParseToATRC(_string_value);
                }
                else if(value is string[]){
                    _array_value = (string[])value;
                    int index_parser = 0;
                    foreach(string val in _array_value){
                        _array_value[index_parser] = this.ParseToATRC(val);
                        index_parser++;
                    }
                    _string_value = string.Join(",", _array_value);
                }
                else
                    throw new System.IO.IOException("Value must be a string or a string array");

                return _string_value;
            }
            

            List<string> ParseToATRC_vars = [];
            /// <summary>
            /// Parses the given string to ATRC format
            /// 
            /// </summary>
            /// <param name="parse">String to be parsed</param>
            /// <returns>The parsed string</returns>
            private string ParseToATRC(string parse){
                string _parse_result = "";
                // Since only value is parsed, some trickery can be done
                string _parse_result_2 = "";
                bool _looking_for_vars = false;
                char _last_char = ' ';
                foreach(char c in parse){
                    _last_char = c;
                    // When we are looking for variables, we will go here.¨
                    if(_looking_for_vars){
                        if(c == '%'){
                            _parse_result += "%" + _parse_result_2 + "%";
                            ParseToATRC_vars.Add(_parse_result_2);
                            _looking_for_vars = false;
                            _parse_result_2 = "";
                            continue;
                        }

                        // If there is , or !, we will just quess that it is not a variable, and we will assign everything as parsed
                        else if(c == ','){
                            _parse_result += "\\%"+_parse_result_2 + "\\,";
                            _looking_for_vars = false;
                            continue;
                        }
                        else if(c == '!'){
                            _parse_result += "\\%"+_parse_result_2 + "\\!";
                            _looking_for_vars = false;
                            continue;
                        }
                        _parse_result_2 += c;
                    } else {
                        if(c == '%'){
                            _looking_for_vars = true;
                            continue;
                        }
                        else if(c == ','){
                            _parse_result += "\\,";
                            continue;
                        }
                        else if(c == '!'){
                            _parse_result += "\\!";
                            continue;
                        } else if(c == '\\'){
                            _parse_result += '\\';
                            continue;
                        }
                        _parse_result += c;
                    }                    
                }
                if(_looking_for_vars){
                    _parse_result += "\\%" + _parse_result_2;
                }

                return _parse_result;
            }

            /// <summary>
            /// Parse line, return parsed line
            /// </summary>
            /// <param name="line">line</param>
            /// <returns></returns>
            private string ParseLine(string line){
                string _parse_result = "";
                bool _last_was_re_dash = false;
                foreach(char c in line){
                    if(c == '\\'){
                        _last_was_re_dash = true;
                        continue;
                    }
                    else if(c == '!' && !_last_was_re_dash) {_last_was_re_dash = false;break;}  
                    _parse_result += c;
                }
                return _parse_result.Trim();
            }

        }
            /// <summary>
            /// Parse value to boolean
            /// </summary>
            /// <param name="value"></param>
            /// <returns></returns>
            /// <exception cref="System.IO.IOException"></exception>
            public bool ParseToBool(string value){
                value = value.ToLower().Trim();
                if(value == "true") return true;
                if(value == "false") return false;
                throw new System.IO.IOException("Value: " + value + " must be a boolean");
            }
            /// <summary>
            /// Parse string to integer
            /// </summary>
            /// <param name="value"></param>
            /// <returns></returns>
            /// <exception cref="System.IO.IOException"></exception>
            public int ParseToInt(string value){
                if(!int.TryParse(value, out int result))
                    throw new System.IO.IOException("Value: " + value + " must be an integer");
                return result;
            }
            /// <summary>
            /// Parse string to float
            /// </summary>
            /// <param name="value">String</param>
            /// <returns></returns>
            /// <exception cref="System.IO.IOException"></exception>
            public float ParseToFloat(string value){
                if(!float.TryParse(value, out float result))
                    throw new System.IO.IOException("Value:" + value + " must be a float");
                return result;
            }
    }
}