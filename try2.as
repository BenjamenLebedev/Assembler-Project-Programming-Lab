; hello there you little prick

HH: .entry Y
.extern Y
.define x = -2
LIST: .data 22,5,-3
DATA: .data 2, 3, LIST[x]
LABEL: cmp #2,LIST
LIST2: .data 4,2,5,LIST[LABEL]
LIST3: .data 4,3,STR[6],STR[2],STR[x]
STR: .string "abcdef22ds"