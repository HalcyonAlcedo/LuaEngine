--[[
    世界数据
    MapId                   地图Id
    Time                    当前时间(任务时间)

    方法
]]
engine_world = {
    MapId = 0,
    Time = 0
}

local pointer = {
    map = function() return GetAddress(0x145073ED0,{ 0x50, 0x7D20 }) end
}

--获取地图Id
function engine_world:getMapId()
    local Id = GetAddressData(pointer:map() + 0xB88, 'int')
    return Id
end
--获取当前时间
function engine_world:getTime()
    local time = GetAddressData(pointer:map() + 0xC24, 'float')
    return time
end

function engine_world:new ()
    local o = {}
    --地图Id
    o.MapId = self:getMapId()
    --时间
    o.Time = self:getTime()
    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_world