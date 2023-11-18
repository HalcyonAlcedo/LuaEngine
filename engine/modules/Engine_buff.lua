engine_buff = {
    info = {
        name = 'Buff'
    },
    buff = {
        b38 = 0,
        b40 = 0,
        b48 = 0,
        b50 = 0,
        b58 = 0,
        b60 = 0,
        b68 = 0,
        b84 = 0,
        b8c = 0,
        b90 = 0,
        b94 = 0,
        b98 = 0,
        b9c = 0,
        ba0 = 0,
        ba4 = 0,
        ba8 = 0,
        bb0 = 0,
        bb8 = 0,
        bc0 = 0,
        bc8 = 0,
        bd0 = 0,
        bd4 = 0,
        bd8 = 0,
        be4 = 0,
        bec = 0,
        bf0 = 0,
        bf4 = 0,
        bfc = 0,
        b10c = 0,
        b110 = 0,
        b114 = 0,
        b118 = 0,
        b11c = 0,
        b124 = 0,
        b12c = 0,
        b130 = 0,
        b134 = 0,
        b138 = 0,
        b13c = 0,
        b144 = 0,
        b14c = 0,
        b150 = 0,
        b154 = 0,
        b158 = 0,
        b604 = 0,
        b690 = 0,
        b694 = 0,
        b698 = 0,
        b6a0 = 0,
        b6b0 = 0,
        b6c4 = 0,
        b6c8 = 0,
        b6cc = 0,
        b6d0 = 0,
        b6ec = 0,
        b6f0 = 0,
        b6f8 = 0,
        b6fc = 0,
        b71c = 0,
        b718 = 0,
        b754 = 0,
        b764 = 0,
        b770 = 0,
        b79c = 0,
        b7a0 = 0,
        b748 = 0,
        b74c = 0,
        b7c8 = 0,
    },
    debuff = {
        b5ec = 0,
        b5f0 = 0,
        b5f4 = 0,
        b5f8 = 0,
        b5fc = 0,
        b600 = 0,
        b608 = 0,
        b5dc = 0,
        b5e0 = 0,
        b62c = 0,
        b60c = 0,
        b614 = 0,
        b61c = 0,
        b620 = 0,
    }
}

local pointer = {
    buff = function() return GetAddress(0x145011760,{ 0x50, 0x7D20 }) end,
}

