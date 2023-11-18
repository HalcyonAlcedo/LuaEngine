--[[
    玩家数据
    Position                坐标信息
    Model                   模型信息
    Collimator              准星信息
    Angle                   角度信息
    Weapon                  武器信息
    Equip                   装备信息
    Characteristic          属性信息
    Action                  动作信息
    Gravity                 重力信息
    Frame                   动作帧信息

    方法
    AimPosition             使得玩家朝向点
]]
engine_player = {
    Position = {
        position = {x = 0, y = 0, z = 0},
        cntrposition = {x = 0, y = 0, z = 0, h = 0},
        reposition = {x = 0, y = 0, z = 0}
    },
    Model = {
        size = {x = 0, y = 0, z = 0},
    },
    Collimator = {
        straightPos = {x = 0, y = 0, z = 0},
        parabolaPos = {x = 0, y = 0, z = 0},
        aimingState = false
    },
    Angle = {
        Quaternion = {w = 0, x = 0, y = 0, z = 0},
        Eulerian = {x = 0, y = 0, z = 0}
    },
    Weapon = {
        position = {x = 0, y = 0, z = 0},
        type = 0,
        id = 0,
        hit = 0
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
    },
    Action = {
        lmtID = 0,
        fsm = {
            fsmID = 0,
            fsmTarget = 0
        },
        useItem = 0
    },
    Gravity = {
        gravity = 0,
        fall = 0,
        liftoff = false
    },
    Frame = {
        frame = 0,
        frameEnd = 0,
        frameSpeed = 0,
        frameSpeedMultiplies = 0
    }
}

local pointer = {
    Player = function() return GetAddress(0x145011760,{ 0x50 }) end,
    PlayerData = function() return GetAddress(0x1451CA0E0,{ 0x48, 0x58, 0x58, 0x40, 0xD0, 0x8 }) end,
    Weapon = {
        Entity = function() return GetAddress(0x145011760,{ 0x50, 0x76B0 }) end,
        Data = function() return GetAddress(0x145011760,{ 0x50, 0xc0, 0x8, 0x78 }) end
    }
}

--获取玩家坐标
function engine_player:getPlayerPosition()
    if not pointer:Player() then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x160, 'float'),
        y = GetAddressData(pointer:Player() + 0x164, 'float'),
        z = GetAddressData(pointer:Player() + 0x168, 'float')
    }
end
--获取玩家中心点坐标
function engine_player:getPlayerCNTRPosition()
    if not pointer:Player() then return { x = 0, y = 0, z = 0, h = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x390, 'float'),
        y = GetAddressData(pointer:Player() + 0x394, 'float'),
        z = GetAddressData(pointer:Player() + 0x398, 'float'),
        h = GetAddressData(pointer:Player() + 0x39c, 'float')
    }
end
--获取遣返坐标
function engine_player:getPlayerRepatriatePos()
    if not pointer:Player() then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0xA50, 'float'),
        y = GetAddressData(pointer:Player() + 0xA54, 'float'),
        z = GetAddressData(pointer:Player() + 0xA58, 'float')
    }
end
--获取玩家模型大小
function engine_player:getPlayerModelSize()
    if not pointer:Player() then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x180, 'float'),
        y = GetAddressData(pointer:Player() + 0x184, 'float'),
        z = GetAddressData(pointer:Player() + 0x188, 'float')
    }
end
--获取玩家准星指向坐标
function engine_player:getPlayerCollimatorPos()
    if not pointer:Player() then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x7D30, 'float'),
        y = GetAddressData(pointer:Player() + 0x7D34, 'float'),
        z = GetAddressData(pointer:Player() + 0x7D38, 'float')
    }
end
--获取玩家四元数角
function engine_player:getPlayerQuaternion()
    if not pointer:Player() then return { x = 0, y = 0, z = 0, w = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x174, 'float'),
        y = GetAddressData(pointer:Player() + 0x178, 'float'),
        z = GetAddressData(pointer:Player() + 0x17C, 'float'),
        w = GetAddressData(pointer:Player() + 0x180, 'float')
    }
