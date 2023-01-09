--[[
    实体数据
    Position                坐标信息
    Model                   模型信息
    Characteristic          属性信息
    Action                  动作信息
    Frame                   动作帧信息

    方法
]]
engine_entity = {
    Position = {
        position = {x = 0, y = 0, z = 0},
        reposition = {x = 0, y = 0, z = 0}
    },
    Model = {
        size = {x = 0, y = 0, z = 0}
    },
    Action = {
        lmtID = 0,
        fsm = {
            fsmID = 0,
            fsmTarget = 0
        }
    },
    Frame = {
        frame = 0,
        frameEnd = 0,
        frameSpeed = 0
    }
}

local pointer = {
    Entity = nil,
}

--获取实体坐标
function engine_entity:getEntityPosition()
    if pointer:Entity == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer:Entity + 0x160, 'float'),
        y = GetAddressData(pointer:Entity + 0x164, 'float'),
        z = GetAddressData(pointer:Entity + 0x168, 'float')
    }
end
--获取遣返坐标
function engine_entity:getEntityRepatriatePos()
    if pointer:Entity == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer:Entity + 0xA50, 'float'),
        y = GetAddressData(pointer:Entity + 0xA54, 'float'),
        z = GetAddressData(pointer:Entity + 0xA58, 'float')
    }
end
--获取实体模型大小
function engine_entity:getEntityModelSize()
    if pointer:Entity == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer:Entity + 0x180, 'float'),
        y = GetAddressData(pointer:Entity + 0x184, 'float'),
        z = GetAddressData(pointer:Entity + 0x188, 'float')
    }
end
--获取实体动作信息
function engine_entity:getEntityActionInfo()
    if pointer:Entity == nil then return {lmtID = 0, fsm = {fsmID = 0, fsmTarget = 0}} end
    return {
        lmtID = GetAddressData(GetAddress(pointer:Entity, { 0x468 }) + 0xE9C4, 'int'),
        fsm = {
            fsmID = GetAddressData(pointer:Entity + 0x6278, 'int'),
            fsmTarget = GetAddressData(pointer:Entity + 0x6274, 'int')
        }
    }
end
--获取动作帧信息
function engine_entity:getEntityFrameInfo()
    return {
        frame = GetAddressData(GetAddress(pointer:Entity, { 0x468 }) + 0x10C, 'float'),
        frameEnd = GetAddressData(GetAddress(pointer:Entity, { 0x468 }) + 0x114, 'float'),
        frameSpeed = GetAddressData(pointer:Entity + 0x6c, 'float')
    }
end

--监听
local function traceHandle(k,v)
    if pointer:Entity == nil then return end
    --坐标修改
    if k == 'position' then
        SetAddressData(pointer:Entity + 0x160,'float',v.x)
        SetAddressData(pointer:Entity + 0x164,'float',v.y)
        SetAddressData(pointer:Entity + 0x168,'float',v.z)
        return
    end
    --模型大小修改
    if k == 'size' then
        SetAddressData(pointer:Entity + 0x180,'float',v.x)
        SetAddressData(pointer:Entity + 0x184,'float',v.y)
        SetAddressData(pointer:Entity + 0x188,'float',v.z)
        return
    end
    --遣返坐标修改
    if k == 'reposition' then
        SetAddressData(pointer:Entity + 0xA50,'float',v.x)
        SetAddressData(pointer:Entity + 0xA54,'float',v.y)
        SetAddressData(pointer:Entity + 0xA58,'float',v.z)
        return
    end
    --动作帧修改
    if k == 'frame' then
        SetAddressData(GetAddress(pointer:Entity, { 0x468 }) + 0x10C,'float',v)
        return
    end
    
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

function engine_entity:new(entity)
    local o = {}
    self:pointer.Entity = entity
    --实体坐标
    o.Position = {
        position = self:getEntityPosition(),
        reposition = self:getEntityRepatriatePos()
    }
    --实体模型
    o.Model = {
        size = self:getEntityModelSize(),
    }
    --实体动作
    o.Action = self:getEntityActionInfo()
    --实体动作帧
    o.Frame = self:getEntityFrameInfo()

    --创建监听
    o.Position = trace(o.Position)
    o.Model = trace(o.Model)

    setmetatable(o, self)
    self.__index = self
    return o
end
return engine_entity