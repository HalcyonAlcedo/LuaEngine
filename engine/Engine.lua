--[[
    LuaEngine游戏数据解析引擎
    版本: 1.5
    适用于LuaEngine模组1.1.0版本
]]


-- log输出格式化
local function logPrint(str)
    str = "\n" .. str
    Console_Info(str)
end
-- key值格式化
local function formatKey(key)
    local t = type(key)
    if t == "number" then
        return "["..key.."]"
    elseif t == "string" then
        local n = tonumber(key)
        if n then
            return "["..key.."]"
        end
    end
    return key
end
-- 栈
local function newStack()
    local stack = {
        tableList = {}
    }
    function stack:push(t)
        table.insert(self.tableList, t)
    end
    function stack:pop()
        return table.remove(self.tableList)
    end
    function stack:contains(t)
        for _, v in ipairs(self.tableList) do
            if v == t then
                return true
            end
        end
        return false
    end
    return stack
end
-- 输出打印table表 函数
function printTable(...)
    local args = {...}
    for k, v in pairs(args) do
        local root = v
        if type(root) == "table" then
            local temp = {
                "------------------------ printTable start ------------------------\n",
                "{\n",
            }
            local stack = newStack()
            local function table2String(t, depth)
                stack:push(t)
                if type(depth) == "number" then
                    depth = depth + 1
                else
                    depth = 1
                end
                local indent = ""
                for i=1, depth do
                    indent = indent .. "    "
                end
                for k, v in pairs(t) do
                    local key = tostring(k)
                    local typeV = type(v)
                    if typeV == "table" then
                        if key ~= "__valuePrototype" then
                            if stack:contains(v) then
                                table.insert(temp, indent..formatKey(key).." = {检测到循环引用!},\n")
                            else
                                table.insert(temp, indent..formatKey(key).." = {\n")
                                table2String(v, depth)
                                table.insert(temp, indent.."},\n")
                            end
                        end
                    elseif typeV == "string" then
                        table.insert(temp, string.format("%s%s = \"%s\",\n", indent, formatKey(key), tostring(v)))
                    else
                        table.insert(temp, string.format("%s%s = %s,\n", indent, formatKey(key), tostring(v)))
                    end
                end
                stack:pop()
            end
            table2String(root)
            table.insert(temp, "}\n------------------------- printTable end -------------------------")
            logPrint(table.concat(temp))
        else
            logPrint(tostring(root))
        end
    end
end

_G.print = printTable

engine = {}

--[[
    按键处理引擎
    可使用预设按键或直接输入键盘VK值进行判断

    键盘按键与组合键是否按下检测
    engine.keypad({'Ctrl', '+'})
    engine.keypad('Num0')
    Xbox手柄按键按下检测
    engine.keypad({'LB', 'R→'}, true)
    engine.keypad('RB', true)
]]
engine.KeyCustom = {}
local function KeyToKeyId(Key)
    local keyList = {
        [1] = 'LMouse', [2] = 'RMouse', [3] = 'Break', [4] = 'MMouse',
        [5] = 'BMouse', [6] = 'FMouse',
        [8] = 'BackSpace', [9] = 'Tab', [12] = 'Clear', [13] = 'Enter',
        [16] = 'Shift', [17] = 'Ctrl', [18] = 'Alt', [19] = 'Pause',
        [20] = 'CapsLock', [27] = 'Esc', [32] = 'Space', [33] = 'PageUp',
        [34] = 'PageDown', [35] = 'End', [36] = 'Home', [37] = '←',
        [38] = '↑', [39] = '→', [40] = '↓', [41] = 'Select',
        [43] = 'Execute', [44] = 'PrintScreen', [45] = 'Insert', [46] = 'Delete',
        [47] = 'Help', [48] = '0', [49] = '1', [50] = '2',
        [51] = '3', [52] = '4', [53] = '5', [54] = '6',
        [55] = '7', [56] = '8', [57] = '9', [65] = 'a',
        [66] = 'b', [67] = 'c', [68] = 'd', [69] = 'e',
        [70] = 'f', [71] = 'g', [72] = 'h', [73] = 'i',
        [74] = 'j', [75] = 'k', [76] = 'l', [77] = 'm',
        [78] = 'n', [79] = 'o', [80] = 'p', [81] = 'q',
        [82] = 'r', [83] = 's', [84] = 't', [85] = 'u',
        [86] = 'v', [87] = 'w', [88] = 'x', [89] = 'y',
        [90] = 'z', [96] = 'Num0', [97] = 'Num1', [98] = 'Num2',
        [99] = 'Num3', [100] = 'Num4', [101] = 'Num5', [102] = 'Num6',
        [103] = 'Num7', [104] = 'Num8', [105] = 'Num9', [106] = '*',
        [107] = '+', [108] = 'NumEnter', [109] = '-', [110] = '.',
        [111] = '/', [112] = 'f1', [113] = 'f2', [114] = 'f3',
        [115] = 'f4', [116] = 'f5', [117] = 'f6', [118] = 'f7',
        [119] = 'f8', [120] = 'f9', [121] = 'f10', [122] = 'f11',
        [123] = 'f12', [144] = 'Numberlock', [192] = '~',
    }
    if type(Key) == 'number' then return Key end
    for id, value in pairs(engine.KeyCustom) do
        if type(value) == 'string' then keyList[id] = value end
    end
    for id, value in pairs(keyList) do
        if string.lower(value) == string.lower(Key) then return id end
    end
    return -1
