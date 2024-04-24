mcr m_mcr
        cmp r3, r1
endmcr

prn #-5


mcr sss
        sub r1, r4
endmcr
.define sz = 2
.entry C
mov STR[5], STR[2]
mov DATA[2], DATA[3]
m_mcr
sss
C: .data 1,2
END:    hlt
STR:    .string "abcdef"
DATA: cmp #2,r3