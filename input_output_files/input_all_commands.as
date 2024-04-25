; this input is legal. all comands with all optional addressing methods 
mcr m_mcr 
inc r2
mov A,r1 
endmcr
LABEL: .data 3,2,1
LIST: .string "hello   23 world"
.define sz = 2
mov #1 , LABEL  
mov #sz , LABEL[1] 
mov #1 , r1 
mov LABEL , LABEL 
mov LABEL , LIST[sz] 
mov LABEL , r2 
mov LABEL[sz] , LABEL 
mov LABEL[sz] , LIST[4] 
mov LABEL[sz] , r3 
mov r4 , LABEL 
mov r5 , LABEL[sz] 
mov r6 , r7 
mcr m_mcr_new 
inc r2
mov A,r1 
endmcr
add #1 , LABEL 
add #sz , LABEL[1] 
add #1 , r1 
add LABEL , LABEL 
add LABEL , LIST[sz] 
add LABEL , r2 
add LABEL[sz] , LABEL 
add LABEL[sz] , LIST[4] 
add LABEL[sz] , r3 
add r4 , LABEL 
add r5 , LABEL[sz] 
add r6 , r7 
sub #1 , LABEL 
sub #sz , LABEL[1] 
sub #1 , r1 
sub LABEL , LABEL 
sub LABEL , LIST[sz] 
sub LABEL , r2 
sub LABEL[sz] , LABEL 
sub LABEL[sz] , LIST[4] 
sub LABEL[sz] , r3 
sub r4 , LABEL 
sub r5 , LABEL[sz] 
sub r6 , r7 
cmp #5 , #6
cmp #1 , LABEL 
cmp #sz , LABEL[1] 
cmp #1 , r1 
cmp LABEL , #sz
cmp LABEL , LABEL 
cmp LABEL , LIST[sz] 
cmp LABEL , r2 
cmp LABEL[1] , #sz
cmp LABEL[sz] , LABEL 
cmp LABEL[sz] , LIST[4] 
cmp LABEL[sz] , r3 
cmp r3 , #sz
cmp r4 , LABEL 
cmp r5 , LABEL[sz] 
cmp r6 , r7 
lea LIST , LABEL
lea LIST , LABEL[1]
lea LIST , r4
lea LIST[5] , LABEL
lea LIST[5] , LABEL[1]
lea LIST[5] , r4
not LIST
not LABEL[1]
not r4
clr LIST
clr LABEL[1]
clr r4
inc LIST
inc LABEL[1]
inc r4
dec LIST
dec LABEL[1]
dec r4
red LIST
red LABEL[1]
red r4
jmp LABEL
jmp r5
bne LABEL
bne r5
jsr LABEL
jsr r5
prn #6
prn LIST
prn LABEL[1]
prn r7 
rts
hlt