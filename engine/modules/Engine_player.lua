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
    Player = function() return GetAddress(aob_player,{ 0x50 }) end,
    PlayerSaveData = function() return GetAddress(aob_Save,{ 0xa8 }) end, 
    Weapon = {
        Entity = function() return GetAddress(aob_player,{ 0x50, 0x76B0 }) end,
        Data = function() return GetAddress(aob_player,{ 0x50, 0xc0, 0x8, 0x78 }) end
    }
}

--获取玩家坐标
function engine_player:getPlayerPosition()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(Player + 0x160, 'float'),
        y = GetAddressData(Player + 0x164, 'float'),
        z = GetAddressData(Player + 0x168, 'float')
    }
end

--获取玩家中心点坐标
function engine_player:getPlayerCNTRPosition()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0, h = 0 } end
    return {
        x = GetAddressData(Player + 0x390, 'float'),
        y = GetAddressData(Player + 0x394, 'float'),
        z = GetAddressData(Player + 0x398, 'float'),
        h = GetAddressData(Player + 0x39c, 'float')
    }
end

--获取遣返坐标
function engine_player:getPlayerRepatriatePos()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(Player + 0xA50, 'float'),
        y = GetAddressData(Player + 0xA54, 'float'),
        z = GetAddressData(Player + 0xA58, 'float')
    }
end

--获取运动增量坐标
function engine_player:getPlayerIncrementalPos()
    local Player = pointer:Player()
    local p468 = GetAddress(Player, { 0x468 })
    if not p468 then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(p468 + 0xe250, 'float'),
        y = GetAddressData(p468 + 0xe254, 'float'),
        z = GetAddressData(p468 + 0xe258, 'float'),
    }
end

--获取玩家模型大小
function engine_player:getPlayerModelSize()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(Player + 0x180, 'float'),
        y = GetAddressData(Player + 0x184, 'float'),
        z = GetAddressData(Player + 0x188, 'float')
    }
end

--获取玩家准星指向坐标
function engine_player:getPlayerCollimatorPos()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(Player + 0x7D30, 'float'),
        y = GetAddressData(Player + 0x7D34, 'float'),
        z = GetAddressData(Player + 0x7D38, 'float')
    }
end

