--[[
    世界数据
    MapId                   地图Id
    Time                    当前时间(任务时间)
    Position                坐标

    方法
]]
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
    if not pointer:map() then return 0 end
    local Id = GetAddressData(pointer:map() + 0xB88, 'int')
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
    if not pointer:map() or not pointer:worldData() or pointer:worldData() < 0x1000 then
        return { x = 0, y = 0, z = 0 }
    end
    return {
        x = GetAddressData(pointer:worldData() + 0x200, 'float'),
        y = GetAddressData(pointer:worldData() + 0x204, 'float'),
        z = GetAddressData(pointer:worldData() + 0x208, 'float')
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

    --获取基址
    if aob_world == nil or aob_player == nil then
        aob_world = SearchPattern({ 0xB0, 0x6B, "??", "??", 0x00, 0x00, 0x00, 0x00 })
        aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
    end

    if aob_world == nil or not aob_world then
        aob_world = 0x1451C4E68
    end
    if aob_player == nil or not aob_player then
        aob_player = 0x1450139A0
    end

    return trace(o)
end

return engine_world
