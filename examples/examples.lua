require "Engine"

--游戏初始化执行的代码
function on_init()
    Console_Info('这是LueEngine的示例代码')
end

--每次切换场景执行的代码
function on_switch_scenes()
    ShowMessage('当前地图：'..engine.World.MapId)
end

--每次时间变动执行的代码
function on_time()
    --按下小键盘5瞬移至目前准星处
    if engine.keypad('Num5') then
        local collimator = engine.Player.Collimator.straightPos
        engine.Player.setPos(collimator.x, collimator.y, collimator.z)
    end
end

--每次生成怪物时执行的代码
function on_monster_create()

end

--每次销毁怪物时执行的代码
function on_monster_destroy()

end