end
--获取玩家欧拉角
function engine_player:getPlayerEulerian()
    local quaternion = self:getPlayerQuaternion()
    local eulerangles = {x = 0, y = 0, z = 0}
    local sinr_cosp = 2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z)
    local cosr_cosp = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y)
    eulerangles.x = math.atan(sinr_cosp, cosr_cosp)
    local sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x)
    if math.abs(sinp) >= 1 then
        eulerangles.y = (sinp >= 0 and {math.pi / 2} or {-math.pi / 2})[1]
    else
        eulerangles.y = math.asin(sinp);
    end
    local siny_cosp = 2 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y)
    local cosy_cosp = 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z)
    eulerangles.z = math.atan(siny_cosp, cosy_cosp);
    return eulerangles
end
--获取玩家抛物线准星指向坐标
function engine_player:getPlayerParabolaCollimatorPos()
    if not pointer:Player() then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer:Player() + 0x7D40, 'float'),
        y = GetAddressData(pointer:Player() + 0x7D44, 'float'),
        z = GetAddressData(pointer:Player() + 0x7D48, 'float')
    }
end
--获取玩家瞄准状态
function engine_player:getPlayerAimingState()
    if not pointer:Player() then return false end
    return GetAddressData(GetAddress(pointer:Player(), { 0xC0 }) + 0xC28, 'bool')
end
--获取玩家武器数据
function engine_player:getPlayerWeaponInfo()
    --地址检查
    if pointer.Weapon:Entity() and pointer.Weapon:Data() then
        local player_weapon_info = {
            --武器坐标
            position = {
                x = GetAddressData(pointer.Weapon:Entity() + 0x160, 'float'),
                y = GetAddressData(pointer.Weapon:Entity() + 0x164, 'float'),
                z = GetAddressData(pointer.Weapon:Entity() + 0x168, 'float')
            },
            --武器类型
            type = GetAddressData(pointer.Weapon:Data() + 0x2E8, 'int'),
            --武器Id
            id = GetAddressData(pointer.Weapon:Data() + 0x2EC, 'int'),
            --武器命中的怪物地址
            hit = GetAddress(pointer:PlayerData(), { 0x2C8 })
        }
        if not player_weapon_info.hit then player_weapon_info.hit = 0 end
        if player_weapon_info.position.x
            and player_weapon_info.position.y
            and player_weapon_info.position.z
            and player_weapon_info.type
            and player_weapon_info.id
        then
            return player_weapon_info
        end
    end
    return {
        position = {x = 0, y = 0, z = 0},
        type = 0,
        id = 0,
        hit = 0
    }
end
--获取玩家装备信息
function engine_player:getPlayerArmorInfo()
    if not pointer:Player() then return { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 } end
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
    if not pointer:Player() then return {
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
    } end
    return {
        health = {
            health_base = GetAddressData(pointer:Player() + 0x7628, 'float'),
            health_current = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x64, 'float'),
            health_max = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x60, 'float'),
        },
        stamina = {
            stamina_current = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x12C, 'float'),
            stamina_max = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x134, 'float'),
            stamina_eat = GetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x13C, 'float'),
        }
    }
end
--获取玩家动作信息
function engine_player:getPlayerActionInfo()
    if not pointer:Player() then return {
        lmtID = 0,
        fsm = {
            fsmID = 0,
            fsmTarget = 0
        },
        useItem = 0
    } end
    return {
        lmtID = GetAddressData(GetAddress(pointer:Player(), { 0x468 }) + 0xE9C4, 'int'),
        fsm = {
            fsmID = GetAddressData(pointer:Player() + 0x6278, 'int'),
            fsmTarget = GetAddressData(pointer:Player() + 0x6274, 'int')
        },
        useItem = GetAddressData(pointer:Player() + 0xb780, 'int')
    }
end
--获取重力信息
function engine_player:getPlayerGravityInfo()
    if not pointer:Player() then return {
        gravity = 0,
        fall = 0,
        liftoff = false
    } end
    return {
        gravity = GetAddressData(pointer:Player() + 0x14B0, 'float'),
        fall = GetAddressData(pointer:Player() + 0xE178, 'float'),
        liftoff = GetAddressData(pointer:Player() + 0x112C, 'bool')
    }
end
--获取动作帧信息
function engine_player:getPlayerFrameInfo()
    if not pointer:Player() then return {
        frame = 0,
        frameEnd = 0,
        frameSpeed = 0,
        frameSpeedMultiplies = 0
    } end
    return {
        frame = GetAddressData(GetAddress(pointer:Player(), { 0x468 }) + 0x10C, 'float'),
        frameEnd = GetAddressData(GetAddress(pointer:Player(), { 0x468 }) + 0x114, 'float'),
        frameSpeed = GetAddressData(pointer:Player() + 0x6c, 'float'),
        frameSpeedMultiplies = GetAddressData(GetAddressData(0x145121688, 'int') + GetAddressData(pointer:Player() + 0x10, 'int') * 0xf8 + 0x9c, 'float')
    }
