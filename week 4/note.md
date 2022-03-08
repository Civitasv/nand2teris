# Week 4

**Universality**

Assembly language

## Machine Languages: Elements

## Hack Machine Language

**What is computer RAM?**

RAM is volatile memory, which means that the information temporarily stored in the module is erased when you restart or shut down your computer. Because the information is stored electrically on transistors, when there is no electric current, the data disappears. Each time you request a file or information, it is retrieved either from the computer's storage disk or the internet. The data is stored in RAM, so each time you switch from one program or page to another, the information is instantly available. When the computer is shut down, the memory is cleared until the process begins again. Volatile memory can be changed, upgraded, or expanded easily by users.

**What is ROM?**

ROM stands for non-volatile memory in computers, which means the information is permanently stored on the chip. The memory does not depend on an electric current to save data, instead, data is written to individual cells using binary code. Non-volatile memory is used for parts of the computer that do not change, such as the initial boot-up portion of the software, or the firmware instructions that make your printer run. Turning off the computer does not have any effect on ROM. Non-volatile memory cannot be changed by users.

### Hack computer Elements

1. Data memory (RAM): a sequence of 16-bit registers.
2. Instruction memory (ROM): a sequence of 16-bit registers.
3. Central Processing Unit (CPU): performs 16-bit instructions.

### Hack computer Language

+ 16-bit A-instructions
+ 16-bit C-instructions

The Hack machine language recognizes three registers:

- `D` holds a 16-bit value
- `A` holds a 16-bit value
- `M` represents the 16-bit RAM register addressed by A.

#### The A-instruction

Syntax: @value

- Sets the A register to value
- Side effect: RAM[A] becomes the selected RAM register, which is M

#### The C-instruction

Syntax: dest=comp; jump

if comp equals 0, then jump to execute the instruction in ROM[A]

