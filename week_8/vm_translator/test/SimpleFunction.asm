// function: SimpleFunction.test
(SimpleFunction.test)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
// push local 0
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// push local 1
@LCL
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
AM=M-1
D=M
A=A-1
M=M+D
// not
@SP
A=M-1
M=!M
// push argument 0
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
AM=M-1
D=M
A=A-1
M=M+D
// push argument 1
@ARG
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// sub
@SP
AM=M-1
D=M
A=A-1
M=M-D
// return: SimpleFunction.test
// endFrame=LCL: 
@LCL
A=M
D=M
@R13
M=D
// retAddr=*(endFrame-5): 
@5
A=D-A
D=M
@R14
M=D
// *ARG=pop(): 
@SP
AM=M-1
D=M
@ARG
A=M
M=D
// SP+ARG+1: 
@ARG
D=M
@1
D=D+A
@SP
M=D
// THAT = *(endFrame - 1): 
@R13
D=M
@1
A=D-A
D=M
@THAT
M=D
@R13
D=M
@2
A=D-A
D=M
@THAT
M=D
@R13
D=M
@3
A=D-A
D=M
@THAT
M=D
@R13
D=M
@4
A=D-A
D=M
@THAT
M=D
@R14
A=M
0;JMP
