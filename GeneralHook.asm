
.data

.const

.code

; 函数：保存寄存器到 C++ 的 Registers 结构
SaveRegisters proc
    ; 将寄存器值存入结构体
    push rbx 
    mov [rcx], rax         ; rax -> regs.rax
    mov [rcx + 16], rbx    ; rcx -> regs.rcx
    mov rbx, [rsp + 8]
    mov [rcx + 8], rbx     ; rbx -> regs.rbx
    mov [rcx + 24], rdx    ; rdx -> regs.rdx
    mov [rcx + 32], rsi    ; rsi -> regs.rsi
    mov [rcx + 40], rdi    ; rdi -> regs.rdi
    mov [rcx + 48], r8     ; r8  -> regs.r8
    mov [rcx + 56], r9     ; r9  -> regs.r9
    mov [rcx + 64], r10    ; r10 -> regs.r10
    mov [rcx + 72], r11    ; r11 -> regs.r11
    mov [rcx + 80], r12    ; r12 -> regs.r12
    mov [rcx + 88], r13    ; r13 -> regs.r13
    mov [rcx + 96], r14    ; r14 -> regs.r14
    mov [rcx + 104], r15    ; r15 -> regs.r15
    pop rbx 
    ret
SaveRegisters endp

; 函数：从 C++ 的 Registers 结构恢复寄存器
RestoreRegisters proc
    ; 从结构体中恢复寄存器
    mov rax, [rcx]         ; regs.rax -> rax
    ;mov rbx, [rcx + 8]     ; regs.rbx -> rbx
    mov rdx, [rcx + 24]    ; regs.rdx -> rdx
    mov rsi, [rcx + 32]    ; regs.rsi -> rsi
    mov rdi, [rcx + 40]    ; regs.rdi -> rdi
    mov r8,  [rcx + 48]    ; regs.r8 -> r8
    mov r9,  [rcx + 56]    ; regs.r9 -> r9
    mov r10, [rcx + 64]    ; regs.r10 -> r10
    mov r11, [rcx + 72]    ; regs.r11 -> r11
    mov r12, [rcx + 80]    ; regs.r12 -> r12
    mov r13, [rcx + 88]    ; regs.r13 -> r13
    mov r14, [rcx + 96]    ; regs.r14 -> r14
    mov r15, [rcx + 104]   ; regs.r15 -> r15
    mov rcx, [rcx + 16]    ; regs.rcx -> rcx
    ret
RestoreRegisters endp

end