--获取玩家buff
function engine_buff:getbuff()
    if not pointer:buff() then 
        return { 
        b38  = 0,        b40  = 0,        b48  = 0,        b50  = 0,        b58  = 0,
        b60  = 0,        b68  = 0,        b84  = 0,        b8c  = 0,        b90  = 0,
        b94  = 0,        b98  = 0,        b9c  = 0,        ba0  = 0,        ba4  = 0,
        ba8  = 0,        bb0  = 0,        bb8  = 0,        bc0  = 0,        bc8  = 0,
        bd0  = 0,        bd4  = 0,        bd8  = 0,        be4  = 0,        bec  = 0,
        bf0  = 0,        bf4  = 0,        bfc  = 0,        b10c = 0,        b110 = 0,
        b114 = 0,        b118 = 0,        b11c = 0,        b124 = 0,        b12c = 0,
        b130 = 0,        b134 = 0,        b138 = 0,        b13c = 0,        b144 = 0,
        b14c = 0,        b150 = 0,        b154 = 0,        b158 = 0,        b604 = 0,
        b690 = 0,        b694 = 0,        b698 = 0,        b6a0 = 0,        b6b0 = 0,
        b6c4 = 0,        b6c8 = 0,        b6cc = 0,        b6d0 = 0,        b6ec = 0,
        b6f0 = 0,        b6f8 = 0,        b6fc = 0,        b71c = 0,        b718 = 0,
        b754 = 0,        b764 = 0,        b770 = 0,        b79c = 0,        b7a0 = 0,
        b748 = 0,        b74c = 0,        b7c8 = 0,
        } 
    end
    return { 
        b38  = GetAddressData(pointer:buff() + 0x38 , 'float'),
        b40  = GetAddressData(pointer:buff() + 0x40 , 'float'),
        b48  = GetAddressData(pointer:buff() + 0x48 , 'float'),
        b50  = GetAddressData(pointer:buff() + 0x50 , 'float'),
        b58  = GetAddressData(pointer:buff() + 0x58 , 'float'),
        b60  = GetAddressData(pointer:buff() + 0x60 , 'float'),
        b68  = GetAddressData(pointer:buff() + 0x68 , 'float'),
        b84  = GetAddressData(pointer:buff() + 0x84 , 'float'),
        b8c  = GetAddressData(pointer:buff() + 0x8c , 'float'),
        b90  = GetAddressData(pointer:buff() + 0x90 , 'float'),
        b94  = GetAddressData(pointer:buff() + 0x94 , 'float'),
        b98  = GetAddressData(pointer:buff() + 0x98 , 'float'),
        b9c  = GetAddressData(pointer:buff() + 0x9c , 'float'),
        ba0  = GetAddressData(pointer:buff() + 0xa0 , 'float'),
        ba4  = GetAddressData(pointer:buff() + 0xa4 , 'float'),
        ba8  = GetAddressData(pointer:buff() + 0xa8 , 'float'),
        bb0  = GetAddressData(pointer:buff() + 0xb0 , 'float'),
        bb8  = GetAddressData(pointer:buff() + 0xb8 , 'float'),
        bc0  = GetAddressData(pointer:buff() + 0xc0 , 'float'),
        bc8  = GetAddressData(pointer:buff() + 0xc8 , 'float'),
        bd0  = GetAddressData(pointer:buff() + 0xd0 , 'float'),
        bd4  = GetAddressData(pointer:buff() + 0xd4 , 'float'),
        bd8  = GetAddressData(pointer:buff() + 0xd8 , 'float'),
        be4  = GetAddressData(pointer:buff() + 0xe4 , 'float'),
        bec  = GetAddressData(pointer:buff() + 0xec , 'float'),
        bf0  = GetAddressData(pointer:buff() + 0xf0 , 'float'),
        bf4  = GetAddressData(pointer:buff() + 0xf4 , 'float'),
        bfc  = GetAddressData(pointer:buff() + 0xfc , 'float'),
        b10c = GetAddressData(pointer:buff() + 0x10c, 'float'),
        b110 = GetAddressData(pointer:buff() + 0x110, 'float'),
        b114 = GetAddressData(pointer:buff() + 0x114, 'float'),
        b118 = GetAddressData(pointer:buff() + 0x118, 'float'),
        b11c = GetAddressData(pointer:buff() + 0x11c, 'float'),
        b124 = GetAddressData(pointer:buff() + 0x124, 'float'),
        b12c = GetAddressData(pointer:buff() + 0x12c, 'float'),
        b130 = GetAddressData(pointer:buff() + 0x130, 'float'),
        b134 = GetAddressData(pointer:buff() + 0x134, 'float'),
        b138 = GetAddressData(pointer:buff() + 0x138, 'float'),
        b13c = GetAddressData(pointer:buff() + 0x13c, 'float'),
        b144 = GetAddressData(pointer:buff() + 0x144, 'float'),
        b14c = GetAddressData(pointer:buff() + 0x14c, 'float'),
        b150 = GetAddressData(pointer:buff() + 0x150, 'float'),
        b154 = GetAddressData(pointer:buff() + 0x154, 'float'),
        b158 = GetAddressData(pointer:buff() + 0x158, 'float'),
        b604 = GetAddressData(pointer:buff() + 0x604, 'float'),
        b690 = GetAddressData(pointer:buff() + 0x690, 'float'),
        b694 = GetAddressData(pointer:buff() + 0x694, 'float'),
        b698 = GetAddressData(pointer:buff() + 0x698, 'float'),
        b6a0 = GetAddressData(pointer:buff() + 0x6a0, 'float'),
        b6b0 = GetAddressData(pointer:buff() + 0x6b0, 'float'),
        b6c4 = GetAddressData(pointer:buff() + 0x6c4, 'float'),
        b6c8 = GetAddressData(pointer:buff() + 0x6c8, 'float'),
        b6cc = GetAddressData(pointer:buff() + 0x6cc, 'float'),
        b6d0 = GetAddressData(pointer:buff() + 0x6d0, 'float'),
        b6ec = GetAddressData(pointer:buff() + 0x6ec, 'float'),
        b6f0 = GetAddressData(pointer:buff() + 0x6f0, 'float'),
        b6f8 = GetAddressData(pointer:buff() + 0x6f8, 'float'),
        b6fc = GetAddressData(pointer:buff() + 0x6fc, 'float'),
        b71c = GetAddressData(pointer:buff() + 0x71c, 'float'),
        b718 = GetAddressData(pointer:buff() + 0x718, 'float'),
        b754 = GetAddressData(pointer:buff() + 0x754, 'float'),
        b764 = GetAddressData(pointer:buff() + 0x764, 'float'),
        b770 = GetAddressData(pointer:buff() + 0x770, 'float'),
        b79c = GetAddressData(pointer:buff() + 0x79c, 'float'),
        b7a0 = GetAddressData(pointer:buff() + 0x7a0, 'float'),
        b748 = GetAddressData(pointer:buff() + 0x748, 'float'),
        b74c = GetAddressData(pointer:buff() + 0x74c, 'float'),
        b7c8 = GetAddressData(pointer:buff() + 0x7c8, 'float'),
    } 
