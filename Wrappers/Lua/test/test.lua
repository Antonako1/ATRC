local m = require("ATRCLua")

-- [ Lua test functions ] --
print("Running Lua test functions...")

print("Testing the test function...")
local res = m.Add(2, 3)
if res == 5 then
    print("[Passed]: Add function works correctly.")
else
    print("[Failed]: Add function returned " .. res .. ", expected 5.")
end

-- [ ATRC STD library functions ] --
print("\nTesting ATRC STD library functions...")
res = m.StrToBool("true")
if res == true then
    print("[Passed]: StrToBool function works correctly for 'true'.")
else
    print("[Failed]: StrToBool function returned " .. tostring(res) .. ", expected true.")
end

res = m.StrToBool("false")
if res == false then
    print("[Passed]: StrToBool function works correctly for 'false'.")
else
    print("[Failed]: StrToBool function returned " .. tostring(res) .. ", expected false.")
end

res = m.StrToNum("-1234567890123456789")
if res == -1234567890123456789 then
    print("[Passed]: StrToNum function works correctly.")
else
    print("[Failed]: StrToNum function returned " .. res .. ", expected -1234567890123456789.")
end

res = m.StrToDouble("3.141592653589793")
if res == 3.141592653589793 then
    print("[Passed]: StrToDouble function works correctly.")
else
    print("[Failed]: StrToDouble function returned " .. res .. ", expected 3.141592653589793.")
end

res = m.ParseMathExp("1+2*3-4/2")
if res == 1+2*3-4/2 then
    print("[Passed]: ParseMathExp function works correctly.")
else
    print("[Failed]: ParseMathExp function returned " .. res .. ", expected " .. (1+2*3-4/2) .. ".")
end




-- [ ATRC file functions ] --
print("\nTesting ATRC file functions...")
local fd = m.OpenATRC("file.atrc")
if fd ~= nil then
    print("[Passed]: OpenATRC function works correctly, returned file descriptor: ")
else
    print("[Failed]: OpenATRC function returned nil, expected a valid file descriptor.")
end

m.SetAutosave(fd, true)
if m.GetAutosave(fd) == true then
    print("[Passed]: SetAutosave function works correctly, autosave is enabled.")
else
    print("[Failed]: SetAutosave function did not enable autosave as expected.")
end

m.SetWriteCheck(fd, true)
if m.GetWriteCheck(fd) == true then
    print("[Passed]: SetWriteCheck function works correctly, write check is enabled.")
else
    print("[Failed]: SetWriteCheck function did not enable write check as expected.")
end

res = m.ReadVariable(fd, "testVar")
if res ~= nil then
    print("[Passed]: ReadVariable function works correctly, returned: " .. res)
else
    print("[Failed]: ReadVariable function returned nil, expected a valid variable value.")
end

res = m.ReadKey(fd, "ReadyBlock", "ReadyKey")
if res ~= nil then
    print("[Passed]: ReadKey function works correctly, returned: " .. res)
else
    print("[Failed]: ReadKey function returned nil, expected a valid key value.")
end

res = m.DoesExistBlock(fd, "ReadyBlock")
if res == true then
    print("[Passed]: DoesExistBlock function works correctly, block exists.")
else
    print("[Failed]: DoesExistBlock function returned false, expected true.")
end

res = m.DoesExistVariable(fd, "testVar")
if res == true then
    print("[Passed]: DoesExistVariable function works correctly, variable exists.")
else
    print("[Failed]: DoesExistVariable function returned false, expected true.")
end

res = m.DoesExistVariable(fd, "InternalKey")
if res == false then
    print("[Passed]: DoesExistVariable function works correctly, variable does not exist or is private.")
else
    print("[Failed]: DoesExistVariable function returned true, expected false.")
end

res = m.DoesExistKey(fd, "ReadyBlock", "ReadyKey")
if res == true then
    print("[Passed]: DoesExistKey function works correctly, key exists.")
else
    print("[Failed]: DoesExistKey function returned false, expected true.")
end

res = m.IsPublic(fd, "InternalKey")
if res == false then
    print("[Passed]: IsPublic function returned " .. tostring(res) .. " for private key.")
else
    print("[Failed]: IsPublic function returned " .. tostring(res) .. ", expected false.")
