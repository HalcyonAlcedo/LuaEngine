--[[
    任务数据
    Time                    任务时间
    Id                      任务Id
    State                   任务状态

    方法
    SetState
]]
engine_quest = {
    Time = 0,
    Id = 0,
    State = 0
}

local pointer = {
    time = function() return GetAddress(0x145073ED0,{ 0x50, 0x7D20 }) end,
    quest = function() return 0x14506F240 end
}

--获取当前任务时间
function engine_quest:getTime()
    local time = GetAddressData(pointer:time() + 0xC24, 'float')
    return time
end
--获取任务Id
function engine_quest:getId()
    local id = GetAddressData(pointer:quest() + 0x4C, 'int')
    return id
end
--获取任务状态
function engine_quest:getState()
    local state = GetAddressData(pointer:quest() + 0x54, 'int')
    return state
end



--设置任务状态
function engine_quest:setState(state)
    SetAddressData(self.pointer.Quest + 0x38,'int',state)
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

function engine_quest:new ()
    local o = {}
    --时间
    o.Time = self:getTime()
    --Id
    o.Id = self:getId()
    --状态
    o.State = self:getState()

    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_quest