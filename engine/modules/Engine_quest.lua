--[[
    任务数据
    Time                    任务时间
    Id                      任务Id
    State                   任务状态

    方法
    SetState
]]
Engine_quest = {}
Engine_quest.pointer = {
    Time = GetAddress(0x145073ED0,{ 0x50, 0x7D20 }),
    Quest = 0x14506F240
}
--获取当前任务时间
local function getTime()
    local time = GetAddressData(Engine_quest.pointer.Time + 0xC24, 'float')
    return time
end
--获取任务Id
local function getId()
    local time = GetAddressData(Engine_quest.pointer.Quest + 0x4C, 'int')
    return time
end
--获取任务状态
local function getState()
    local time = GetAddressData(Engine_quest.pointer.Quest + 0x54, 'int')
    return time
end


--任务时间
Engine_quest.Time = getTime()


--设置任务状态
function Engine_quest.setState(state)
    SetAddressData(Engine_quest.pointer.Quest + 0x38,'int',state)
end

return Engine_quest