--[[
    玩家数据
    Position                坐标信息
    Collimator              准星信息
    Weapon                  武器信息
    Equip                   装备信息
    Characteristic          属性信息

    方法
]]
engine_player = {
    Position = {
        position = {x = 0, y = 0, z = 0},
        reposition = {x = 0, y = 0, z = 0}
    },
    Collimator = {
        straightPos = {x = 0, y = 0, z = 0},
        parabolaPos = {x = 0, y = 0, z = 0},
        aimingState = 0
    },
    Weapon = {
        position = {x = 0, y = 0, z = 0},
        type = 0,
        id = 0
    },
    Armor = {
        head = 0,
        chest = 0,
        arm = 0,
        waist = 0,
        leg = 0
    },
    Characteristic = {
        health = {
            health_base = 0,
            health_current = 0,
            health_max = 0
        },
        stamina = {
            stamina_current = 0,
            stamina_max = 0,
            stamina_eat = 0
        }
    }
}

local pointer = {
    Player = function() return GetAddress(0x145073ED0,{ 0x50 }) end,
    Weapon = {
        Entity = function() return GetAddress(0x145073ED0,{ 0x50, 0x76B0 }) end,
        Data = function() return GetAddress(0x145073ED0,{ 0x50, 0xc0, 0x8, 0x78 }) end
    }
}

--获取玩家坐标
function engine_player:getPlayerPosition()
    return {
        x = GetAddressData(pointer:Player() + 0x160, 'float'),
        y = GetAddressData(pointer:Player() + 0x164, 'float'),
        z = GetAddressData(pointer:Player() + 0x168, 'float')
    }
end
--获取遣返坐标
function engine_player:getPlayerRepatriatePos()
    return {
        x = GetAddressData(pointer:Player() + 0xA50, 'float'),
        y = GetAddressData(pointer:Player() + 0xA54, 'float'),
        z = GetAddressData(pointer:Player() + 0xA58, 'float')
    }
end
--获取玩家准星指向坐标
function engine_player:getPlayerCollimatorPos()
    return {
        x = GetAddressData(pointer:Player() + 0x7D30, 'float'),
        y = GetAddressData(pointer:Player() + 0x7D34, 'float'),
        z = GetAddressData(pointer:Player() + 0x7D38, 'float')
    }
end
--获取玩家抛物线准星指向坐标
function engine_player:getPlayerParabolaCollimatorPos()
    return {
        x = GetAddressData(pointer:Player() + 0x7D40, 'float'),
        y = GetAddressData(pointer:Player() + 0x7D44, 'float'),
        z = GetAddressData(pointer:Player() + 0x7D48, 'float')
    }
end
--获取玩家瞄准状态
function engine_player:getPlayerAimingState()
    return GetAddressData(GetAddress(pointer:Player(), { 0xC0 }) + 0xC28, 'bool')
end
--获取玩家武器数据
function engine_player:getPlayerWeaponInfo()
    return {
        --武器坐标
        position = {
            x = GetAddressData(pointer.Weapon:Entity() + 0x160, 'float'),
            y = GetAddressData(pointer.Weapon:Entity() + 0x164, 'float'),
            z = GetAddressData(pointer.Weapon:Entity() + 0x168, 'float')
        },
        --武器类型
        type = GetAddressData(pointer.Weapon:Data() + 0x2E8, 'int'),
        --武器Id
        id = GetAddressData(pointer.Weapon:Data() + 0x2EC, 'int')
    }
end
--获取玩家装备信息
function engine_player:getPlayerArmorInfo()
    return {
        --头id
        head = GetAddressData(GetAddress(pointer:Player(), { 0x12610 }) + 0xCC, 'int'),
        --胸id
        chest =GetAddressData(GetAddress(pointer:Player(), { 0x12610 }) + 0xD0, 'int'),
        --手id
        arm = GetAddressData(GetAddress(pointer:Player(), { 0x12610 }) + 0xD4, 'int'),
        --腰id
        waist = GetAddressData(GetAddress(pointer:Player(), { 0x12610 }) + 0xD8, 'int'),
        --鞋id
        leg = GetAddressData(GetAddress(pointer:Player(), { 0x12610 }) + 0xDC, 'int'),
    }
end
--获取玩家状态信息
function engine_player:getPlayerCharacteristic()
    return {
        health = {
            health_base = GetAddressData(pointer:Player() + 0x7628, 'float'),
            health_current = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x64, 'float'),
            health_max = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x60, 'float'),
        },
        stamina = {
            stamina_current = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x13C, 'float'),
            stamina_max = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x144, 'float'),
            stamina_eat = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x14C, 'float'),
        }
    }
end

--监听
local function traceHandle(k,v)
    --耐力修改
    if k == 'stamina_current' then SetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x13C, 'float', v) return end
    if k == 'stamina_max' then SetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x144, 'float', v) return end
    --健康修改
    if k == 'health_base' then SetAddressData(pointer:Player() + 0x7628, 'float', v) return end
    if k == 'health_current' then SetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x64, 'float', v) return end
    --坐标修改
    if k == 'position' then
        SetAddressData(pointer:Player() + 0x160,'float',v.x)
        SetAddressData(pointer:Player() + 0x164,'float',v.y)
        SetAddressData(pointer:Player() + 0x168,'float',v.z)
        return
    end
    --遣返坐标修改
    if k == 'reposition' then
        SetAddressData(pointer:Player() + 0xA50,'float',v.x)
        SetAddressData(pointer:Player() + 0xA54,'float',v.y)
        SetAddressData(pointer:Player() + 0xA58,'float',v.z)
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
	local proxy = {}   --代理
	proxy[index] = t
	setmetatable(proxy, mt)
	return proxy
end

function engine_player:new()
    local o = {}
    --玩家坐标
    o.Position = {
        position = self:getPlayerPosition(),
        reposition = self:getPlayerRepatriatePos()
    }
    --玩家瞄准信息
    o.Collimator = {
        --直线坐标
        straightPos = self:getPlayerCollimatorPos(),
        --抛物线坐标
        parabolaPos = self:getPlayerParabolaCollimatorPos(),
        --瞄准状态
        aimingState = self:getPlayerAimingState()
    }
    --玩家武器
    o.Weapon = self:getPlayerWeaponInfo()
    --玩家装备
    o.Armor = self:getPlayerArmorInfo()
    --玩家状态
    o.Characteristic = self:getPlayerCharacteristic()

    --创建监听
    o.Characteristic.health = trace(o.Characteristic.health)
    o.Characteristic.stamina = trace(o.Characteristic.stamina)
    o.Position = trace(o.Position)
    
    setmetatable(o, self)
    self.__index = self
    return o
end
return engine_player