end
--获取玩家debuff
function engine_buff:getdebuff()
    if not pointer:buff() then return {
        b5ec = 0,        b5f0 = 0,        b5f4 = 0,        b5f8 = 0,
        b5fc = 0,        b600 = 0,        b608 = 0,        b5dc = 0,
        b5e0 = 0,        b62c = 0,        b60c = 0,        b614 = 0,
        b61c = 0,        b620 = 0,
    }
    end
    return {
        b5ec = GetAddressData(pointer:buff() + 0x5ec, 'float'),
        b5f0 = GetAddressData(pointer:buff() + 0x5f0, 'float'),
        b5f4 = GetAddressData(pointer:buff() + 0x5f4, 'float'),
        b5f8 = GetAddressData(pointer:buff() + 0x5f8, 'float'),
        b5fc = GetAddressData(pointer:buff() + 0x5fc, 'float'),
        b600 = GetAddressData(pointer:buff() + 0x600, 'float'),
        b608 = GetAddressData(pointer:buff() + 0x608, 'float'),
        b5dc = GetAddressData(pointer:buff() + 0x5dc, 'float'),
        b5e0 = GetAddressData(pointer:buff() + 0x5e0, 'float'),
        b62c = GetAddressData(pointer:buff() + 0x62c, 'float'),
        b60c = GetAddressData(pointer:buff() + 0x60c, 'float'),
        b614 = GetAddressData(pointer:buff() + 0x614, 'float'),
        b61c = GetAddressData(pointer:buff() + 0x61c, 'float'),
        b620 = GetAddressData(pointer:buff() + 0x620, 'float'),
    }
end

