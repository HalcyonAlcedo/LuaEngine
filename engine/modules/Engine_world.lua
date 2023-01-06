--[[
    世界数据
    MapId                   地图Id
    Time                    当前时间(任务时间)

    方法
]]
engine_world = {}
engine_world.pointer = {
    Map = GetAddress(0x145073ED0,{ 0x50, 0x7D20 })
}
--获取地图Id
local function getMapId()
    local Id = GetAddressData(engine_world.pointer.Map + 0xB88, 'int')
    return Id
end
--获取当前时间
local function getTime()
    local time = GetAddressData(engine_world.pointer.Map + 0xC24, 'float')
    return time
end

--地图Id
engine_world.MapId = getMapId()
--时间
engine_world.Time = getTime()

return engine_world