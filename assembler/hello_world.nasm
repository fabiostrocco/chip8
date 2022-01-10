%include "chip8.inc"

ld v2, 0x78
ldi 0x500
ld v3, 0x1
ld v4, 0x1

ldk v1
cls
ldst v2
ldf v1
drw 3, 4, 5
jmp 0x200