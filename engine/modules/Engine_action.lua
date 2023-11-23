require "modules/Engine_player"
local Player = engine_player
--动作派生数据
local actionDB = {}
--跳帧缓存数据
local skipFrameDB = {}
--异常记录
local ExceptionLog = {}

local engine_action = {}

function ExceptionData(i, info)
    table.insert(ExceptionLog, info)
    table.remove(actionDB ,i)
end
function ActionSystem()
    local Data_Player = Player:new()
    local ActionList = actionDB[Data_Player.Action.lmtID]
    for _index, ActionData in ipairs(ActionList) do
        --是否启用
        if not ActionData.enable then
            goto runact_continue 
        end
        --执行派生动作
        if ActionData.doAction ~= nil then
            --异常的动作数据，删除并跳过
            if ActionData.doAction.type == nil then
                ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行动作类型为空(ActionData.doAction.type is nil)')
                goto runact_continue 
            end
            --确认执行帧条件
            local continue_action = false
            if ActionData.doAction.frame ~= nil then
                if type(ActionData.doAction.frame) == 'table' then
                    for _, checkFrame in pairs(ActionData.doAction.frame) do
                        if checkFrame.StartFrame == nil or checkFrame.EndFrame == nil then
                            ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行派生动作帧检查时未找到数据')
                            goto runact_continue 
                        end
                        if Data_Player.Frame.frame >= checkFrame.StartFrame and Data_Player.Frame.frame <= checkFrame.EndFrame then
                            continue_action = true
                            break
                        end
                    end
                else
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行派生动作帧检查时数据类型异常(ActionData.doAction.frame is '..type(ActionData.doAction.frame)..')')
                    goto runact_continue 
                end
            else
                continue_action = true
            end
            if not continue_action then goto runact_continue end
            --确认额外条件
            if type(ActionData.doAction.condition) == 'function' then
                if not ActionData.doAction.condition() then
                    goto runact_continue
                end
            end
            local skipId = nil
            --执行fsm派生动作
            if string.lower(ActionData.doAction.type) == 'fsm' then
                if ActionData.doAction.fsm == nil then
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行fsm动作时未找到数据(ActionData.doAction.fsm is nil)')
                    goto runact_continue 
                end
                if type(ActionData.doAction.fsm.id) ~= 'number' or type(ActionData.doAction.fsm.target) ~= 'number' then
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行fsm动作时id或target数据类型错误(ActionData.doAction.fsm.id or ActionData.doAction.fsm.target is not number)')
                    goto runact_continue 
                end
                Data_Player.Action.fsm = { fsmTarget = ActionData.doAction, fsmID = 86 }
            elseif string.lower(ActionData.doAction.type) == 'lmt' then
                if ActionData.doAction.lmt == nil then
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行lmt动作时未找到数据(ActionData.doAction.lmt is nil)')
                    goto runact_continue 
                end
                if type(ActionData.doAction.lmt.id) ~= 'number' then
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行lmt动作时id数据类型错误(ActionData.doAction.lmt.id is not number)')
                    goto runact_continue 
                end
                Data_Player.Action.lmtID = ActionData.doAction.lmt.id
                skipId = ActionData.doAction.lmt.id
            else
                ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行动作类型不明(ActionData.doAction.type is '..ActionData.doAction.type..')')
                goto runact_continue 
            end
            --设置动作执行后的跳帧操作
            if ActionData.doAction.skipFrame ~= nil then
                if skipId == nil then
                    if ActionData.doAction.skipFrame.id == nil then
                        ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行fsm动作跳帧设定时未找到数据(ActionData.doAction.skipFrame.id is nil)')
                        goto runact_continue 
                    end
                    skipId = ActionData.doAction.skipFrame.id
                end
                local skipdata = {}
                if type(ActionData.doAction.skipFrame.frames) == 'number' then
                    table.insert(skipdata, {{StartFrame = 0, EndFrame = ActionData.doAction.skipFrame.frames}})
                elseif type(ActionData.doAction.skipFrame.frames) == 'table' then
                    skipdata = ActionData.doAction.skipFrame.frames
                else
                    ExceptionData(_index,'[动作'..Data_Player.Action.lmtID..'处理]异常的动作组数据，执行跳帧设定时帧数据异常(ActionData.doAction.skipFrame.frames is '..type(ActionData.doAction.skipFrame.frames)..')')
                    goto runact_continue 
                end
                skipFrameDB[skipId] = skipdata
            end
        end
        ::runact_continue::
        --当前动作跳帧操作
        local SkipList = skipFrameDB[Data_Player.Action.lmtID]
        for _index, SkipFrameData in pairs(SkipList) do
            if SkipFrameData.StartFrame == nil or SkipFrameData.EndFrame == nil then
                goto skipfra_continue 
            end
            if Data_Player.Frame.frame >= SkipFrameData.StartFrame and Data_Player.Frame.frame <= SkipFrameData.EndFrame and SkipFrameData.EndFrame <= Data_Player.Frame.frameEnd then
                Data_Player.Frame.frame = SkipFrameData.EndFrame
            end
            ::skipfra_continue::
        end
        --清除当前帧的跳帧缓存
        table.remove(skipFrameDB ,Data_Player.Action.lmtID)
        --当前动作下额外执行的内容
        if type(ActionData.doScript) == 'function' then
            ActionData.doScript()
        end
    end
end

--引擎处理方法
engine_action.ExceptionLog = ExceptionLog
engine_action.ActionDB = actionDB
engine_action.SkipFrameDB = skipFrameDB
--[[
    addAction方法
    id: 动作lmtId
    action: 动作表
    notes: 备注，可选
    script: 额外脚本，可选

    示例

    addAction(49123, {
        --执行动作类型
        type = 'fsm', --可选fsm或lmt
        --fsm动作数据
        fsm = {
            id = 100,
            target = 3 --通常为1或3
        },
        --lmt动作数据，当type为lmt时使用这个
        --lmt = { id = 49238 },
        --动作执行的帧条件，可省略
        frame = {
            {
                StartFrame = 0,
                EndFrame = 15
            },
            {
                StartFrame = 50,
                EndFrame = 60
            }
        }
        --额外的执行条件，可省略
        condition = function()
            return true
        end
        --动作执行后的跳帧操作，可省略
        skipFrame = {
            id = 49238, --当type为lmt时被忽略并默认为lmtId
            frames = {
                {
                    StartFrame = 10,
                    EndFrame = 30
                },
                {
                    StartFrame = 45,
                    EndFrame = 50
                }
            }
            -- frames = 20 --当只执行一个跳帧操作且动作开始就跳帧的情况下看省略为一个数值
        }
    },'这里可以写一些备注信息，可以用备注信息索引指定派生',
    function()
        print('这里执行额外的操作，可省略')
    end)

]]
engine_action.addAction = function(id, action, notes, script)
    local addData = {
        enable = true,
        doAction = action,
        notes = notes,
        doScript = script
    }
    --检查插入数据是否合法
    if type(addData.doAction) == 'table'
        and (addData.doAction.type == 'fsm' or addData.doAction.type == 'lmt')
    then
        if actionDB[id] ~= nil then
            table.insert(actionDB[id], addData)
            return #actionDB[id]
        else
            actionDB[id] = {addData}
            return 1
        end
    end
    return 0
end
engine_action.removeAction = function(id, index)
    if index ~= nil then
        table.remove(actionDB[id] ,i)
    else
        actionDB[id] = {}
    end
end
function engine_action:new()
    ActionSystem()
end
return engine_action