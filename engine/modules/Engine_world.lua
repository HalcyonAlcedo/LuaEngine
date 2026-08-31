--[[
    世界数据
    MapId                   地图Id
    Time                    当前时间(任务时间)
    Position                坐标

    方法
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

engine_world = {
    info = {
        name = 'World'
    },
    MapId = 0,
    Time = 0,
    Position = {
        wayPosition = { x = 0, y = 0, z = 0 }
    }
}
local aob_world
local aob_player

local pointer = {
    map = function() return GetAddress(aob_player,{ 0x50, 0x7D20 }) end,
    worldData = function() return GetAddress(aob_world,{ 0x90, 0x40, 0x90, 0x18 }) end
}

--获取地图Id
function engine_world:getMapId()
    local mapPtr = pointer:map()
    if not mapPtr then return 0 end
    local Id = GetAddressData(mapPtr + 0xB88, 'int')
    return Id
end

--获取当前时间
function engine_world:getTime()
    if not pointer:map() then return 0 end
    local time = GetAddressData(pointer:map() + 0xC24, 'float')
    return time
end

--获取导航坐标
function engine_world:getWayPosition()
    local mapPtr = pointer:map()
    local worldPtr = pointer:worldData()
    if not mapPtr or not worldPtr or worldPtr < 0x1000 then
        return { x = 0, y = 0, z = 0 }
    end
    return {
        x = GetAddressData(worldPtr + 0x200, 'float'),
        y = GetAddressData(worldPtr + 0x204, 'float'),
        z = GetAddressData(worldPtr + 0x208, 'float')
    }
end

--监听
local function traceHandle(k, v)
    -- 这里可以添加监听逻辑
end

local index = {}
local mt = {
    __index = function(t, k)
        if k == 'MapId' then
            return engine_world:getMapId()
        elseif k == 'Time' then
            return engine_world:getTime()
        elseif k == 'Position' then
            return { wayPosition = engine_world:getWayPosition() }
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

function engine_world:new()
    local o = {}

    --获取基址(扫描结果缓存到全局变量,跨脚本状态共享,避免每个状态重复全模块扫描)
    aob_world = GlobalVariable_int('Engine_aob_World')
    aob_player = GlobalVariable_int('Engine_aob_Player')
    if aob_world == 0 or aob_player == 0 then
        if aob_world == 0 then
            aob_world = SearchPattern({ 0xB0, 0x6B, "??", "??", 0x00, 0x00, 0x00, 0x00 })
            if not aob_world then aob_world = 0x1451C4E68 end
            setGlobalVariable_int('Engine_aob_World', aob_world)
        end
        if aob_player == 0 then
            aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
            if not aob_player then aob_player = 0x1450139A0 end
            setGlobalVariable_int('Engine_aob_Player', aob_player)
        end
    end

    --地图Id
    o.MapId = self:getMapId()
    --时间
    o.Time = self:getTime()
    --导航坐标
    o.Position = { wayPosition = self:getWayPosition() }

    return trace(o)
end

return engine_world
