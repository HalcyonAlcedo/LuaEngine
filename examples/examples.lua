local openDataview = true
local Switch_Weapon = {
    type = 0,
    id = 0
}
local FSM_Run = {
    target = 0,
    id = 0
}
local Keyboard_Shortcut  = '~'
--游戏初始化执行的代码
function on_init()
    print('这是LueEngine的示例代码')
end

--每次切换场景执行的代码
function on_switch_scenes()
    
end

--每次时间变动执行的代码
function on_time()
    --开关数据视图
    if engine.keypad(Keyboard_Shortcut) and
    (CheckChronoscope('keypad_keyCD_'..Keyboard_Shortcut)
    or not CheckPresenceChronoscope('keypad_keyCD_'..Keyboard_Shortcut))
    then
        AddChronoscope(1,'keypad_keyCD_'..Keyboard_Shortcut)
        openDataview = not openDataview
    end
end

--图形绘制代码放这里
function on_imgui()
    --初始化数据引擎
    local Data_Player = engine.Player:new()
    local Data_World = engine.World:new()
    local Data_Quest = engine.Quest:new()

    if openDataview then
        ImGui.SetNextWindowBgAlpha(0.35)
        ImGui.SetNextWindowSize(500, 800)
        openDataview, shouldDraw = ImGui.Begin('数据窗口', openDataview)

        --玩家数据
        if ImGui.TreeNode("玩家数据") then
            if ImGui.TreeNode("玩家坐标") then
                ImGui.Text("玩家坐标")
                ImGui.Text("X: "..Data_Player.Position.position.x)
                ImGui.Text("Y: "..Data_Player.Position.position.y)
                ImGui.Text("Z: "..Data_Player.Position.position.z)
                ImGui.Text("玩家遣返坐标")
                ImGui.Text("X: "..Data_Player.Position.reposition.x)
                ImGui.Text("Y: "..Data_Player.Position.reposition.y)
                ImGui.Text("Z: "..Data_Player.Position.reposition.z)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("玩家模型") then
                ImGui.Text("模型大小")
                ImGui.Text("X: "..Data_Player.Model.size.x)
                ImGui.Text("Y: "..Data_Player.Model.size.y)
                ImGui.Text("Z: "..Data_Player.Model.size.z)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("准星信息") then
                if Data_Player.Collimator.aimingState then
                    ImGui.Text("瞄准状态: 是")
                else
                    ImGui.Text("瞄准状态: 否")
                end
                ImGui.Text("直线瞄准坐标")
                ImGui.Text("X: "..Data_Player.Collimator.straightPos.x)
                ImGui.Text("Y: "..Data_Player.Collimator.straightPos.y)
                ImGui.Text("Z: "..Data_Player.Collimator.straightPos.z)
                ImGui.Text("抛物线瞄准坐标")
                ImGui.Text("X: "..Data_Player.Collimator.parabolaPos.x)
                ImGui.Text("Y: "..Data_Player.Collimator.parabolaPos.y)
                ImGui.Text("Z: "..Data_Player.Collimator.parabolaPos.z)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("武器信息") then
                ImGui.Text("武器坐标")
                ImGui.Text("X: "..Data_Player.Weapon.position.x)
                ImGui.Text("Y: "..Data_Player.Weapon.position.y)
                ImGui.Text("Z: "..Data_Player.Weapon.position.z)
                ImGui.Text("武器类型: "..Data_Player.Weapon.type)
                ImGui.Text("武器ID: "..Data_Player.Weapon.id)
                ImGui.Separator()
                ImGui.Text("更换武器")
                Switch_Weapon.type = ImGui.InputInt("武器类型", Switch_Weapon.type, 1, 13)
                Switch_Weapon.id = ImGui.InputInt("武器ID", Switch_Weapon.id)
                if ImGui.Button("更换") then
                    ChangeWeapons(Switch_Weapon.type, Switch_Weapon.id)
                end
                ImGui.Separator()
                ImGui.Text("最后命中的怪物")
                ImGui.Text("地址 "..Data_Player.Weapon.hit)
                if Data_Player.Weapon.hit ~= 0 then
                    ImGui.Text("怪物坐标")
                    --初始化目标怪物引擎
                    local hit_monster = engine.Monster:new(Data_Player.Weapon.hit)
                    ImGui.Text("X: "..hit_monster.Position.position.x)
                    ImGui.Text("Y: "..hit_monster.Position.position.y)
                    ImGui.Text("Z: "..hit_monster.Position.position.z)
                end
                ImGui.TreePop()
            end
            if ImGui.TreeNode("玩家装备") then
                ImGui.Text("头盔: "..Data_Player.Armor.head)
                ImGui.Text("胸甲: "..Data_Player.Armor.chest)
                ImGui.Text("腕甲: "..Data_Player.Armor.arm)
                ImGui.Text("腰甲: "..Data_Player.Armor.waist)
                ImGui.Text("鞋子: "..Data_Player.Armor.leg)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("状态") then
                if ImGui.TreeNode("健康") then
                    Data_Player.Characteristic.health.health_current = ImGui.SliderFloat("当前生命", Data_Player.Characteristic.health.health_current, 0, Data_Player.Characteristic.health.health_max)
                    Data_Player.Characteristic.health.health_base = ImGui.SliderFloat("基础生命", Data_Player.Characteristic.health.health_base, 0, 150)
                    ImGui.Text("最大生命: "..Data_Player.Characteristic.health.health_max)
                    ImGui.TreePop()
                end
                if ImGui.TreeNode("耐力") then
                    Data_Player.Characteristic.stamina.stamina_current = ImGui.SliderFloat("当前耐力", Data_Player.Characteristic.stamina.stamina_current, 0, Data_Player.Characteristic.stamina.stamina_max)
                    Data_Player.Characteristic.stamina.stamina_max = ImGui.SliderFloat("最大耐力", Data_Player.Characteristic.stamina.stamina_max, 0, 150)
                    ImGui.Text("饥饿时间: "..Data_Player.Characteristic.health.health_max)
                    ImGui.TreePop()
                end
                ImGui.TreePop()
            end
            if ImGui.TreeNode("动作") then
                local lmtid = ImGui.InputInt("当前动作", Data_Player.Action.lmtID)
                if Data_Player.Action.lmtID ~= lmtid then Data_Player.Action.lmtID = lmtid end
                ImGui.Text("动作帧大小: "..Data_Player.Frame.frameEnd)
                ImGui.SameLine()
                ImGui.Text("动作帧速率: "..Data_Player.Frame.frameSpeed)
                Data_Player.Frame.frame = ImGui.InputInt("当前动作帧", Data_Player.Frame.frame)
                ImGui.Text("派生id: "..Data_Player.Action.fsm.fsmID)
                ImGui.Text("派生目标: "..Data_Player.Action.fsm.fsmTarget)
                ImGui.Separator()
                FSM_Run.target = ImGui.InputInt("派生目标", FSM_Run.target)
                FSM_Run.id = ImGui.InputInt("派生id", FSM_Run.id)
                if ImGui.Button("执行派生动作") then
                    Data_Player.Action.fsm = { fsmTarget = FSM_Run.target, fsmID = FSM_Run.id }
                end
                ImGui.TreePop()
            end
            if ImGui.TreeNode("重力") then
                Data_Player.Gravity.gravity = ImGui.InputInt("重力加速度", Data_Player.Gravity.gravity)
                Data_Player.Gravity.fall = ImGui.InputInt("下落速度", Data_Player.Gravity.fall)
                if Data_Player.Gravity.liftoff then
                    ImGui.Text("是否在空中: 是")
                else
                    ImGui.Text("是否在空中: 否")
                end
                ImGui.TreePop()
            end
            ImGui.TreePop()
        end
        --世界数据
        if ImGui.TreeNode("世界数据") then
            ImGui.Text("世界时间: "..Data_World.Time)
            ImGui.Text("世界ID: "..Data_World.MapId)
            ImGui.TreePop()
        end
        --任务数据
        if ImGui.TreeNode("任务数据") then
            ImGui.Text("任务时间: "..Data_Quest.Time)
            ImGui.Text("任务ID: "..Data_Quest.Id)
            ImGui.Text("任务状态: "..Data_Quest.State)
            ImGui.TreePop()
        end
        --怪物数据
        if ImGui.TreeNode("怪物数据") then
            local monsterList = GetAllMonster()
            for monster, monsterData in pairs(monsterList) do
                local Data_Monster = engine.Monster:new(monster)
                if ImGui.TreeNode("怪物"..monsterData.Id.." _ "..monster) then
                    if ImGui.TreeNode("怪物坐标") then
                        ImGui.Text("怪物坐标")
                        ImGui.Text("X: "..Data_Monster.Position.position.x)
                        ImGui.Text("Y: "..Data_Monster.Position.position.y)
                        ImGui.Text("Z: "..Data_Monster.Position.position.z)
                        ImGui.Text("怪物遣返坐标")
                        ImGui.Text("X: "..Data_Monster.Position.reposition.x)
                        ImGui.Text("Y: "..Data_Monster.Position.reposition.y)
                        ImGui.Text("Z: "..Data_Monster.Position.reposition.z)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("怪物模型") then
                        ImGui.Text("模型大小")
                        ImGui.Text("X: "..Data_Monster.Model.size.x)
                        ImGui.Text("Y: "..Data_Monster.Model.size.y)
                        ImGui.Text("Z: "..Data_Monster.Model.size.z)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("状态") then
                        Data_Monster.Characteristic.health_current = ImGui.InputInt("当前生命", Data_Monster.Characteristic.health_current, 0, Data_Monster.Characteristic.health_max)
                        Data_Monster.Characteristic.health_max = ImGui.InputInt("最大生命", Data_Monster.Characteristic.health_max)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("动作") then
                        ImGui.Text("当前动作: "..Data_Monster.Action.lmtID)
                        ImGui.Text("动作帧大小: "..Data_Monster.Frame.frameEnd)
                        ImGui.SameLine()
                        ImGui.Text("动作帧速率: "..Data_Monster.Frame.frameSpeed)
                        Data_Monster.Frame.frame = ImGui.InputInt("当前动作帧", Data_Monster.Frame.frame)
                        ImGui.TreePop()
                    end
                    ImGui.TreePop()
                end
            end
            ImGui.TreePop()
        end
        --环境生物数据
        if ImGui.TreeNode("环境生物数据") then
            local animalsList = GetAllAnimals()
            for animals, animalsData in pairs(animalsList) do
                local Data_Animals = engine.Entity:new(animals)
                if ImGui.TreeNode("环境生物"..animalsData.Id.." _ "..animals) then
                    if ImGui.TreeNode("环境生物坐标") then
                        ImGui.Text("环境生物坐标")
                        ImGui.Text("X: "..Data_Animals.Position.position.x)
                        ImGui.Text("Y: "..Data_Animals.Position.position.y)
                        ImGui.Text("Z: "..Data_Animals.Position.position.z)
                        ImGui.Text("环境生物遣返坐标")
                        ImGui.Text("X: "..Data_Animals.Position.reposition.x)
                        ImGui.Text("Y: "..Data_Animals.Position.reposition.y)
                        ImGui.Text("Z: "..Data_Animals.Position.reposition.z)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("环境生物模型") then
                        ImGui.Text("模型大小")
                        ImGui.Text("X: "..Data_Animals.Model.size.x)
                        ImGui.Text("Y: "..Data_Animals.Model.size.y)
                        ImGui.Text("Z: "..Data_Animals.Model.size.z)
                        ImGui.TreePop()
                    end
                    ImGui.TreePop()
                end
            end
            ImGui.TreePop()
        end
        ImGui.End()
    end
end

--每次生成怪物时执行的代码
function on_monster_create()

end

--每次销毁怪物时执行的代码
function on_monster_destroy()

end