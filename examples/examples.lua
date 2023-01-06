--游戏初始化执行的代码
function on_init()
    Console_Info('这是LueEngine的示例代码')
end

--每次切换场景执行的代码
function on_switch_scenes()
    local MapPtr = GetAddress(0x145073ED0,{ 0x50, 0x7D20 })
    local MapId = GetAddressData(MapPtr + 0xB88, 'int')
    ShowMessage('当前地图：'..MapId)
end

--每次时间变动执行的代码
function on_time()

end

--每次生成怪物时执行的代码
function on_monster_create()

end
--每次销毁怪物时执行的代码
function on_monster_destroy()

end