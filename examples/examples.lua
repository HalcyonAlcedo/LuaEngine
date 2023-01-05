--游戏初始化执行的代码
function on_init()
    System_Console_Info('LueEngine Hello World')
end

--每次切换场景执行的代码
function on_switch_scenes()
    local MapPtr = System_Memory_GetAddress(0x145073ED0,{ 0x50, 0x88, 0x1B0, 0x308, 0x10, 0x10, 0xB88 })
    local MapId = System_Memory_GetAddressData(MapPtr, 'int')
    System_Message_ShowMessage('当前地图：'..MapId)
end

--每次时间变动执行的代码
function on_time()
    
end