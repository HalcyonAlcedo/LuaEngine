--[[
    玩家数据
    Position                玩家坐标信息
    Collimator              玩家准星信息
    Weapon                  玩家武器信息

    方法
    setPos(x, y, z)         设置玩家坐标
]]
engine_player = {
    Position = {x = 0, y = 0, z = 0},
    Collimator = {
        straightPos = {x = 0, y = 0, z = 0},
        parabolaPos = {x = 0, y = 0, z = 0},
        aimingState = 0
    },
    Weapon = {
        Position = {x = 0, y = 0, z = 0},
        Type = 0,
        Id = 0
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
    local Pos_X = GetAddressData(pointer:Player() + 0x160, 'float')
    local Pos_Y = GetAddressData(pointer:Player() + 0x164, 'float')
    local Pos_Z = GetAddressData(pointer:Player() + 0x168, 'float')
    return {x = Pos_X, y = Pos_Y, z = Pos_Z}
end
--获取玩家准星指向坐标
function engine_player:getPlayerCollimatorPos()
    local Pos_X = GetAddressData(pointer:Player() + 0x7D30, 'float')
    local Pos_Y = GetAddressData(pointer:Player() + 0x7D34, 'float')
    local Pos_Z = GetAddressData(pointer:Player() + 0x7D38, 'float')
    return {x = Pos_X, y = Pos_Y, z = Pos_Z}
end
--获取玩家抛物线准星指向坐标
function engine_player:getPlayerParabolaCollimatorPos()
    local Pos_X = GetAddressData(pointer:Player() + 0x7D40, 'float')
    local Pos_Y = GetAddressData(pointer:Player() + 0x7D44, 'float')
    local Pos_Z = GetAddressData(pointer:Player() + 0x7D48, 'float')
    return {x = Pos_X, y = Pos_Y, z = Pos_Z}
end
--获取玩家瞄准状态
function engine_player:getPlayerAimingState()
    local AimingStatePlot = GetAddress(pointer:Player(), { 0xC0 })
    return GetAddressData(AimingStatePlot + 0xC28, 'bool')
end
--获取玩家武器坐标
function engine_player:getPlayerWeaponPos()
    local Pos_X = GetAddressData(pointer.Weapon:Entity() + 0x160, 'float')
    local Pos_Y = GetAddressData(pointer.Weapon:Entity() + 0x164, 'float')
    local Pos_Z = GetAddressData(pointer.Weapon:Entity() + 0x168, 'float')
    return {x = Pos_X, y = Pos_Y, z = Pos_Z}
end
--获取玩家武器Id和类型
function engine_player:getPlayerWeaponType()
    local Type = GetAddressData(pointer.Weapon:Data() + 0x2E8, 'int')
    return Type
end
function engine_player:getPlayerWeaponId()
    local Id = GetAddressData(pointer.Weapon:Data() + 0x2EC, 'int')
    return Id
end


function engine_player:new ()
    local o = {}
    --玩家坐标
    o.Position = self:getPlayerPosition()
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
    o.Weapon = {
        --武器坐标
        Position = self:getPlayerWeaponPos(),
        --武器类型
        Type = self:getPlayerWeaponType(),
        --武器Id
        Id = self:getPlayerWeaponId()
    }
    setmetatable(o, self)
    self.__index = self
    return o
end

--设置玩家坐标
function engine_player:setPos(x, y, z, pierce = false)
    SetAddressData(pointer:Player() + 0x160,'float',x)
    SetAddressData(pointer:Player() + 0x164,'float',y)
    SetAddressData(pointer:Player() + 0x168,'float',z)
    if pierce then
        SetAddressData(pointer:Player() + 0xA50,'float',x)
        SetAddressData(pointer:Player() + 0xA54,'float',y)
        SetAddressData(pointer:Player() + 0xA58,'float',z)
    end
end

return engine_player