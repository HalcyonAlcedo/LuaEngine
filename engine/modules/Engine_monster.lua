--[[
    怪物数据
    Position                坐标信息
    Model                   模型信息
    Characteristic          属性信息
    Action                  动作信息
    Frame                   动作帧信息

    方法
]]
engine_monster = {
    Position = {
        position = {x = 0, y = 0, z = 0},
        reposition = {x = 0, y = 0, z = 0}
    },
    Model = {
        size = {x = 0, y = 0, z = 0}
    },
    Characteristic = {
        health_current = 0,
        health_max = 0
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
    Monster = nil
}

--获取怪物坐标
function engine_monster:getMonsterPosition()
    if pointer.Monster == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer.Monster + 0x160, 'float'),
        y = GetAddressData(pointer.Monster + 0x164, 'float'),
        z = GetAddressData(pointer.Monster + 0x168, 'float')
    }
end
--获取遣返坐标
function engine_monster:getMonsterRepatriatePos()
    if pointer.Monster == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer.Monster + 0xA50, 'float'),
        y = GetAddressData(pointer.Monster + 0xA54, 'float'),
        z = GetAddressData(pointer.Monster + 0xA58, 'float')
    }
end
--获取怪物模型大小
function engine_monster:getMonsterModelSize()
    if pointer.Monster == nil then return {x = 0, y = 0, z = 0} end
    return {
        x = GetAddressData(pointer.Monster + 0x180, 'float'),
        y = GetAddressData(pointer.Monster + 0x184, 'float'),
        z = GetAddressData(pointer.Monster + 0x188, 'float')
    }
end
--获取怪物状态信息
function engine_monster:getMonsterCharacteristic()
    if pointer.Monster == nil then return {health_current = 0, health_max = 0} end
    return {
        health_current = GetAddressData(GetAddress(pointer.Monster, { 0x7630 }) + 0x64, 'float'),
        health_max = GetAddressData(GetAddress(pointer.Monster, { 0x7630 }) + 0x60, 'float'),
    }
end
--获取怪物动作信息
function engine_monster:getMonsterActionInfo()
    if pointer.Monster == nil then return {lmtID = 0, fsm = {fsmID = 0, fsmTarget = 0}} end
    return {
        lmtID = GetAddressData(GetAddress(pointer.Monster, { 0x468 }) + 0xE9C4, 'int'),
        fsm = {
            fsmID = GetAddressData(pointer.Monster + 0x6278, 'int'),
            fsmTarget = GetAddressData(pointer.Monster + 0x6274, 'int')
        }
    }
end
--获取动作帧信息
function engine_monster:getMonsterFrameInfo()
    return {
        frame = GetAddressData(GetAddress(pointer.Monster, { 0x468 }) + 0x10C, 'float'),
        frameEnd = GetAddressData(GetAddress(pointer.Monster, { 0x468 }) + 0x114, 'float'),
        frameSpeed = GetAddressData(pointer.Monster + 0x6c, 'float')
    }
end

--监听
local function traceHandle(k,v)
    if pointer.Monster == nil then return end
    --健康修改
    if k == 'health_base' then SetAddressData(pointer.Monster + 0x7628, 'float', v) return end
    if k == 'health_current' then SetAddressData(GetAddress(pointer.Monster, { 0x7630 }) + 0x64, 'float', v) return end
    --坐标修改
    if k == 'position' then
        SetAddressData(pointer.Monster + 0x160,'float',v.x)
        SetAddressData(pointer.Monster + 0x164,'float',v.y)
        SetAddressData(pointer.Monster + 0x168,'float',v.z)
        return
    end
    --模型大小修改
    if k == 'size' then
        SetAddressData(pointer.Monster + 0x180,'float',v.x)
        SetAddressData(pointer.Monster + 0x184,'float',v.y)
        SetAddressData(pointer.Monster + 0x188,'float',v.z)
        return
    end
    --遣返坐标修改
    if k == 'reposition' then
        SetAddressData(pointer.Monster + 0xA50,'float',v.x)
        SetAddressData(pointer.Monster + 0xA54,'float',v.y)
        SetAddressData(pointer.Monster + 0xA58,'float',v.z)
        return
    end
    --动作帧修改
    if k == 'frame' then
        SetAddressData(GetAddress(pointer.Monster, { 0x468 }) + 0x10C,'float',v)
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

function engine_monster:new(monster)
    local o = {}
    pointer.Monster = monster
    --怪物坐标
    o.Position = {
        position = self:getMonsterPosition(),
        reposition = self:getMonsterRepatriatePos()
    }
    --怪物模型
    o.Model = {
        size = self:getMonsterModelSize(),
    }
    --怪物状态
    o.Characteristic = self:getMonsterCharacteristic()
    --怪物动作
    o.Action = self:getMonsterActionInfo()
    --怪物动作帧
    o.Frame = self:getMonsterFrameInfo()

    --创建监听
    o.Characteristic.health = trace(o.Characteristic.health)
    o.Position = trace(o.Position)
    o.Model = trace(o.Model)

    setmetatable(o, self)
    self.__index = self
    return o
end
return engine_monster