end
local function XKeyToKeyId(Key)
    local keyList = {
        [0] = 'L↑', [1] = 'L→', [2] = 'L↓', [3] = 'L←', [4] = 'LDown', [5] = 'R↑',
        [6] = 'R→', [7] = 'R↓', [8] = 'R←', [9] = 'RDown', [10] = 'LT', [11] = 'RT',
        [12] = 'LB', [13] = 'RB', [14] = '↑', [15] = '→', [16] = '↓', [17] = '←',
        [32] = 'Y', [33] = 'B', [34] = 'A', [35] = 'X', [36] = 'Window', [37] = 'Menu',
    }
    if type(Key) == 'number' then return Key end
    for id, value in pairs(keyList) do
        if string.lower(value) == string.lower(Key) then return id end
    end
    return -1
end
--获取组合键是否按下
function engine.keypad(Keys, Xbox)
    if Xbox == nil then Xbox = false end

    if type(Keys) == 'table' and next(Keys) ~= nil then
        if Xbox then
            for _, Key in pairs(Keys) do
                if not XCheckKeyIsPressed(XKeyToKeyId(Key)) then return false end
            end
        else
            for _, Key in pairs(Keys) do
                if not CheckKeyIsPressed(KeyToKeyId(Key)) then return false end
            end
        end
        return true
    elseif type(Keys) == 'string' then
        if Xbox then
            if XCheckKeyIsPressed(XKeyToKeyId(Keys)) then return true end
        else
            if CheckKeyIsPressed(KeyToKeyId(Keys)) then return true end
        end
    elseif type(Keys) == 'number' then
        if Xbox then
            if XCheckKeyIsPressed(Keys) then return true end
        else
            if CheckKeyIsPressed(Keys) then return true end
        end
    end
    return false
end
--获取两点间距离
function engine.distance(p1,p2)
    return math.sqrt((p1.x-p2.x)^2+(p1.y-p2.y)^2+(p1.z-p2.z)^2)
end
--列表中是否存在某值
function engine.table_include(value, tab)
    for _,v in ipairs(tab) do
        if v == value then
            return true
        end
    end
    return false
end
--获取文件列表
local allFilePath
function getFiles(filePath, dir)
    for entry in lfs.dir(filePath) do
        if entry~='.' and entry~='..' then
            local path = filePath.."\\"..entry
            local attr = lfs.attributes(path)
            assert(type(attr)=="table")
            if(attr.mode == "directory" and dir) then
                getFiles(path)
            elseif attr.mode=="file" then
                table.insert(allFilePath,{
                    path = path,
                    file = entry
                })
            end
        end
    end
end
function engine.GetAllFiles(rootPath, dir)
    if dir == nil then dir = false end
    allFilePath = {}
    getFiles(rootPath, dir)
    return allFilePath
end
--自动加载引擎模组
local engineModules = engine.GetAllFiles("./Lua/modules")
for _, eFile in pairs(engineModules) do
    if eFile.file:sub(-4) == ".lua" then
        local engineModule = dofile("./Lua/modules/"..eFile.file)
        if engineModule ~= nil then
            if engineModule.info ~= nil and engineModule.info.name ~= nil then
                engine[engineModule.info.name] = engineModule
            else
                engine[eFile.file] = engineModule
            end
        end
    end
end

_G.engine = engine