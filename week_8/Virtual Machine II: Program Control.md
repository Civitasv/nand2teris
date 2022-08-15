# Virtual Machine II: Program Control

> If everything seems under control, you're just not going fast enough.
> Mario Andretti (b. 1940), race car champion.

本章介绍如何使用 Stack Machine 实现函数调用，分支等。

## Program Flow

![program flow](images/Program_Flow.png)  

## Subroutine Calling

subroutines, procesures, or functions.

![subroutine](images/subroutine.png)  

![call stack](images/call_stack.png)  

## VM Spec

### Program Flow Commands

- label *label*
  
  ```hack
  (label)
  ```

- goto *label*

  ```hack
  @label
  0;JMP
  ```

- if-goto *label*

  ```hack
  @SP
  AM=M-1
  D=M
  @label
  D;JNE
  ```

### Function Calling Commands

Facinating part.

- function *f* *n*

    ```hack
    ```

- call *f* *m*
- return
