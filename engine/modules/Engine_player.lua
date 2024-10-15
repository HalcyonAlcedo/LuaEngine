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
    info = {
        name = 'Player'
    },
    Position = {
        position = { x = 0, y = 0, z = 0 },
        cntrposition = { x = 0, y = 0, z = 0, h = 0 },
        reposition = { x = 0, y = 0, z = 0 },
        incremental = { x = 0, y = 0, z = 0 }
    },
    Model = {
        size = { x = 0, y = 0, z = 0 },
    },
    Collimator = {
        straightPos = { x = 0, y = 0, z = 0 },
        parabolaPos = { x = 0, y = 0, z = 0 },
        aimingState = false
    },
    Angle = {
        Quaternion = { w = 0, x = 0, y = 0, z = 0 },
        Eulerian = { x = 0, y = 0, z = 0 }
    },
    Weapon = {
        position = { x = 0, y = 0, z = 0 },
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
    Layered = {
        layeredHead = 0,
        layeredChest = 0,
        layeredArm = 0,
        layeredWaist = 0,
        layeredLeg = 0
    },
    TempArmorData = {
        layered = {
            head = 0,
            chest = 0,
            arm = 0,
            waist = 0,
            leg = 0
        },
        Armor = {
            head = 0,
            chest = 0,
            arm = 0,
            waist = 0,
            leg = 0
        },
        colour = {
            head = {
                r = 0,
                g = 0,
                b = 0,
                a = 0
            },
            chest = {
                r = 0,
                g = 0,
                b = 0,
                a = 0
            },
            arm = {
                r = 0,
                g = 0,
                b = 0,
                a = 0
            },
            waist = {
                r = 0,
                g = 0,
                b = 0,
                a = 0
            },
            leg = {
                r = 0,
                g = 0,
                b = 0,
                a = 0
            }
        }
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

local aob_Save
local aob_player

local pointer = {
    Player = nil,
    PlayerData = nil,
    PlayerSaveData = nil,
    Weapon = {
        Entity = nil,
        Data = nil
    }
}

--获取玩家坐标
function engine_player:getPlayerPosition()
    if not pointer.Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0x160, 'float'),
        y = GetAddressData(pointer.Player + 0x164, 'float'),
        z = GetAddressData(pointer.Player + 0x168, 'float')
    }
end

--获取玩家中心点坐标
function engine_player:getPlayerCNTRPosition()
    if not pointer.Player then return { x = 0, y = 0, z = 0, h = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0x390, 'float'),
        y = GetAddressData(pointer.Player + 0x394, 'float'),
        z = GetAddressData(pointer.Player + 0x398, 'float'),
        h = GetAddressData(pointer.Player + 0x39c, 'float')
    }
end

--获取遣返坐标
function engine_player:getPlayerRepatriatePos()
    if not pointer.Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0xA50, 'float'),
        y = GetAddressData(pointer.Player + 0xA54, 'float'),
        z = GetAddressData(pointer.Player + 0xA58, 'float')
    }
end

--获取运动增量坐标
function engine_player:getPlayerIncrementalPos()
    if not GetAddress(pointer.Player, { 0x468 }) then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0xe250, 'float'),
        y = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0xe254, 'float'),
        z = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0xe258, 'float'),
    }
end

--获取玩家模型大小
function engine_player:getPlayerModelSize()
    if not pointer.Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0x180, 'float'),
        y = GetAddressData(pointer.Player + 0x184, 'float'),
        z = GetAddressData(pointer.Player + 0x188, 'float')
    }
end

--获取玩家准星指向坐标
function engine_player:getPlayerCollimatorPos()
    if not pointer.Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0x7D30, 'float'),
        y = GetAddressData(pointer.Player + 0x7D34, 'float'),
        z = GetAddressData(pointer.Player + 0x7D38, 'float')
    }
end

--获取玩家四元数角
function engine_player:getPlayerQuaternion()
    if not pointer.Player then return { x = 0, y = 0, z = 0, w = 0 } end
    return {
        w = GetAddressData(pointer.Player + 0x170, 'float'),
        x = GetAddressData(pointer.Player + 0x174, 'float'),
        y = GetAddressData(pointer.Player + 0x178, 'float'),
        z = GetAddressData(pointer.Player + 0x17c, 'float')
    }
end

--获取玩家欧拉角
function engine_player:getPlayerEulerian()
    local quaternion = self:getPlayerQuaternion()
    local eulerangles = { x = 0, y = 0, z = 0 }
    local sinr_cosp = 2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z)
    local cosr_cosp = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y)
    eulerangles.x = math.atan(sinr_cosp, cosr_cosp)
    local sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x)
    if math.abs(sinp) >= 1 then
        eulerangles.y = (sinp >= 0 and { math.pi / 2 } or { -math.pi / 2 })[1]
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
    if not pointer.Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(pointer.Player + 0x7D40, 'float'),
        y = GetAddressData(pointer.Player + 0x7D44, 'float'),
        z = GetAddressData(pointer.Player + 0x7D48, 'float')
    }
