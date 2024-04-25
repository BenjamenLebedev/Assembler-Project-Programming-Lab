mcr m_mcr
        cmp r3, r1
endmcr

mcr ssa
;comment
endmcr

prn #-5
.define C = -8
mcr sss
        sub r1, r4
endmcr
.define sz = 2
.entry C
mov STR[5], STR[2] 
m_mcr
sss
cmp C[2] , #4
C: .data 1,2
END:    hlt
STR:    .string "abcdef"