--[[
    任务数据
    Time                    任务时间
    Id                      任务Id
    State                   任务状态

    方法
    SetState
]]

--[[
本模块内部使用的安全封装:GetAddress / GetAddressData 失败时返回 0(数字)
而非 false,保证地址未初始化阶段(游戏启动/切图)所有算术表达式安全求值。
仅遮蔽本模块作用域,对外注册的原始 API 与用户脚本不受影响;
bool 类型在有效地址上读到的真实布尔值原样保留。
]]
local _GetAddress = GetAddress
local GetAddress = function(base, ...)
    local r = _GetAddress(base, ...)
    if r == false then return 0 end
    return r
end
local _GetAddressData = GetAddressData
local GetAddressData = function(addr, t)
    if type(addr) ~= 'number' or addr < 0x10000 then
        return 0 -- 无效地址(失败链路的 0 值算术结果),直接降级为 0
    end
    local r = _GetAddressData(addr, t)
    if r == false and t ~= 'bool' then
        return 0 -- 有效地址但读取失败(指针悬挂等),降级为 0
    end
    return r
end

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
    local timePtr = pointer:time()
    if not timePtr then return 0 end
    local time = GetAddressData(timePtr + 0xC24, 'float')
    return time
end

--获取任务Id
function engine_quest:getId()
    local questPtr = pointer:quest()
    if not questPtr then return 0 end
    local id = GetAddressData(questPtr + 0x4C, 'int')
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

    --获取基址(扫描结果缓存到全局变量,跨脚本状态共享,避免每个状态重复全模块扫描)
    aob_quest = GlobalVariable_int('Engine_aob_Quest')
    aob_player = GlobalVariable_int('Engine_aob_Player')
    if aob_quest == 0 or aob_player == 0 then
        if aob_quest == 0 then
            aob_quest = SearchPattern({ 0x10, 0x22, "??", 0x0F, 0x00, 0x00 })
            if not aob_quest then aob_quest = 0x14500ED30 end
            setGlobalVariable_int('Engine_aob_Quest', aob_quest)
        end
        if aob_player == 0 then
            aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
            if not aob_player then aob_player = 0x1450139A0 end
            setGlobalVariable_int('Engine_aob_Player', aob_player)
        end
    end

    --时间
    o.Time = self:getTime()
    --Id
    o.Id = self:getId()
    --状态
    o.State = self:getState()

    return trace(o)
end

return engine_quest