--获取玩家四元数角
function engine_player:getPlayerQuaternion()
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0, w = 0 } end
    return {
        w = GetAddressData(Player + 0x170, 'float'),
        x = GetAddressData(Player + 0x174, 'float'),
        y = GetAddressData(Player + 0x178, 'float'),
        z = GetAddressData(Player + 0x17c, 'float')
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
    local Player = pointer:Player()
    if not Player then return { x = 0, y = 0, z = 0 } end
    return {
        x = GetAddressData(Player + 0x7D40, 'float'),
        y = GetAddressData(Player + 0x7D44, 'float'),
        z = GetAddressData(Player + 0x7D48, 'float')
    }
end

--获取玩家瞄准状态
function engine_player:getPlayerAimingState()
    local Player = pointer:Player()
    if not Player then return false end
    return GetAddressData(GetAddress(Player, { 0xC0 }) + 0xC28, 'bool')
end

--获取玩家武器数据
function engine_player:getPlayerWeaponInfo()
    --地址检查
    local WeaponEntity = pointer.Weapon:Entity()
    local WeaponData = pointer.Weapon:Data()
    if WeaponEntity and WeaponData then
        local player_weapon_info = {
            --武器坐标
            position = {
                x = GetAddressData(WeaponEntity + 0x160, 'float'),
                y = GetAddressData(WeaponEntity + 0x164, 'float'),
                z = GetAddressData(WeaponEntity + 0x168, 'float')
            },
            --武器类型
            type = GetAddressData(WeaponData + 0x2E8, 'int'),
            --武器Id
            id = GetAddressData(WeaponData + 0x2EC, 'int'),
            --武器命中的怪物地址
            hit = GetAddress(pointer:Player(), { 0x12958 })
        }
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
    local WeaponEntity = pointer.Weapon:Entity()
    local WeaponData = pointer.Weapon:Data()
    if not WeaponEntity then return { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 } end
    return {
        --头id
        head = GetAddressData(WeaponData + 0x1C4, 'int'),
        --胸id
        chest = GetAddressData(WeaponData + 0x1C8, 'int'),
        --手id
        arm = GetAddressData(WeaponData + 0x1CC, 'int'),
        --腰id
        waist = GetAddressData(WeaponData + 0x1D0, 'int'),
        --鞋id
        leg = GetAddressData(WeaponData + 0x1D4, 'int'),
    }
end

--获取玩家幻化信息
function engine_player:getPlayerLayeredInfo()
    local PlayerSaveData = pointer:PlayerSaveData()
    if not PlayerSaveData then return { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 } end
    return {
        --头id
        layeredHead = GetAddressData(PlayerSaveData + 0xE7434, 'int'),
        --胸id
        layeredChest = GetAddressData(PlayerSaveData + 0xE7438, 'int'),
        --手id
        layeredArm = GetAddressData(PlayerSaveData + 0xE743C, 'int'),
        --腰id
        layeredWaist = GetAddressData(PlayerSaveData + 0xE7440, 'int'),
        --鞋id
        layeredLeg = GetAddressData(PlayerSaveData + 0xE7444, 'int'),
    }
end

--获取玩家临时装备信息
function engine_player:getPlayerTempArmorDataInfo()
    local Player = pointer:Player()
    if not Player then
        return {
            layered = { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 },
            Armor = { head = 0, chest = 0, arm = 0, waist = 0, leg = 0 },
            colour = { head = { r = 0, g = 0, b = 0, a = 0 }, chest = { r = 0, g = 0, b = 0, a = 0 }, arm = { r = 0, g = 0, b = 0, a = 0 }, waist = { r = 0, g = 0, b = 0, a = 0 }, leg = { r = 0, g = 0, b = 0, a = 0 } }
        }
    end
    local armorAddr = GetAddress(Player, { 0x12610 })
    return {
        armor = {
            head = GetAddressData(armorAddr + 0xCC, 'int'),
            chest = GetAddressData(armorAddr + 0xD0, 'int'),
            arm = GetAddressData(armorAddr + 0xD4, 'int'),
            waist = GetAddressData(armorAddr + 0xD8, 'int'),
            leg = GetAddressData(armorAddr + 0xDC, 'int')
        },
        layered = {
            head = GetAddressData(armorAddr + 0xE4, 'int'),
            chest = GetAddressData(armorAddr + 0xE8, 'int'),
            arm = GetAddressData(armorAddr + 0xEC, 'int'),
            waist = GetAddressData(armorAddr + 0xF0, 'int'),
            leg = GetAddressData(armorAddr + 0xF4, 'int')
        },
        colour = {
            head = {
                r = GetAddressData(armorAddr + 0x170, 'float'),
                g = GetAddressData(armorAddr + 0x174, 'float'),
                b = GetAddressData(armorAddr + 0x178, 'float'),
                a = GetAddressData(armorAddr + 0x17C, 'float')
            },
            chest = {
                r = GetAddressData(armorAddr + 0x180, 'float'),
                g = GetAddressData(armorAddr + 0x184, 'float'),
                b = GetAddressData(armorAddr + 0x188, 'float'),
                a = GetAddressData(armorAddr + 0x18C, 'float')
            },
            arm = {
                r = GetAddressData(armorAddr + 0x190, 'float'),
                g = GetAddressData(armorAddr + 0x194, 'float'),
                b = GetAddressData(armorAddr + 0x198, 'float'),
                a = GetAddressData(armorAddr + 0x19C, 'float')
            },
            waist = {
                r = GetAddressData(armorAddr + 0x1A0, 'float'),
                g = GetAddressData(armorAddr + 0x1A4, 'float'),
                b = GetAddressData(armorAddr + 0x1A8, 'float'),
                a = GetAddressData(armorAddr + 0x1AC, 'float')
            },
            leg = {
                r = GetAddressData(armorAddr + 0x1B0, 'float'),
                g = GetAddressData(armorAddr + 0x1B4, 'float'),
                b = GetAddressData(armorAddr + 0x1B8, 'float'),
                a = GetAddressData(armorAddr + 0x1BC, 'float')
            }
        }
    }
end

--获取玩家状态信息
function engine_player:getPlayerCharacteristic()
    local Player = pointer:Player()
    if not Player then
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
    local healthAddr = GetAddress(Player, { 0x7630 })
    return {
        health = {
            health_base = GetAddressData(pointer:Player() + 0x7628, 'float'),
            health_current = GetAddressData(healthAddr + 0x64, 'float'),
            health_max = GetAddressData(healthAddr + 0x60, 'float'),
        },
        stamina = {
            stamina_current = GetAddressData(healthAddr + 0x12C, 'float'),
            stamina_max = GetAddressData(healthAddr + 0x134, 'float'),
            stamina_eat = GetAddressData(healthAddr + 0x13C, 'float'),
        }
    }
end

--获取玩家动作信息
function engine_player:getPlayerActionInfo()
    local Player = pointer:Player()
    if not Player then
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
        lmtID = GetAddressData(GetAddress(Player, { 0x468 }) + 0xE9C4, 'int'),
        fsm = {
            fsmID = GetAddressData(Player + 0x6278, 'int'),
            fsmTarget = GetAddressData(Player + 0x6274, 'int')
        },
        useItem = GetAddressData(Player + 0xb780, 'int')
    }
