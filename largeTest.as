; file ps.as
.entry LIST
.extern W 
.define sz = 2
.define C = -999
mcr	uniSucks
	sub #sz , r3
	prn #-5
endmcr

uniSucks
MAIN:	mov r3, LIST[sz]
mcr Maicos
	bne W
	add C, r3
L1: 	inc L3
endmcr 
LOOP: 	jmp W

	mov STR[5], STR[2] 
	sub r1, r4
	cmp K, #sz
Maicos
.entry LOOP
	bne LOOP 
END: hlt
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, len
K: .data 22 
.extern L3