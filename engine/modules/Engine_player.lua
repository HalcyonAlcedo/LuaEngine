--[[
    玩家数据
    Position                玩家坐标信息
    Collimator              玩家准星信息
    Weapon                  玩家武器信息

    方法
    setPos(x, y, z)         设置玩家坐标
]]
engine_player = {}
engine_player.pointer = {
    Player = GetAddress(0x145073ED0,{ 0x50 }),
    Weapon = {
        Entity = GetAddress(0x145073ED0,{ 0x50, 0x76B0 }),
        Data = GetAddress(0x145073ED0,{ 0x50, 0xc0, 0x8, 0x78 })
    }
}
--获取玩家坐标
local function getPlayerPosition()
    local Pos_X = GetAddressData(engine_player.pointer.Player + 0x160, 'float')
    local Pos_Y = GetAddressData(engine_player.pointer.Player + 0x164, 'float')
    local Pos_Z = GetAddressData(engine_player.pointer.Player + 0x168, 'float')
    return {x: Pos_X, y: Pos_Y,z: Pos_Z}
end
--获取玩家准星指向坐标
local function getPlayerCollimatorPos()
    local Pos_X = GetAddressData(engine_player.pointer.Player + 0x7D30, 'float')
    local Pos_Y = GetAddressData(engine_player.pointer.Player + 0x7D34, 'float')
    local Pos_Z = GetAddressData(engine_player.pointer.Player + 0x7D38, 'float')
    return {x: Pos_X, y: Pos_Y,z: Pos_Z}
end
--获取玩家抛物线准星指向坐标
local function getPlayerParabolaCollimatorPos()
    local Pos_X = GetAddressData(engine_player.pointer.Player + 0x7D40, 'float')
    local Pos_Y = GetAddressData(engine_player.pointer.Player + 0x7D44, 'float')
    local Pos_Z = GetAddressData(engine_player.pointer.Player + 0x7D48, 'float')
    return {x: Pos_X, y: Pos_Y,z: Pos_Z}
end
--获取玩家武器坐标
local function getPlayerWeaponPos()
    local Pos_X = GetAddressData(engine_player.pointer.Weapon.Entity + 0x160, 'float')
    local Pos_Y = GetAddressData(engine_player.pointer.Weapon.Entity + 0x164, 'float')
    local Pos_Z = GetAddressData(engine_player.pointer.Weapon.Entity + 0x168, 'float')
    return {x: Pos_X, y: Pos_Y,z: Pos_Z}
end
--获取玩家瞄准状态
local function getPlayerAimingState()
    local AimingStatePlot = GetAddress(engine_player.pointer.Player,{ 0xC0 })
    return GetAddressData(AimingStatePlot + 0xC28, 'bool')
end


--玩家坐标
engine_player.Position = getPlayerPosition()
function engine_player.setPos(x, y, z)
    SetAddressData(engine_player.pointer.Player + 0x160,'float',x)
    SetAddressData(engine_player.pointer.Player + 0xA50,'float',x)
    SetAddressData(engine_player.pointer.Player + 0x164,'float',y)
    SetAddressData(engine_player.pointer.Player + 0xA54,'float',y)
    SetAddressData(engine_player.pointer.Player + 0x168,'float',z)
    SetAddressData(engine_player.pointer.Player + 0xA58,'float',z)
end
--玩家瞄准信息
engine_player.Collimator = {
    --直线坐标
    straightPos: getPlayerCollimatorPos(),
    --抛物线坐标
    parabolaPos: getPlayerParabolaCollimatorPos(),
    --瞄准状态
    aimingState: getPlayerAimingState()
}
--玩家武器
engine_player.Weapon = {
    --武器坐标
    Position: getPlayerWeaponPos()
}

return engine_player