end

--监听
local function traceHandle(k,v)
    --耐力修改
    if k == 'stamina_current' then SetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x12C, 'float', v) return end
    if k == 'stamina_max' then SetAddressData(GetAddress(pointer:Player(), { 0x7630 }) + 0x134, 'float', v) return end
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
    --模型大小修改
    if k == 'size' then
        SetAddressData(pointer:Player() + 0x180,'float',v.x)
        SetAddressData(pointer:Player() + 0x184,'float',v.y)
        SetAddressData(pointer:Player() + 0x188,'float',v.z)
        return
    end
    --四元数角修改
    if k == 'Quaternion' then
        SetAddressData(pointer:Player() + 0x174,'float',v.x)
        SetAddressData(pointer:Player() + 0x178,'float',v.y)
        SetAddressData(pointer:Player() + 0x17C,'float',v.z)
        SetAddressData(pointer:Player() + 0x180,'float',v.w)
    end
    --动作修改
    if k == 'lmtID' then
        RunLmtAction(v)
        return
    end
    if k == 'fsm' then
        RunFsmAction(v.fsmTarget, v.fsmID)
        return
    end
    --重力修改
    if k == 'gravity' then
        SetAddressData(pointer:Player() + 0x14B0,'float',v)
        return
    end
    if k == 'fall' then
        SetAddressData(pointer:Player() + 0xE178,'float',v)
        return
    end
    --动作帧修改
    if k == 'frame' then
        SetAddressData(GetAddress(pointer:Player(), { 0x468 }) + 0x10C,'float',v)
        return
    end
    --动作帧速率倍率修改
    if k == 'frameSpeedMultiplies' then
        SetAddressData(
            GetAddressData(0x145121688, 'int') + GetAddressData(pointer:Player() + 0x10, 'int') * 0xf8 + 0x9c
        ,'float',v)
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

function engine_player:AimPosition(pos)
    local direction_x = pos.x - self.Position.position.x
    local direction_z = pos.z - self.Position.position.z
    local aim_angle = math.atan(direction_x/direction_z)

    local sign = function(x) if x < o then return -1 elseif x == 0 then return 0 else return 1 end end
    local a2q = function(angle) if angle / math.pi > 0.5 then return {x = math.pi, y = angle - math.pi, z = 0} elseif angle / math.pi < -0.5 then return {x = math.pi, y = angle + math.pi, z = 0} else return {x = 0, y = -angle, z = math.pi} end end
    aim_angle = aim_angle + sign(direction_x) * (1 - sing(direction_z)) * math.pi / 2
    
    quaternion = a2q(quaternion)
    self.Angle.Quaternion = {
        self.Angle.Quaternion.w,
        quaternion.x,
        self.Angle.Quaternion.y,
        quaternion.z
    }
end

function engine_player:new()
    local o = {}
    --玩家坐标
    o.Position = {
        position = self:getPlayerPosition(),
        cntrposition = self:getPlayerCNTRPosition(),
        reposition = self:getPlayerRepatriatePos()
    }
    --玩家模型
    o.Model = {
        size = self:getPlayerModelSize(),
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
    --玩家角度
    o.Angle = {
        Quaternion = self:getPlayerQuaternion(),
        Eulerian = self:getPlayerEulerian()
    }
    --玩家武器
    o.Weapon = self:getPlayerWeaponInfo()
    --玩家装备
    o.Armor = self:getPlayerArmorInfo()
    --玩家状态
    o.Characteristic = self:getPlayerCharacteristic()
    --玩家动作
    o.Action = self:getPlayerActionInfo()
    --玩家重力
    o.Gravity = self:getPlayerGravityInfo()
    --玩家动作帧
    o.Frame = self:getPlayerFrameInfo()

    --创建监听
    o.Characteristic.health = trace(o.Characteristic.health)
    o.Characteristic.stamina = trace(o.Characteristic.stamina)
    o.Position = trace(o.Position)
    o.Model = trace(o.Model)
    o.Action = trace(o.Action)
    o.Gravity = trace(o.Gravity)
    o.Frame = trace(o.Frame)
    
    setmetatable(o, self)
    self.__index = self
    return o
end
return engine_player