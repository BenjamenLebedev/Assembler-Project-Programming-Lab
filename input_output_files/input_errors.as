; Everything here is a syntax error EXCEPT for the data segment.
LABEL: .data 3,2,1
LIST: .string "hello   23 world"
.define sz = 2
.define imm = 5
mov #1  LABEL  
mov ,#sz , LABEL[1] 
mov #1 , #6 
mov NOTLABEL , NOTLABEL 
prn LABEL[5]
prn r7 , r5
rts #3
hlt #imm