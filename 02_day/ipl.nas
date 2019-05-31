; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序装载地址 设定代码的装载首地址

; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code

		JMP		entry			;goto entry 汇编编译器会把entry 翻译替换成具体的地址
		DB		0x90			; data byte 0x90
		DB		"HELLOIPL"		; 启动扇区名称（8字节）
		DW		512				; 每个扇区（sector）大小（必须512字节）
		DB		1				; 簇（cluster）大小（必须为1个扇区）
		DW		1				; FAT起始位置（一般为第一个扇区）
		DB		2				; FAT个数（必须为2）
		DW		224				; 根目录大小（一般为224项）
		DW		2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
		DB		0xf0			; 磁盘类型（必须为0xf0）
		DW		9				; FAT的长度（必??9扇区）
		DW		18				; 一个磁道（track）有几个扇区（必须为18）
		DW		2				; 磁头数（必??2）
		DD		0				; 不使用分区，必须是0
		DD		2880			; 重写一次磁盘大小
		DB		0,0,0x29		; 意义不明（固定）
		DD		0xffffffff		; （可能是）卷标号码
		DB		"HELLO-OS   "	; 磁盘的名称（必须为11字节，不足填空格）
		DB		"FAT12   "		; 磁盘格式名称（必须为8字节，不足填空格）
		RESB	18				; 先空出18字节

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器 累加寄存器赋值0
		MOV		SS,AX			; 栈段寄存器把AX里面的值压栈
		MOV		SP,0x7c00		; 栈指针寄存器赋值为0x7c00 ,本段代码存储的首地址ORG 0x7c00
		MOV		DS,AX			; 段寄存器赋值AX里的值
		MOV		ES,AX			; 附加段寄存器压0
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		MOV		SI,msg			; 源变地址寄存器 压入 msg 代码段首地址
putloop:
		MOV		AL,[SI]			; 现在AL的值是SI所存储的值对应地址里面的值,书上的解释并不正确
							; mov al, 0x5 ; al = 0x5
							; mov si, 0x5 ; si = 0x5
							; mov al, [si] ; al = *si; that is, al is now what was in memory at address 0x5;也就是说AL里面的值是内存地址0x5所存储的值
		ADD		SI,1			; 给SI加1
		CMP		AL,0
		JE		fin			; 如果AL里面的值是0
		INT		0x10			; 调用显卡BIOS
		JMP		putloop
fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环

msg:
		DB		0x0a, 0x0a		; 换行两次
		DB		"hello, world"
		DB		0x0a
		DB		"this should display"
		DB		0x0a			; 换行
		DB		0

		RESB	0x7dfe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa
; 下面是一些伪代码 对以上会汇编的解释
; msg = &byte [0x0a,0x0a,",h,e,l,l,o, w,o,r,l,d,0"]			<==> 声明一个byte array并赋值 数组首地址给msg
; ax = 0			<==> 			MOV		AX,0			<==> ax = 0
; ss = ax			<==>			MOV		SS,AX			<==> ss = 0
; sp = 0x7c00		<==>			MOV		SP,0x7c00		<==> sp = 0x7c00
; ds = ax			<==>			MOV		DS,AX			<==> ds = ax 
; ex = ax			<==>			MOV		ES,AX			<==> ex = ax 
; si = msg			<==>			MOV		SI,msg			<==> si = msg		
; loop:
;	al = *si		<==>			MOV		AL,[SI]			<==> al 赋值 *si,	
;	si = si+1 		<==>			ADD		SI,1 			<==> si 值+1,结合上面就是si指向的地址后移
;	if(al == 0)		<==>			CMP		AL,0			<==> al 是不是 ==0 ,因为msg这个array的最后一个byte为0
;	{
;		goto fin	<==>			JE		fin				<==> goto close	
;	} 
;	ah = 0x0e 		<==>			MOV		AH,0x0e			<==> ah = 0xe 应该可以放到最开始,不用在循环里,verified
;	bx =15  		<==>			MOV		BX,15			<==> bx = 15 应该可以放到最开始,不用在循环里,verified
;	dis(ah,al,bh,bl)<==>			INT		0x10			<==> 执行方法	INT  0X10 需要读取 ah,al,bh,bl 四个寄存器的值
;	goto loop;