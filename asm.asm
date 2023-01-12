
.data

.const

.code

GetRAXPtr proc
    mov [rcx], rax
	ret
GetRAXPtr endp

GetRBXPtr proc
    mov [rcx], rbx
	ret
GetRBXPtr endp

GetRDIPtr proc
    mov [rcx], rdi
	ret
GetRDIPtr endp

SetEDX proc
    mov edx, [rcx]
	ret
SetEDX endp

SetR14D proc
    mov r14d, [rcx]
	ret
SetR14D endp

end