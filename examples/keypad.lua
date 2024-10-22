--[[
    通过按键触发功能示例
    通常情况下可以使用引擎内置的按键处理功能处理键盘按键按下

    键盘按键与组合键是否按下检测
    engine.keypad({'Ctrl', '+'})
    engine.keypad('Num0')
    Xbox手柄按键按下检测
    engine.keypad({'LB', 'R→'}, true)
    engine.keypad('RB', true)

    配合计时器功能可以实现某个按键（或按键组合）在一定时间内只触发一次
]]
function on_time()
    if engine.keypad({'ctrl','c'}) and
        (CheckChronoscope('keypad_keyCD_ctrl+c')
            or not CheckPresenceChronoscope('keypad_keyCD_ctrl+c'))
    then
        -- 1秒内只触发一次
        AddChronoscope(1, 'keypad_keyCD_ctrl+c')
        -- 游戏内消息
        Message('ctrl + c')
        -- 聊天消息
        SendChatMessage('复制')
        -- console消息
        print('按下了按键 ctrl + c')
    end
end
