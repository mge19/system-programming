;BLG413E-System Programming Assignment-1
;Mehmet Gencay Ertürk-150130118
section .text 
global cross_correlation_asm_full
cross_correlation_asm_full:
push ebp
mov ebp,esp
push ebx
mov eax,0
mov ecx,0
control: cmp ecx,[ebp+28]
je end
mov ebx,[ebp+24]
mov edx,0
mov [ebx+ecx*4],edx
cmp ecx,[ebp+20]
jl assign
mov edx,ecx
sub edx,[ebp+20]
inc edx
mov edi,0
jmp loop
assign: mov edx,0
mov edi,[ebp+20]
sub edi,ecx
dec edi
loop: cmp edx,[ebp+12]
je continue
cmp edi,[ebp+20]
je continue
mov ebx,[ebp+8]
mov ebx,[ebx+4*edx]
mov esi,[ebp+16]
mov esi,[esi+4*edi]
imul ebx,esi
mov esi,[ebp+24]
add [esi+4*ecx],ebx
inc edx
inc edi
jmp loop
continue: mov ebx,[ebp+24]
mov esi,[ebx+ecx*4]
cmp esi,0
je zero
inc eax
zero: inc ecx
jmp control
end: pop ebx
mov esp,ebp
pop ebp
ret
