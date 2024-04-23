mcr m_mcr
        cmp r3, r1
endmcr
prn #-5

mcr sss
        sub r1, r4
endmcr
mov STR[5], STR[2] 
m_mcr
sss
END:    hlt
STR:    .string "abcdef"