end

local args = {"InternalKey", "Variable value!"}
local line = m.ReadKey(fd, "ReadyBlock", "Inserts")
res = m.InsertVars(line, args)
if res ~= nil then
    print("[Passed]: InsertVars function works correctly, returned: " .. res)
else
    print("[Failed]: InsertVars function returned nil, expected a valid result.")
end
res = m.AddBlock(fd, "NewBlock")
if res == true then
    print("[Passed]: AddBlock function works correctly, block added.")
else
    print("[Failed]: AddBlock function returned false, expected true.")
end
res = m.RemoveBlock(fd, "NewBlock")
if res == true then
    print("[Passed]: RemoveBlock function works correctly, block removed.")
else
    print("[Failed]: RemoveBlock function returned false, expected true.")
end



res = m.AddVariable(fd, "NewVar", "New value!")
if res == true then
    print("[Passed]: AddVariable function works correctly, variable added.")
else
    print("[Failed]: AddVariable function returned false, expected true.")
end

res = m.ModifyVariable(fd, "NewVar", "Modified value2!")
if res == true then
    print("[Passed]: ModifyVariable function works correctly, variable modified.")
else
    print("[Failed]: ModifyVariable function returned false, expected true.")
end

res = m.RemoveVariable(fd, "NewVar")
if res == true then
    print("[Passed]: RemoveVariable function works correctly, variable removed.")
else
    print("[Failed]: RemoveVariable function returned false, expected true.")
end



res = m.AddKey(fd, "ReadyBlock", "NewKey", "New value!")
if res == true then
    print("[Passed]: AddKey function works correctly, key added.")
else
    print("[Failed]: AddKey function returned false, expected true.")
end

res = m.ModifyKey(fd, "ReadyBlock", "NewKey", "Modified value!")
if res == true then
    print("[Passed]: ModifyKey function works correctly, key modified.")
else
    print("[Failed]: ModifyKey function returned false, expected true.")
end

res = m.RemoveKey(fd, "ReadyBlock", "NewKey")
if res == true then
    print("[Passed]: RemoveKey function works correctly, key removed.")
else
    print("[Failed]: RemoveKey function returned false, expected true.")
end



res = m.WriteCommentToBottom(fd, "This is a comment at the bottom.")
if res == true then
    print("[Passed]: WriteCommentToBottom function works correctly, comment added.")
else
    print("[Failed]: WriteCommentToBottom function returned false, expected true.")
end

res = m.WriteCommentToTop(fd, "This is a comment at the top.")
if res == true then
    print("[Passed]: WriteCommentToTop function works correctly, comment added.")
else
    print("[Failed]: WriteCommentToTop function returned false, expected true.")
end

res = m.GetEnumValue(fd, "ReadyBlock", "Inserts")
if res ~= nil then
    print("[Passed]: GetEnumValue function works correctly, returned: " .. res)
else
    print("[Failed]: GetEnumValue function returned nil, expected a valid result.")
end

res = m.GetVariableCount(fd)
if res >= 0 then
    print("[Passed]: GetVariableCount function works correctly, returned: " .. res)
else
    print("[Failed]: GetVariableCount function returned " .. res .. ", expected a non-negative integer.")
end

res = m.GetFilename(fd)
if res ~= nil then
    print("[Passed]: GetFilename function works correctly, returned: " .. res)
else
    print("[Failed]: GetFilename function returned nil, expected a valid filename.")
end

fd2 = m.CopyATRC(fd)
if fd2 ~= nil then
    print("[Passed]: CopyATRC function works correctly.")
else
    print("[Failed]: CopyATRC function returned nil, expected a valid result.")
end

res = m.GetVariableCount(fd2)
if res >= 0 then
    print("[Passed]: GetVariableCount function works correctly on copied ATRC, returned: " .. res)
else
    print("[Failed]: GetVariableCount function on copied ATRC returned " .. res .. ", expected a non-negative integer.")
end
m.CloseATRC(fd2)
fd2 = nil

m.CloseATRC(fd)
fd = nil
if fd == nil then
    print("[Passed]: OpenATRC function works correctly.")
else
    print("[Failed]: OpenATRC function returned a file descriptor, expected nil.")
end
