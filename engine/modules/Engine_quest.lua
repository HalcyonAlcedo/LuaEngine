--[[
    任务数据
    Time                    任务时间
    Id                      任务Id
    State                   任务状态

    方法
    SetState
]]
engine_quest = {
    info = {
        name = 'Quest'
    },
    Time = 0,
    Id = 0,
    State = 0,
}

local aob_quest
local aob_player

local pointer = {
    time = function() return GetAddress(aob_player,{ 0x50, 0x7D20 }) end,
    quest = function() return GetAddressData(aob_quest, 'int') end
}

--获取当前任务时间
function engine_quest:getTime()
    if not pointer:time() then return 0 end
    local time = GetAddressData(pointer:time() + 0xC24, 'float')
    return time
end

--获取任务Id
function engine_quest:getId()
    if not pointer:quest() then return 0 end
    local id = GetAddressData(pointer:quest() + 0x4C, 'int')
    return id
end

--获取任务状态
function engine_quest:getState()
    if not pointer:quest() then return 0 end
    local state = GetAddressData(pointer:quest() + 0x54, 'int')
    return state
end

--设置任务状态
function engine_quest:setState(state)
    if not pointer:quest() then return 0 end
    SetAddressData(pointer:quest() + 0x38, 'int', state)
end

--监听
local function traceHandle(k, v)
    if k == 'State' then
        engine_quest:setState(v)
    elseif k == 'EndTime' then
        engine_quest:setEndTime(v)
    end
end

local index = {}
local mt = {
    __index = function(t, k)
        if k == 'Time' then
            return engine_quest:getTime()
        elseif k == 'Id' then
            return engine_quest:getId()
        elseif k == 'State' then
            return engine_quest:getState()
        end
        return t[index][k]
    end,
    __newindex = function(t, k, v)
        traceHandle(k, v)
        t[index][k] = v
    end
}

local function trace(t)
    local proxy = {}
    proxy[index] = t
    setmetatable(proxy, mt)
    return proxy
end

function engine_quest:new()
    local o = {}

    --获取基址
    if aob_quest == nil or aob_player == nil then
        aob_quest = SearchPattern({ 0x10, 0x22, "??", 0x0F, 0x00, 0x00 })
        aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
    end

    if aob_quest == nil or not aob_quest then
        aob_quest = 0x14500ED30
    end
    if aob_player == nil or not aob_quest then
        aob_player = 0x1450139A0
    end

    return trace(o)
end

return engine_quest