end

--获取玩家瞄准状态
function engine_player:getPlayerAimingState()
    if not pointer.Player then return false end
    return GetAddressData(GetAddress(pointer.Player, { 0xC0 }) + 0xC28, 'bool')
end

--获取玩家武器数据
function engine_player:getPlayerWeaponInfo()
    --地址检查
    if pointer.Weapon.Entity and pointer.Weapon.Data then
        local player_weapon_info = {
            --武器坐标
            position = {
                x = GetAddressData(pointer.Weapon.Entity + 0x160, 'float'),
                y = GetAddressData(pointer.Weapon.Entity + 0x164, 'float'),
                z = GetAddressData(pointer.Weapon.Entity + 0x168, 'float')
            },
            --武器类型
            type = GetAddressData(pointer.Weapon.Data + 0x2E8, 'int'),
            --武器Id
            id = GetAddressData(pointer.Weapon.Data + 0x2EC, 'int'),
            --武器命中的怪物地址
            --hit = GetAddress(pointer.PlayerData, { 0x2C8 })
        }
        --if not player_weapon_info.hit then player_weapon_info.hit = 0 end
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
        position = { x = 0, y = 0, z = 0 },
        type = 0,
        id = 0,
        hit = 0
    }
end

--获取玩家装备信息
function engine_player:getPlayerArmorInfo()
    if not pointer.Weapon.Entity then return { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 } end
    return {
        --头id
        head = GetAddressData(pointer.Weapon.Data + 0x1C4, 'int'),
        --胸id
        chest = GetAddressData(pointer.Weapon.Data + 0x1C8, 'int'),
        --手id
        arm = GetAddressData(pointer.Weapon.Data + 0x1CC, 'int'),
        --腰id
        waist = GetAddressData(pointer.Weapon.Data + 0x1D0, 'int'),
        --鞋id
        leg = GetAddressData(pointer.Weapon.Data + 0x1D4, 'int'),
    }
end

--获取玩家幻化信息
function engine_player:getPlayerLayeredInfo()
    if not pointer.PlayerSaveData then return { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 } end
    return {
        --头id
        layeredHead = GetAddressData(pointer.PlayerSaveData + 0xE7434, 'int'),
        --胸id
        layeredChest = GetAddressData(pointer.PlayerSaveData + 0xE7438, 'int'),
        --手id
        layeredArm = GetAddressData(pointer.PlayerSaveData + 0xE743C, 'int'),
        --腰id
        layeredWaist = GetAddressData(pointer.PlayerSaveData + 0xE7440, 'int'),
        --鞋id
        layeredLeg = GetAddressData(pointer.PlayerSaveData + 0xE7444, 'int'),
    }
end

--获取玩家临时装备信息
function engine_player:getPlayerTempArmorDataInfo()
    if not pointer.Player then
        return {
            layered = { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 },
            Armor = { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 },
            colour = { head = { r = 0, g = 0, b = 0, a = 0 }, chest = { r = 0, g = 0, b = 0, a = 0 }, arm = { r = 0, g = 0, b = 0, a = 0 }, waist = { r = 0, g = 0, b = 0, a = 0 }, leg = { r = 0, g = 0, b = 0, a = 0 } }
        }
    end
    return {
        armor = {
            head = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xCC, 'int'),
            chest = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xD0, 'int'),
            arm = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xD4, 'int'),
            waist = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xD8, 'int'),
            leg = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xDC, 'int')
        },
        layered = {
            head = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xE4, 'int'),
            chest = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xE8, 'int'),
            arm = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xEC, 'int'),
            waist = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xF0, 'int'),
            leg = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xF4, 'int')
        },
        colour = {
            head = {
                r = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x170, 'float'),
                g = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x174, 'float'),
                b = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x178, 'float'),
                a = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x17C, 'float')
            },
            chest = {
                r = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x180, 'float'),
                g = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x184, 'float'),
                b = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x188, 'float'),
                a = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x18C, 'float')
            },
            arm = {
                r = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x190, 'float'),
                g = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x194, 'float'),
                b = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x198, 'float'),
                a = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x19C, 'float')
            },
            waist = {
                r = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1A0, 'float'),
                g = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1A4, 'float'),
                b = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1A8, 'float'),
                a = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1AC, 'float')
            },
            leg = {
                r = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1B0, 'float'),
                g = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1B4, 'float'),
                b = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1B8, 'float'),
                a = GetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0x1BC, 'float')
            }
        }
    }
