# 1. 操作系统怎么加载
## 1.1 MBR(主引导记录)
 BIOS(主板上ROM装载的一个内置程序)会加载磁盘上第一个扇区的512 byte 序列


# 2. CPU寄存器
## 2.1 通用寄存器
### 2.1.1 16位(均由 8高位H 8低位L 两个寄存器组成)
AX(累加寄存器)add
CX(计数寄存器)count
DX(数据寄存器)data
BX(基址寄存器)base
[要在以下寄存器取值，需要先move到AX中然后取AL,AH来取低位高位的值]
SP(栈指针寄存器) [stack]
BP(基址指针寄存器)[base]
SI(源变址寄存器)[source index]
DI(目的变址寄存器)[destination index]

### 2.1.2 32位(高16,低16)
EAX(累加寄存器) 4 byte ，只能取低位2byte ，高位2byte 需要MOVE AL才可以取值
ECX(计数寄存器)
EDX(数据寄存器)
EBX(基址寄存器)
ESP(栈指针寄存器)
EBP(基址指针寄存器)
ESI(源变址寄存器)
EDI(目的变址寄存器)

### 2.1.2 64位(高16,低16)
RAX(累加寄存器) 8 byte ，只能取低位2byte ，高位2byte 需要MOVE AL才可以取值
RCX(计数寄存器)
RDX(数据寄存器)
RBX(基址寄存器)
RSP(栈指针寄存器)
RBP(基址指针寄存器)
RSI(源变址寄存器)
RDI(目的变址寄存器)
## 2.2 段寄存器(均为16位)
ES-附加段寄存器 extra
CS-代码段寄存器 code
SS-栈段寄存器 stack
DS-数据段寄存器 data , 用于内存寻址 value*段寄存器的2^bit+常数
FS-noname(segment part2)
GS-noname(segment part3)