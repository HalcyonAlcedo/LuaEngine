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
        -- 获取玩家地址(失败返回 false)
        local plaeyrPtr = GetAddress(playerBasePtr,{ 0x50 })
        if plaeyrPtr then
            -- 获取血量数据地址:失败返回 false,必须先用 if 判断,
            -- 否则 false 参与算术(+ 0x64)会报错
            local healthPtr = GetAddress(plaeyrPtr, { 0x7630 })
            if healthPtr then
                -- 当前血量(读取失败时降级为 0,保证比较运算安全)
                local current = GetAddressData(healthPtr + 0x64, 'float') or 0
                -- 最大血量
                local max = GetAddressData(healthPtr + 0x60, 'float') or 0

                -- 如果当前血量小于最大血量，设置当前血量等于最大血量
                if current < max then
                    SetAddressData(healthPtr + 0x64, 'float', max)
                end
            end
        end
    end
end
