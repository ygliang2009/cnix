#ifndef _KEYMAP_H
#define _KEYMAP_H

#define		DISKEYMASK		0x1000
#define		ESC			0x1000
#define 	BACK			0x1001
#define		TAB			0x1002
#define		CR			0x1003
#define		LC			0x1004
#define		LEFTSHIFT		0x1005
#define		RIGHTSHIFT		0x1006
#define		ALT			0x1007
#define		CAP_LOCKS		0x1008
#define		F1			0x1009
#define		F2			0x100A
#define		F3			0x100B
#define		F4			0x100C
#define		F5			0x100D
#define		F6			0x100E
#define		F7			0x100F
#define		F8			0x1010
#define		F9			0x1011
#define		F10			0x1012
#define		NUMLOCK			0x1013
#define		SCRLOCK			0x1014
#define		HOME			0x1015
#define		CURUP			0x1016
#define		PGUP			0x1017
#define		REDUCE			0x1018
#define		LEFT			0x1019
#define		MID			0x101A
#define		RIGHT			0x101B
#define		PLUS			0x101C
#define		END			0x101D
#define		DOWN			0x101E
#define		PGDOWN			0x101F
#define		INSERT			0x1020
#define		DELETE			0x1021
#define		F11			0x1022
#define		F12			0x1023

extern int key_mapper[];

/* 显示器属性 */
typedef struct SCREEN{
	/* 显示的起始位置 */
	int start_pos;
	/* 起始光标位置 */
	int cursor_start_pos;
	/* 当前显示位置 */
	int disp_pos;
	/* 显示范围大小 */
	int vmem_size;
	/* 光标所在位置 */
	int cursor_pos;
}Screen;

/* Keyboard Control Block */
typedef struct KCB{
	/* 终端名称 */
	char name[16];
	/* 输入缓冲区 */
	char key_buf[NR_KEYBUF];
	/* 消费者指针 */
	int read_count;
	/* 生产者指针 */
	int write_count;
	/* 剩余未读字节数 */
	int left_count;
	/* 键盘控制缓冲区 */
	Screen screen;
}Kcb;

extern Kcb kcb;

extern Kcb tty[];

/* 当前是否按下shift键 */
extern int is_shift;
/* 当前是否按下alt键 */
extern int is_alt;

/* 当前显示的终端 */
extern Kcb *cur_tty;
#endif
