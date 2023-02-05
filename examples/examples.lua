local openDataview = true
local Switch_Weapon = {
    type = 0,
    id = 0
}
local FSM_Run = {
    target = 0,
    id = 0
}
local addFrameSpeed = 0
local Keyboard_Shortcut  = '~'
local shlpid = 0
local MonsterShlpTempData = {}
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
                ImGui.InputFloat3("玩家坐标", {
                    Data_Player.Position.position.x,
                    Data_Player.Position.position.y,
                    Data_Player.Position.position.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.Text("玩家遣返坐标")
                ImGui.InputFloat3("玩家遣返坐标", {
                    Data_Player.Position.reposition.x,
                    Data_Player.Position.reposition.y,
                    Data_Player.Position.reposition.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("玩家模型") then
                ImGui.InputFloat3("模型大小", {
                    Data_Player.Model.size.x,
                    Data_Player.Model.size.y,
                    Data_Player.Model.size.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("角度信息") then
                ImGui.InputFloat4("四元数", {Data_Player.Angle.Quaternion.x,Data_Player.Angle.Quaternion.y,Data_Player.Angle.Quaternion.z,Data_Player.Angle.Quaternion.w}, "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.InputFloat3("欧拉角", {Data_Player.Angle.Eulerian.x,Data_Player.Angle.Eulerian.y,Data_Player.Angle.Eulerian.z}, "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("准星信息") then
                if Data_Player.Collimator.aimingState then
                    ImGui.Text("瞄准状态: 是")
                else
                    ImGui.Text("瞄准状态: 否")
                end
                ImGui.InputFloat3("直线瞄准坐标", {
                    Data_Player.Collimator.straightPos.x,
                    Data_Player.Collimator.straightPos.y,
                    Data_Player.Collimator.straightPos.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.InputFloat3("抛物线瞄准坐标", {
                    Data_Player.Collimator.parabolaPos.x,
                    Data_Player.Collimator.parabolaPos.y,
                    Data_Player.Collimator.parabolaPos.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
                ImGui.TreePop()
            end
            if ImGui.TreeNode("武器信息") then
                ImGui.InputFloat3("武器坐标", {
                    Data_Player.Weapon.position.x,
                    Data_Player.Weapon.position.y,
                    Data_Player.Weapon.position.z}
                    , "%.5f",ImGuiInputTextFlags.ReadOnly)
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
                    --初始化目标怪物引擎
                    local hit_monster = engine.Monster:new(Data_Player.Weapon.hit)
                    ImGui.InputFloat3("怪物坐标", {
                        hit_monster.Position.position.x,
                        hit_monster.Position.position.y,
                        hit_monster.Position.position.z}
                        , "%.5f",ImGuiInputTextFlags.ReadOnly)
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
                t_addFrameSpeed = ImGui.SliderFloat("增加速率", addFrameSpeed, -2, 10)
                if addFrameSpeed ~= t_addFrameSpeed then --减少map访问
                    addFrameSpeed = t_addFrameSpeed
                    AddFrameSpeed(GetAddress(0x145073ED0,{ 0x50 }),addFrameSpeed)
                end
                ImGui.Separator()
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
                Data_Player.Gravity.fall = ImGui.InputFloat("下落速度", Data_Player.Gravity.fall)
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
            ImGui.InputFloat3("导航坐标", {
                Data_World.Position.wayPosition.x,
                Data_World.Position.wayPosition.y,
                Data_World.Position.wayPosition.z}
                , "%.5f",ImGuiInputTextFlags.ReadOnly)
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
                if ImGui.TreeNode("怪物"..monsterData.Id.." _ "..string.format("%X", monster)) then
                    if ImGui.TreeNode("怪物坐标") then
                        ImGui.InputFloat3("怪物坐标", {
                            Data_Monster.Position.position.x,
                            Data_Monster.Position.position.y,
                            Data_Monster.Position.position.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
                        ImGui.InputFloat3("怪物遣返坐标", {
                            Data_Monster.Position.reposition.x,
                            Data_Monster.Position.reposition.y,
                            Data_Monster.Position.reposition.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("怪物模型") then
                        ImGui.InputFloat3("模型大小", {
                            Data_Monster.Model.size.x,
                            Data_Monster.Model.size.y,
                            Data_Monster.Model.size.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("状态") then
                        Data_Monster.Characteristic.health_current = ImGui.InputInt("当前生命", Data_Monster.Characteristic.health_current, 0, Data_Monster.Characteristic.health_max)
                        Data_Monster.Characteristic.health_max = ImGui.InputInt("最大生命", Data_Monster.Characteristic.health_max)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("动作") then
                        if MFSM_Run[monster] == nil then 
                            MFSM_Run[monster] = {
                                target = 0,
                                id = 0
                            }
                        end
                        ImGui.Text("当前动作: "..Data_Monster.Action.lmtID)
                        ImGui.Text("动作帧大小: "..Data_Monster.Frame.frameEnd)
                        ImGui.SameLine()
                        ImGui.Text("动作帧速率: "..Data_Monster.Frame.frameSpeed)
                        Data_Monster.Frame.frame = ImGui.InputInt("当前动作帧", Data_Monster.Frame.frame)
                        ImGui.Separator()
                        ImGui.Text("派生目标: "..Data_Monster.Action.fsm.fsmTarget)
                        ImGui.Text("派生id: "..Data_Monster.Action.fsm.fsmID)
                        ImGui.Separator()
                        MFSM_Run[monster].target = ImGui.InputInt("派生目标", MFSM_Run[monster].target)
                        MFSM_Run[monster].id = ImGui.InputInt("派生id", MFSM_Run[monster].id)
                        if ImGui.Button("执行派生动作") then
                            Data_Monster.Action.fsm = { fsmTarget = MFSM_Run[monster].target, fsmID = MFSM_Run[monster].id }
                        end
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("投射物") then
                        if Data_Monster.Shlp.ListPtr then
                            ImGui.Text("投射物列表地址: "..string.format("%X", Data_Monster.Shlp.ListPtr))
                            if MonsterShlpTempData[monster] == nil then InitMonsterShlpTempData(monster) end
                            MonsterShlpTempData[monster].from= ImGui.Combo("投射人", MonsterShlpTempData[monster].from, { "玩家", "怪物" }, 2)
                            MonsterShlpTempData[monster].shell_shlpid = ImGui.InputInt("投射物id", MonsterShlpTempData[monster].shell_shlpid)
                            MonsterShlpTempData[monster].startPos = ImGui.InputFloat3("起点坐标", MonsterShlpTempData[monster].startPos)
                            MonsterShlpTempData[monster].endPos = ImGui.InputFloat3("终点坐标", MonsterShlpTempData[monster].endPos)
                            if ImGui.Button("发射") and Data_Monster.Shlp.ListPtr then
                                local fromList = {[0] = GetAddress(0x145073ED0,{ 0x50 }),[1] = monster}
                                CreateProjectiles(
                                    MonsterShlpTempData[monster].shell_shlpid,
                                    MonsterShlpTempData[monster].startPos[1],
                                    MonsterShlpTempData[monster].startPos[2],
                                    MonsterShlpTempData[monster].startPos[3],
                                    MonsterShlpTempData[monster].endPos[1],
                                    MonsterShlpTempData[monster].endPos[2],
                                    MonsterShlpTempData[monster].endPos[3],
                                    fromList[MonsterShlpTempData[monster].from],
                                    Data_Monster.Shlp.ListPtr
                                )
                            end
                        end
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
                        ImGui.InputFloat3("环境生物坐标", {
                            Data_Animals.Position.position.x,
                            Data_Animals.Position.position.y,
                            Data_Animals.Position.position.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
                        ImGui.InputFloat3("环境生物遣返坐标", {
                            Data_Animals.Position.reposition.x,
                            Data_Animals.Position.reposition.y,
                            Data_Animals.Position.reposition.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
                        ImGui.TreePop()
                    end
                    if ImGui.TreeNode("环境生物模型") then
                        ImGui.Text("模型大小")
                        ImGui.InputFloat3("模型大小", {
                            Data_Animals.Model.size.x,
                            Data_Animals.Model.size.y,
                            Data_Animals.Model.size.z}
                            , "%.5f",ImGuiInputTextFlags.ReadOnly)
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


function InitMonsterShlpTempData(monster)
    MonsterShlpTempData[monster] = {
        shell_shlpid = 0,
        from = 0,
        startPos = {0,0,0},
        endPos = {0,0,0}
    }
end