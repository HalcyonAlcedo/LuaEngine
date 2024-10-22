--[[
    Hook功能示例
    可以指定地址hook获取寄存器信息并执行自定义代码
    InstallHook功能可以创建钩子
    UninstallHook功能可以移除钩子

    同一个脚本中对同一个地址的钩子只能执行一个函数，新的InstallHook操作将覆盖就的函数
    多个脚本对同一个地址可同时执行多个函数
]]

local function hookFunction(regs)
    -- 获取rbx和rdx寄存器，并输出
    local source = regs.rbx
    local target = regs.rdx
    print({
        source = string.format("%X", source),
        target = string.format("%X", target),
    })
end

function on_init()
    -- 在0x142042776处创建钩子执行hookFunction
    InstallHook(0x142042776, hookFunction)
end