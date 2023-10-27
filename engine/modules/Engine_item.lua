engine_item = {
    item = {
        id = 0,
        number = 0
    }
}

local pointer = {
    item = nil
}

--获取物品
function engine_item:getItem()
    if not pointer.item then return {
        id = 0,
        number = 0
    } end
    return {
        id = GetAddressData(pointer.item,'int'),
        number = GetAddressData(pointer.item + 4,'int')
    }
end
--监听
local function traceHandle(k,v)
    if k == 'id' then
        if pointer.item then
            SetAddressData(pointer.item,'int',v)
        end
    return end
    if k == 'number' then
        if pointer.item then
            SetAddressData(pointer.item + 4,'int',v)
        end
    return end
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

function engine_item:new(item)
    local o = {}
    pointer.item = item
    o.item = self:getItem()
    o.item = trace(o.item)
    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_item