--监听
local function traceHandle(k,v)
    if k == 'b38' then SetAddressData(pointer:buff() + 0x38,'float',v) return end
    if k == 'b40' then SetAddressData(pointer:buff() + 0x40,'float',v) return end
    if k == 'b48' then SetAddressData(pointer:buff() + 0x48,'float',v) return end
    if k == 'b50' then SetAddressData(pointer:buff() + 0x50,'float',v) return end
    if k == 'b58' then SetAddressData(pointer:buff() + 0x58,'float',v) return end
    if k == 'b60' then SetAddressData(pointer:buff() + 0x60,'float',v) return end
    if k == 'b68' then SetAddressData(pointer:buff() + 0x68,'float',v) return end
    if k == 'b84' then SetAddressData(pointer:buff() + 0x84,'float',v) return end
    if k == 'b8c' then SetAddressData(pointer:buff() + 0x8c,'float',v) return end
    if k == 'b90' then SetAddressData(pointer:buff() + 0x90,'float',v) return end
    if k == 'b94' then SetAddressData(pointer:buff() + 0x94,'float',v) return end
    if k == 'b98' then SetAddressData(pointer:buff() + 0x98,'float',v) return end
    if k == 'b9c' then SetAddressData(pointer:buff() + 0x9c,'float',v) return end
    if k == 'ba0' then SetAddressData(pointer:buff() + 0xa0,'float',v) return end
    if k == 'ba4' then SetAddressData(pointer:buff() + 0xa4,'float',v) return end
    if k == 'ba8' then SetAddressData(pointer:buff() + 0xa8,'float',v) return end
    if k == 'bb0' then SetAddressData(pointer:buff() + 0xb0,'float',v) return end
    if k == 'bb8' then SetAddressData(pointer:buff() + 0xb8,'float',v) return end
    if k == 'bc0' then SetAddressData(pointer:buff() + 0xc0,'float',v) return end
    if k == 'bc8' then SetAddressData(pointer:buff() + 0xc8,'float',v) return end
    if k == 'bd0' then SetAddressData(pointer:buff() + 0xd0,'float',v) return end
    if k == 'bd4' then SetAddressData(pointer:buff() + 0xd4,'float',v) return end
    if k == 'bd8' then SetAddressData(pointer:buff() + 0xd8,'float',v) return end
    if k == 'be4' then SetAddressData(pointer:buff() + 0xe4,'float',v) return end
    if k == 'bec' then SetAddressData(pointer:buff() + 0xec,'float',v) return end
    if k == 'bf0' then SetAddressData(pointer:buff() + 0xf0,'float',v) return end
    if k == 'bf4' then SetAddressData(pointer:buff() + 0xf4,'float',v) return end
    if k == 'bfc' then SetAddressData(pointer:buff() + 0xfc,'float',v) return end
    if k == 'b10c' then SetAddressData(pointer:buff() + 0x10c,'float',v) return end
    if k == 'b110' then SetAddressData(pointer:buff() + 0x110,'float',v) return end
    if k == 'b114' then SetAddressData(pointer:buff() + 0x114,'float',v) return end
    if k == 'b118' then SetAddressData(pointer:buff() + 0x118,'float',v) return end
    if k == 'b11c' then SetAddressData(pointer:buff() + 0x11c,'float',v) return end
    if k == 'b124' then SetAddressData(pointer:buff() + 0x124,'float',v) return end
    if k == 'b12c' then SetAddressData(pointer:buff() + 0x12c,'float',v) return end
    if k == 'b130' then SetAddressData(pointer:buff() + 0x130,'float',v) return end
    if k == 'b134' then SetAddressData(pointer:buff() + 0x134,'float',v) return end
    if k == 'b138' then SetAddressData(pointer:buff() + 0x138,'float',v) return end
    if k == 'b13c' then SetAddressData(pointer:buff() + 0x13c,'float',v) return end
    if k == 'b144' then SetAddressData(pointer:buff() + 0x144,'float',v) return end
    if k == 'b14c' then SetAddressData(pointer:buff() + 0x14c,'float',v) return end
    if k == 'b150' then SetAddressData(pointer:buff() + 0x150,'float',v) return end
    if k == 'b154' then SetAddressData(pointer:buff() + 0x154,'float',v) return end
    if k == 'b158' then SetAddressData(pointer:buff() + 0x158,'float',v) return end
    if k == 'b604' then SetAddressData(pointer:buff() + 0x604,'float',v) return end
    if k == 'b690' then SetAddressData(pointer:buff() + 0x690,'float',v) return end
    if k == 'b694' then SetAddressData(pointer:buff() + 0x694,'float',v) return end
    if k == 'b698' then SetAddressData(pointer:buff() + 0x698,'float',v) return end
    if k == 'b6a0' then SetAddressData(pointer:buff() + 0x6a0,'float',v) return end
    if k == 'b6b0' then SetAddressData(pointer:buff() + 0x6b0,'float',v) return end
    if k == 'b6c4' then SetAddressData(pointer:buff() + 0x6c4,'float',v) return end
    if k == 'b6c8' then SetAddressData(pointer:buff() + 0x6c8,'float',v) return end
    if k == 'b6cc' then SetAddressData(pointer:buff() + 0x6cc,'float',v) return end
    if k == 'b6d0' then SetAddressData(pointer:buff() + 0x6d0,'float',v) return end
    if k == 'b6ec' then SetAddressData(pointer:buff() + 0x6ec,'float',v) return end
    if k == 'b6f0' then SetAddressData(pointer:buff() + 0x6f0,'float',v) return end
    if k == 'b6f8' then SetAddressData(pointer:buff() + 0x6f8,'float',v) return end
    if k == 'b6fc' then SetAddressData(pointer:buff() + 0x6fc,'float',v) return end
    if k == 'b71c' then SetAddressData(pointer:buff() + 0x71c,'float',v) return end
    if k == 'b718' then SetAddressData(pointer:buff() + 0x718,'float',v) return end
    if k == 'b754' then SetAddressData(pointer:buff() + 0x754,'float',v) return end
    if k == 'b764' then SetAddressData(pointer:buff() + 0x764,'float',v) return end
    if k == 'b770' then SetAddressData(pointer:buff() + 0x770,'float',v) return end
    if k == 'b79c' then SetAddressData(pointer:buff() + 0x79c,'float',v) return end
    if k == 'b7a0' then SetAddressData(pointer:buff() + 0x7a0,'float',v) return end
    if k == 'b748' then SetAddressData(pointer:buff() + 0x748,'float',v) return end
    if k == 'b74c' then SetAddressData(pointer:buff() + 0x74c,'float',v) return end
    if k == 'b7c8' then SetAddressData(pointer:buff() + 0x7c8,'float',v) return end
    if k == 'b5ec' then SetAddressData(pointer:buff() + 0x5ec,'float',v) return end
    if k == 'b5f0' then SetAddressData(pointer:buff() + 0x5f0,'float',v) return end
    if k == 'b5f4' then SetAddressData(pointer:buff() + 0x5f4,'float',v) return end
    if k == 'b5f8' then SetAddressData(pointer:buff() + 0x5f8,'float',v) return end
    if k == 'b5fc' then SetAddressData(pointer:buff() + 0x5fc,'float',v) return end
    if k == 'b600' then SetAddressData(pointer:buff() + 0x600,'float',v) return end
    if k == 'b608' then SetAddressData(pointer:buff() + 0x608,'float',v) return end
    if k == 'b5dc' then SetAddressData(pointer:buff() + 0x5dc,'float',v) return end
    if k == 'b5e0' then SetAddressData(pointer:buff() + 0x5e0,'float',v) return end
    if k == 'b62c' then SetAddressData(pointer:buff() + 0x62c,'float',v) return end
    if k == 'b60c' then SetAddressData(pointer:buff() + 0x60c,'float',v) return end
    if k == 'b614' then SetAddressData(pointer:buff() + 0x614,'float',v) return end
    if k == 'b61c' then SetAddressData(pointer:buff() + 0x61c,'float',v) return end
    if k == 'b620' then SetAddressData(pointer:buff() + 0x620,'float',v) return end
end

local index = {}
local mt = {
	__index = function(t, k)
		return t[index][k]
	end,
    __newindex = function (t,k,v)
        traceHandle(k,v)
    	t[index][k] = v
    end
}
local function trace(t)
	local proxy = {}
	proxy[index] = t
	setmetatable(proxy, mt)
	return proxy
end

function engine_buff:new()
    local o = {}
    o.buff = self:getbuff()
    o.debuff = self:getdebuff()

    o.buff = trace(o.buff)
    o.debuff = trace(o.debuff)

    setmetatable(o, self)
    self.__index = self
    return o
end
return engine_buff