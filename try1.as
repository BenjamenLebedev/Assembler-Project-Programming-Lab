.entry LIST 
.extern W 
.define sz = 2
mcr maico
MAIN:   mov r3, LIST[sz]
LOOP:   jmp L1
endmr
        mcr m_mcr
                cmp r3, #sz
                bne END
        endmcr
        
        maico
        prn #-5
        mov STR[5], STR[2]
        sub r1, r4
        m_mcr
L1:     inc K
        bne LOOP
END:    hlt
.define len = 4
STR:    .string "abcdef"
LIST:   .data 6, -9, len
K:      .data 22 