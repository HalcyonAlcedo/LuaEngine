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
	local proxy = {}   --代理
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

    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_world