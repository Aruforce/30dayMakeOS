; haribote-os
; TAB=4
	ORG 	0xc000			; 这里告诉编译器,伪指令
	MOV		AH,0x0e			; 显示一个文字
	MOV		BX,15			; 指定字符颜色
	MOV		SI,msg			; 源变地址寄存器 压入 msg 代码段首地址
putloop:
	MOV		AL,[SI]			; 现在AL的值是SI所存储的值对应地址里面的值,书上的解释并不正确
	ADD		SI,1			; 给SI加1
	CMP		AL,0
	JE		fin			; 如果AL里面的值是0
	MOV		AH,0x0e			; 显示一个文字
	MOV		BX,15			; 指定字符颜色
	INT		0x10			; 调用显卡BIOS
	JMP		putloop
fin:
	HLT
	JMP fin
msg:
	DB		0x0a, 0x0a		; 换行两次
	DB		"hello, world"
	DB		0x0a
	DB		"this should display"
	DB		0x0a			; 换行
	DB		0