end

--获取玩家状态信息
function engine_player:getPlayerCharacteristic()
    if not pointer.Player then
        return {
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
    end
    return {
        health = {
            health_base = GetAddressData(pointer.Player + 0x7628, 'float'),
            health_current = GetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x64, 'float'),
            health_max = GetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x60, 'float'),
        },
        stamina = {
            stamina_current = GetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x12C, 'float'),
            stamina_max = GetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x134, 'float'),
            stamina_eat = GetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x13C, 'float'),
        }
    }
end

--获取玩家动作信息
function engine_player:getPlayerActionInfo()
    if not pointer.Player then
        return {
            lmtID = 0,
            fsm = {
                fsmID = 0,
                fsmTarget = 0
            },
            useItem = 0
        }
    end
    return {
        lmtID = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0xE9C4, 'int'),
        fsm = {
            fsmID = GetAddressData(pointer.Player + 0x6278, 'int'),
            fsmTarget = GetAddressData(pointer.Player + 0x6274, 'int')
        },
        useItem = GetAddressData(pointer.Player + 0xb780, 'int')
    }
end

--获取重力信息
function engine_player:getPlayerGravityInfo()
    if not pointer.Player then
        return {
            gravity = 0,
            fall = 0,
            liftoff = false
        }
    end
    return {
        gravity = GetAddressData(pointer.Player + 0x14B0, 'float'),
        fall = GetAddressData(pointer.Player + 0xE178, 'float'),
        liftoff = GetAddressData(pointer.Player + 0x112C, 'bool')
    }
end

--获取动作帧信息
function engine_player:getPlayerFrameInfo()
    if not pointer.Player then
        return {
            frame = 0,
            frameEnd = 0,
            frameSpeed = 0,
            frameSpeedMultiplies = 0
        }
    end
    return {
        frame = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0x10C, 'float'),
        frameEnd = GetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0x114, 'float'),
        frameSpeed = GetAddressData(pointer.Player + 0x6c, 'float'),
        frameSpeedMultiplies = GetAddressData(
            GetAddressData(0x1451238C8, 'int') + GetAddressData(pointer.Player + 0x10, 'int') * 0xf8 + 0x9c, 'float')
    }
end

--监听
local function traceHandle(k, v)
    --耐力修改
    if k == 'stamina_current' then
        SetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x12C, 'float', v)
        return
    end
    if k == 'stamina_max' then
        SetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x134, 'float', v)
        return
    end
    --健康修改
    if k == 'health_base' then
        SetAddressData(pointer.Player + 0x7628, 'float', v)
        return
    end
    if k == 'health_current' then
        SetAddressData(GetAddress(pointer.Player, { 0x7630 }) + 0x64, 'float', v)
        return
    end
    --坐标修改
    if k == 'position' then
        SetAddressData(pointer.Player + 0x160, 'float', v.x)
        SetAddressData(pointer.Player + 0x164, 'float', v.y)
        SetAddressData(pointer.Player + 0x168, 'float', v.z)
        return
    end
    --遣返坐标修改
    if k == 'reposition' then
        SetAddressData(pointer.Player + 0xA50, 'float', v.x)
        SetAddressData(pointer.Player + 0xA54, 'float', v.y)
        SetAddressData(pointer.Player + 0xA58, 'float', v.z)
        return
    end
    --模型大小修改
    if k == 'size' then
        SetAddressData(pointer.Player + 0x180, 'float', v.x)
        SetAddressData(pointer.Player + 0x184, 'float', v.y)
        SetAddressData(pointer.Player + 0x188, 'float', v.z)
        return
    end
    --四元数角修改
    if k == 'Quaternion' then
        SetAddressData(pointer.Player + 0x170, 'float', v.w)
        SetAddressData(pointer.Player + 0x174, 'float', v.x)
        SetAddressData(pointer.Player + 0x178, 'float', v.y)
        SetAddressData(pointer.Player + 0x17c, 'float', v.z)
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
        SetAddressData(pointer.Player + 0x14B0, 'float', v)
        return
    end
    if k == 'fall' then
        SetAddressData(pointer.Player + 0xE178, 'float', v)
        return
    end
    --动作帧修改
    if k == 'frame' then
        SetAddressData(GetAddress(pointer.Player, { 0x468 }) + 0x10C, 'float', v)
        return
    end
    --动作帧速率倍率修改
    if k == 'frameSpeedMultiplies' then
        SetAddressData(
            GetAddressData(0x1451238C8, 'int') + GetAddressData(pointer.Player + 0x10, 'int') * 0xf8 + 0x9c
            , 'float', v)
        return
    end
    --幻化
    if k == 'layeredHead' then
        SetAddressData(pointer.PlayerSaveData + 0xE7434, 'int', v)
        SetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xE4, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredChest' then
        SetAddressData(pointer.PlayerSaveData + 0xE7438, 'int', v)
        SetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xE8, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredArm' then
        SetAddressData(pointer.PlayerSaveData + 0xE743C, 'int', v)
        SetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xEC, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredWaist' then
        SetAddressData(pointer.PlayerSaveData + 0xE7440, 'int', v)
        SetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xF0, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredLeg' then
        SetAddressData(pointer.PlayerSaveData + 0xE7444, 'int', v)
        SetAddressData(GetAddress(pointer.Player, { 0x12610 }) + 0xF4, 'int', v)
        RefreshEquip()
    end
