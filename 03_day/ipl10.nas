; haribote-ipl
; TAB=4

CYLS	EQU		10				; 声明CYLS=10,类似于静态变量或者C的define

		ORG		0x7c00			; 指明程序装载地址

; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; 启动扇区名称（8字节）
		DW		512				; 每个扇区（sector）大小（必须512字节）
		DB		1				; 簇（cluster）大小（必须为1个扇区）
		DW		1				; FAT起始位置（一般为第一个扇区）
		DB		2				; FAT个数（必须为2）
		DW		224				; 根目录大小（一般为224项）
		DW		2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
		DB		0xf0			; 磁盘类型（必须为0xf0）
		DW		9				; FAT的长度（必须为9扇区）
		DW		18				; 一个磁道（track）有几个扇区（必须为18）
		DW		2				; 磁头数（必须为2,因为教材假定为软盘,软盘基本为单盘片结构,0正1负两个磁头）
		DD		0				; 不使用分区，必须是0
		DD		2880			; 重写一次磁盘大小
		DB		0,0,0x29		; 意义不明（固定）
		DD		0xffffffff		; （可能是）卷标号码
		DB		"HARIBOTEOS "	; 磁盘的名称（必须为11字节，不足填空格）
		DB		"FAT12   "		; 磁盘格式名称（必须为8字节，不足填空格）
		RESB	18				; 先空出18字节

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX			; 栈段寄存器
		MOV		SP,0x7c00		; 栈指针寄存器
		MOV		DS,AX			; 数据段寄存器

; 读取磁盘

		MOV		AX,0x0800		; 这个不知道为什么,注意书上不完全正确,可用范围起始地址是0x7e00,
								; 可以参考规范 https://wiki.osdev.org/Memory_Map_(x86) ;
								; 结合到下文 jump 到0xc200= 0x8000+0x4200,所以推断这里磁盘存储文件应该是从0x8000开始，
								; 但是不知道为什么第一个扇区跳过去了..而不是书上所写的0x0820或者0x8200
		MOV		ES,AX			; 数据加载首地址 0x0820*0x0010 = 0x8200
		MOV		CH,0			; 柱面0 , 磁盘有多个盘片，请参考硬盘的物理结构
		MOV		DH,0			; 磁头0 , 每个盘片有两个正面一个反面一个
		MOV		CL,2			; 扇区2 ,注意：扇区计数从1开始,因为第一个扇区512byte是当前IPL的代码,已经被BIOS加载到内存0x7c00处

readloop:
		MOV		SI,0			; 记录失败次数

retry:
		MOV		AH,0x02			; AH=0x02 : 读入磁盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC		next			; 没出错则跳转到next , jump if not carry ,跳转if AH ==0
		ADD		SI,1			; 往SI加1
		CMP		SI,5			; 比较SI与5
		JAE		error			; SI >= 5 跳转到error,jump if above or equal
		MOV		AH,0x00			; 
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 重置驱动器
		JMP		retry
next:
		MOV		AX,ES			; 把内存地址后移0x200（512/16十六进制转换）
		ADD		AX,0x0020		; ES 计算时要*0x10
		MOV		ES,AX			; ADD ES,0x020因为没有ADD ES，只能通过AX进行
		ADD		CL,1			; 往CL里面加1
		CMP		CL,18			; 比较CL与18
		JBE		readloop		; CL <= 18 跳转到readloop ,jump if blow or equal,为什么是18呢？因为一个柱面设定为18个扇区
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 跳转到readloop，为什么是2 因为一个盘片有正反两面
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS 跳转到readloop

; 读取完毕，跳转到haribote.sys执行！
		MOV		[0x0ff0],CH		; 记录读取多少个柱面
		MOV		AX,0x8000		; 加载的代码的内存起始地址，即从文件的0x0200开始读取
		ADD		AX,0x4000		; 但是为什么是0x4200? 磁盘拼接文件生成的固定值,因为没有读取第一个sector，所有要减去0x0200 ，that is 512 byte 
		
		JMP		AX			; 跳转到0xc000(=0x8000+0x4200-0x0200)执行

error:
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; 给SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS
		JMP		putloop

fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环

msg:
		DB		0x0a, 0x0a		; 换行两次
		DB		"load error"
		DB		0x0a			; 换行
		DB		0

		RESB	0x7dfe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa
