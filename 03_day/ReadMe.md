## 1. ipl10.nas + haribote.nas
### 1.1 执行逻辑
	1. ipl10.nas 编译成机器码ipl10.bin(512byte),及 haribote.nas编译生成haribote.bin
	2. 准备空白软盘文件一个 first_step.img
	3. edimg 将ipl10.bin写入软盘镜像的前512个byte，然后从0x4200处写入haribote.bin
	4. qemu 载入 first_step.img 第1扇区到0X7C00处
	5. CPU跳转到0x7c00处执行(JMP entry)
	6. ipl10.nas循环载入10个柱面，起始位为0x8000,从0柱面的2扇区开始载入,共计10个柱面(10*2*18-1)
	7. JMP到0x8000+0x4000(0x4200-0x0200)处执行haribote.nas代码
### 1.2 尚且未理解的点
	1. ORG 这个指令的含义