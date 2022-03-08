// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

  // i = SCREEN
  @SCREEN
  D=A
  @i
  M=D

  // white = -1
  @white
  M=-1

(KBDLOOP)
  @KBD
  D=M

  @WHITEN
  D;JEQ

  @BLACKEN
  D;JNE

(SCRLOOP)
  // predicate if i < 24576
  @i
  D=M
  @KBD
  D=D-A
  @SCREND
  D;JEQ

  @white
  D=M

  @WHITENIT
  D;JEQ
  @BLACKENIT
  D;JLT

(WHITENIT)
  @i
  A=M
  M=0
  @i
  M=M+1
  @SCRLOOP
  0;JMP

(BLACKENIT)
  @i
  A=M
  M=-1
  @i
  M=M+1
  @SCRLOOP
  0;JMP

(SCREND)
  // i = SCREEN
  @SCREEN
  D=A
  @i
  M=D

  @KBDLOOP
  0;JMP

(WHITEN)
  @white
  M=0
  @SCRLOOP
  0;JMP
(BLACKEN)
  @white
  M=-1
  @SCRLOOP
  0;JMP