end

--获取重力信息
function engine_player:getPlayerGravityInfo()
    local Player = pointer:Player()
    if not Player then
        return {
            gravity = 0,
            fall = 0,
            liftoff = false
        }
    end
    return {
        gravity = GetAddressData(Player + 0x14B0, 'float'),
        fall = GetAddressData(Player + 0xE178, 'float'),
        liftoff = GetAddressData(Player + 0x112C, 'bool')
    }
end

--获取动作帧信息
function engine_player:getPlayerFrameInfo()
    local Player = pointer:Player()
    if not Player then
        return {
            frame = 0,
            frameEnd = 0,
            frameSpeed = 0,
            frameSpeedMultiplies = 0
        }
    end
    local p468 = GetAddress(Player, { 0x468 })
    return {
        frame = GetAddressData(p468 + 0x10C, 'float'),
        frameEnd = GetAddressData(p468 + 0x114, 'float'),
        frameSpeed = GetAddressData(Player + 0x6c, 'float'),
        frameSpeedMultiplies = GetAddressData(
            GetAddressData(0x1451238C8, 'int') + GetAddressData(Player + 0x10, 'int') * 0xf8 + 0x9c, 'float')
    }
end

--监听
local function traceHandle(k, v)
    local Player = pointer:Player()
    --耐力修改
    if k == 'stamina_current' then
        SetAddressData(GetAddress(Player, { 0x7630 }) + 0x12C, 'float', v)
        return
    end
    if k == 'stamina_max' then
        SetAddressData(GetAddress(Player, { 0x7630 }) + 0x134, 'float', v)
        return
    end
    --健康修改
    if k == 'health_base' then
        SetAddressData(Player + 0x7628, 'float', v)
        return
    end
    if k == 'health_current' then
        SetAddressData(GetAddress(Player, { 0x7630 }) + 0x64, 'float', v)
        return
    end
    --坐标修改
    if k == 'position' then
        SetAddressData(Player + 0x160, 'float', v.x)
        SetAddressData(Player + 0x164, 'float', v.y)
        SetAddressData(Player + 0x168, 'float', v.z)
        return
    end
    --遣返坐标修改
    if k == 'reposition' then
        SetAddressData(Player + 0xA50, 'float', v.x)
        SetAddressData(Player + 0xA54, 'float', v.y)
        SetAddressData(Player + 0xA58, 'float', v.z)
        return
    end
    --模型大小修改
    if k == 'size' then
        SetAddressData(Player + 0x180, 'float', v.x)
        SetAddressData(Player + 0x184, 'float', v.y)
        SetAddressData(Player + 0x188, 'float', v.z)
        return
    end
    --四元数角修改
    if k == 'Quaternion' then
        SetAddressData(Player + 0x170, 'float', v.w)
        SetAddressData(Player + 0x174, 'float', v.x)
        SetAddressData(Player + 0x178, 'float', v.y)
        SetAddressData(Player + 0x17c, 'float', v.z)
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
        SetAddressData(Player + 0x14B0, 'float', v)
        return
    end
    if k == 'fall' then
        SetAddressData(Player + 0xE178, 'float', v)
        return
    end
    --动作帧修改
    if k == 'frame' then
        SetAddressData(GetAddress(Player, { 0x468 }) + 0x10C, 'float', v)
        return
    end
    --动作帧速率倍率修改
    if k == 'frameSpeedMultiplies' then
        SetAddressData(
            GetAddressData(0x1451238C8, 'int') + GetAddressData(Player + 0x10, 'int') * 0xf8 + 0x9c
            , 'float', v)
        return
    end
    --幻化
    if k == 'layeredHead' then
        SetAddressData(pointer:PlayerSaveData() + 0xE7434, 'int', v)
        SetAddressData(GetAddress(Player, { 0x12610 }) + 0xE4, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredChest' then
        SetAddressData(pointer:PlayerSaveData() + 0xE7438, 'int', v)
        SetAddressData(GetAddress(Player, { 0x12610 }) + 0xE8, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredArm' then
        SetAddressData(pointer:PlayerSaveData() + 0xE743C, 'int', v)
        SetAddressData(GetAddress(Player, { 0x12610 }) + 0xEC, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredWaist' then
        SetAddressData(pointer:PlayerSaveData() + 0xE7440, 'int', v)
        SetAddressData(GetAddress(Player, { 0x12610 }) + 0xF0, 'int', v)
        RefreshEquip()
    end
    if k == 'layeredLeg' then
        SetAddressData(pointer:PlayerSaveData() + 0xE7444, 'int', v)
        SetAddressData(GetAddress(Player, { 0x12610 }) + 0xF4, 'int', v)
        RefreshEquip()
    end
end

local function trace(t)
    local proxy = {}
    local mt = {
        __index = function(_, k)
            if k == 'position' then
                t[k] = engine_player:getPlayerPosition()
            elseif k == 'cntrposition' then
                t[k] =  engine_player:getPlayerCNTRPosition()
            elseif k == 'reposition' then
                t[k] =  engine_player:getPlayerRepatriatePos()
            elseif k == 'incremental' then
                t[k] =  engine_player:getPlayerIncrementalPos()
            elseif k == 'size' then
                t[k] =  engine_player:getPlayerModelSize()
            elseif k == 'straightPos' then
                t[k] =  engine_player:getPlayerCollimatorPos()
            elseif k == 'parabolaPos' then
                t[k] =  engine_player:getPlayerParabolaCollimatorPos()
            elseif k == 'aimingState' then
                t[k] =  engine_player:getPlayerAimingState()
            elseif k == 'Quaternion' then
                t[k] =  engine_player:getPlayerQuaternion()
            elseif k == 'Eulerian' then
                t[k] =  engine_player:getPlayerEulerian()
            elseif k == 'Weapon' then
                t[k] =  engine_player:getPlayerWeaponInfo()
            elseif k == 'Armor' then
                t[k] =  engine_player:getPlayerArmorInfo()
            elseif k == 'Layered' then
                t[k] =  engine_player:getPlayerLayeredInfo()
            elseif k == 'TempArmorData' then
                t[k] =  engine_player:getPlayerTempArmorDataInfo()
            elseif k == 'Characteristic' then
                t[k] =  engine_player:getPlayerCharacteristic()
            elseif k == 'Action' then
                t[k] =  engine_player:getPlayerActionInfo()
            elseif k == 'Gravity' then
                t[k] =  engine_player:getPlayerGravityInfo()
            elseif k == 'Frame' then
                t[k] =  engine_player:getPlayerFrameInfo()
            end

            if type(t[k]) == "table" then
                return trace(t[k]) -- 仅对未追踪的表进行监听
            end

            return rawget(t, k) -- 返回原始值
        end,
        __newindex = function(_, k, v)
            traceHandle(k, v)
            rawset(t, k, v) -- 其他情况直接设置
        end,
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

    --获取基址(扫描结果缓存到全局变量,跨脚本状态共享,避免每个状态重复全模块扫描)
    aob_Save = GlobalVariable_int('Engine_aob_Save')
    aob_player = GlobalVariable_int('Engine_aob_Player')
    if aob_Save == 0 or aob_player == 0 then
        if aob_Save == 0 then
            aob_Save = SearchPattern({ 0xE0, 0x9E, "??", "??", 0x00, 0x00, 0x00, 0x00, 0x68 })
            if not aob_Save then aob_Save = 0x145013950 end
            setGlobalVariable_int('Engine_aob_Save', aob_Save)
        end
        if aob_player == 0 then
            aob_player = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
            if not aob_player then aob_player = 0x1450139A0 end
            setGlobalVariable_int('Engine_aob_Player', aob_player)
        end
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

    return trace(o)
end

return engine_player
