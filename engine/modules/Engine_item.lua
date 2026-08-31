
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

engine_item = {
    info = {
        name = 'Item'
    },
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