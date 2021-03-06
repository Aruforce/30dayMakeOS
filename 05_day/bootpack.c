# include <stdio.h>
/*naskfunc.nas*/
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
/*inter c code*/
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/*
 * 启动信息struct
 */
struct BOOTINFO {
	char  cyls,leds,vmode,reserve;/*byte*/
	short scrnx,scrny;/*word*/
	char *vram;/*byte*/
};
/*内存分段 8byte*/
struct SEGMENT_DESCRIPTOR{
	short limit_low,base_low; //4,
	char base_mid,access_right; //2
	char limit_high,base_high; //2
};
/*中断表 8 byte*/
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char  dw_count, access_right;
	short offset_high;
};

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void HariMain(void)
{
	char *vram;/* 声明变量vram、用于BYTE [...]地址 */
	int xsize,ysize;
	struct BOOTINFO *bootinfo;
	
	char s[40], mcursor[256];
	int mx, my;
	
	bootinfo = (struct BOOTINFO *) 0x0ff0; /*也就是说从首地址开始转换,对应asmhead的里面的设定*/
	xsize = (*bootinfo).scrnx;
	ysize = (*bootinfo).scrny;
	vram = (*bootinfo).vram;
	init_palette();/* 设定调色板 */
	init_screen((*bootinfo).vram,bootinfo->scrnx,bootinfo->scrny);/* 根据 0xa0000 + x + y * 320 计算坐标 */
	putfont8_asc(bootinfo->vram,bootinfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	
	/* 显示鼠标 */
	mx = (bootinfo->scrnx - 16) / 2; /* 计算画面的中心坐标*/
	my = (bootinfo->scrny - 28 - 16) / 2;
	
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(bootinfo->vram, bootinfo->scrnx, 16, 16, mx, my, mcursor, 16);
	
	for (;;) {
		io_hlt();
	}
}
/**
 * 初始化调色板
 */
void init_palette(void)
{
	/* C语言中的static char语句只能用于数据，相当于汇编中的DB指令 */
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:梁红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, 15, table_rgb);
	return;
}
/**
 * 设置调色板
 */
void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	io_cli(); 					/* 将中断许可标志置为0,禁止中断 */
	io_out8(0x03c8, start); /*0x03c8*/
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}
/**
 *
 */
void init_screen(char *vram, int x, int y){
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}
/**
 * 画方框
 * vram:	像素点起始位置,是一个二维数组，
 * xsize：	列宽
 * c:		颜色类
 * x0,y0: 	框的左上坐标
 * x1,y1: 	框的右下坐标
 * 
 */
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}
/*
 *
 */
void putfont8_asc(char *vram, int xsize, int x, int y, char c, char *s){
	extern char hankaku[4096];
	for(;*s!=0x00;s++){
		putfont8(vram,xsize,x,y,c,hankaku+*s*16);
		x+=8;
	}
	return ;
}
void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}
void init_mouse_cursor8(char *mouse, char bc)
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}
void putblock8_8(char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

/*全局段号记录表及全局中断记录表初始化*/
void init_gdtidt(void){
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;//设置内存段段表首地址为0x00270000
	struct GATE_DESCRIPTOR	  *idt = (struct GATE_DESCRIPTOR *) 0x0026f800;//设置中断表首地址为0x0026f800
	int i ;
	for(i=0;i<8192;i++){ //因为DS段寄存器只有16bit,而且高3bit无法使用，也就是只能表示2^13=8*1024个数;假定32bit地址总线(也就是内存4GB),那么单页大小为0x80000=2^32/2^13
		set_segment(gdt+i,0,0,0);
	}
	
	set_segment(gdt + 1, 0xffffffff, 0x00000000, 0x4092);/*段1设定,*/
	set_segment(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);/*段2设定,512kb大小，首地址为0x280000,0x270000-0x27ffff为表的存储区域*/
	
	load_gdtr(0xffff, 0x00270000);/*大小,首地址*/
	
	/* IDT初始化 */
	for (i = 0; i < 256; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(0x7ff, 0x0026f800);/*大小,首地址*/
}
/**
 * sd: struct SEGMENT_DESCRIPTOR 的首地址
 * limit: 段空间大小
 * base: 起始位置
 * ar: 状态及权限
 */
void set_segment(struct SEGMENT_DESCRIPTOR *sd,unsigned int limit, int base, int ar){
	if(limit>0xfffff){
		ar |= 0x8000; //
		limit /= 0x1000; //段大小
	}
	sd->limit_low = limit & 0xffff;/*取后两个字节*/
	sd->base_low = base & 0xffff;/*取后两个字节*/
	sd->base_mid = (base>>16) & 0xff;/*取中间的字节*/
	sd->access_right = ar&0xff;
	sd->limit_high = ((limit>>16)&0x0f)|((ar>>8)&0xf0);
	sd->base_high = (base>>24)&0xff;
	return ;
}
/**
 * gd:首地址
 * offset:偏移量
 * selector：
 * ar：
 */
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}

