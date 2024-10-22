--[[
    内存操作功能示例
    内存操作分为标准和安全的内存访问
    标准内存访问只提供基础的地址检查
    安全内存访问提供了更多的检查和崩溃记录功能，但运行开销较大，如果频繁操作对游戏帧率影响巨大
    推荐在开发时使用安全内存访问，完成脚本开发后换成标准内存访问

    标准内存访问
    GetAddress
    GetAddressData
    SetAddressData
    SearchPattern

    安全内存访问
    SafeGetAddress
    SafeGetAddressData
    SafeSetAddressData
    SafeSearchPattern
]]

local playerBasePtr = nil

function on_init()
    -- 根据特征搜索玩家基址
    if playerBasePtr == nil then
        playerBasePtr = SearchPattern({ 0x20, 0x67, "??", "??", 0x00, 0x00, 0x00, 0x00 })
    end
    -- 如果未搜索到则使用目前已知的静态基址
    if playerBasePtr == nil or not playerBasePtr then
        playerBasePtr = 0x1450139A0
    end
end

function on_time()
    if playerBasePtr ~= nil then
        -- 获取玩家地址
        local plaeyrPtr = GetAddress(playerBasePtr,{ 0x50 })
        if plaeyrPtr then
            -- 如果成功获取，可以读取文件的血量

            -- 当前血量
            local current = GetAddressData(GetAddress(plaeyrPtr, { 0x7630 }) + 0x64, 'float')
            -- 最大血量
            local max = GetAddressData(GetAddress(plaeyrPtr, { 0x7630 }) + 0x60, 'float')

            -- 如果当前血量小于最大血量，设置当前血量等于最大血量
            if current < max then
                SetAddressData(GetAddress(plaeyrPtr, { 0x7630 }) + 0x64, 'float', max)
            end
        end
    end
end
