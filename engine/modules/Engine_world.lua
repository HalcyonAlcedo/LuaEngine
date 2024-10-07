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
        wayPosition = {x = 0, y = 0, z = 0}
    }
}

local pointer = {
    map = function() return GetAddress(0x1450139A0,{ 0x50, 0x7D20 }) end,
    worldData = function() return GetAddress(0x1451C4E68,{ 0x90, 0x40, 0x90, 0x18 }) end
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
    -- pointer:worldData()未完成初始化时可能取到0x1，对初始化进行一下判断
    if not pointer:map() or not pointer:worldData() or pointer:worldData() < 0x1000 then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer:worldData() + 0x200, 'float'),
        y = GetAddressData(pointer:worldData() + 0x204, 'float'),
        z = GetAddressData(pointer:worldData() + 0x208, 'float')
    }
end
--监听
local function traceHandle(k,v)
    
end

local index = {}
local mt = {
	__index = function(t, k)
		return t[index][k]
	end,
    __newindex = function (t,k,v)
        traceHandle(k,v)
    	t[index][k] = v
    end
}
local function trace(t)
	local proxy = {}
	proxy[index] = t
	setmetatable(proxy, mt)
	return proxy
end

function engine_world:new ()
    local o = {}
    --地图Id
    o.MapId = self:getMapId()
    --时间
    o.Time = self:getTime()
    --导航坐标
    o.Position = { wayPosition = self:getWayPosition() }
    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_world