end

local function trace(t)
    local proxy = {}
    local mt = {
        __index = function(_, k)
            return t[k] -- 直接从原始对象中获取
        end,
        __newindex = function(_, k, v)
            traceHandle(k, v) -- 调用处理函数
            rawset(t, k, v)   -- 更新原始对象的值
        end
    }
    setmetatable(proxy, mt) -- 设置元表
    return proxy
end

function engine_player:AimPosition(target)
    local direction_x = target.x - self.Position.position.x
    local direction_z = target.z - self.Position.position.z
    local aim_angle = math.atan(direction_x / direction_z)
    local sign = function(x) if x < 0 then return -1 elseif x == 0 then return 0 else return 1 end end
    local a2q = function(angle)
        local eulerangles = angle
        if angle / math.pi > 0.5 then
            eulerangles = { x = math.pi, y = angle - math.pi, z = 0 }
        elseif angle / math.pi < -0.5 then
            eulerangles = { x = math.pi, y = angle + math.pi, z = 0 }
        else
            eulerangles = { x = 0, y = -angle, z = math.pi }
        end
        local cr = math.cos(eulerangles.x * 0.5)
        local sr = math.sin(eulerangles.x * 0.5)
        local cp = math.cos(eulerangles.y * 0.5)
        local sp = math.sin(eulerangles.y * 0.5)
        local cy = math.cos(eulerangles.z * 0.5)
        local sy = math.sin(eulerangles.z * 0.5)
        return {
            w = cy * cp * cr + sy * sp * sr,
            x = cy * cp * sr - sy * sp * cr,
            y = sy * cp * sr + cy * sp * cr,
            z = sy * cp * cr - cy * sp * sr
        }
    end
    aim_angle = aim_angle + sign(direction_x) * (1 - sign(direction_z)) * math.pi / 2
    local quaternion = a2q(aim_angle)
    self.Angle.Quaternion = {
        w = self.Angle.Quaternion.w,
        x = quaternion.x,
        y = self.Angle.Quaternion.y,
        z = quaternion.z
    }
end

function engine_player:new()
    local o = {}

    --获取基址
    if aob_Save == nil or aob_player == nil then
        aob_Save = SearchPattern({ 0xE0, 0x9E, "??", "??", 0x00, 0x00, 0x00, 0x00, 0x68 })
        aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
    end

    if aob_Save == nil or not aob_quest then
        aob_Save = 0x145013950
    end
    if aob_player == nil or not aob_quest then
        aob_player = 0x1450139A0
    end

    if pointer.PlayerSaveData == nil then
        pointer.PlayerSaveData = GetAddress(aob_Save, { 0xa8 })
    end

    if pointer.Player == nil then
        pointer.Player = GetAddress(aob_player, { 0x50 })
    end

    if pointer.PlayerData == nil then
        pointer.PlayerData = GetAddress(aob_player, { 0x50, 0xC0, 0x98, 0x18, 0x70, 0xC8, 0xD0, 0x5D0, 0x20 })
    end

    if pointer.Weapon.Entity == nil then
        pointer.Weapon.Entity = GetAddress(aob_player, { 0x50, 0x76B0 })
    end

    if pointer.Weapon.Data == nil then
        pointer.Weapon.Data = GetAddress(aob_player, { 0x50, 0xc0, 0x8, 0x78 })
    end

    --玩家坐标
    o.Position = {
        position = self:getPlayerPosition(),
        cntrposition = self:getPlayerCNTRPosition(),
        reposition = self:getPlayerRepatriatePos(),
        incremental = self:getPlayerIncrementalPos()
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
    --玩家幻化
    o.Layered = self:getPlayerLayeredInfo()
    --玩家临时装备数据
    o.TempArmorData = self:getPlayerTempArmorDataInfo()
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
    o.Layered = trace(o.Layered)
    o.Angle = trace(o.Angle)

    setmetatable(o, self)
    self.__index = self
    return o
end

return engine_player
