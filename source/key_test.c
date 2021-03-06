/*
	佐须之男 forgotfun.org
*/

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#define RED	(RGB8(0xff, 0x00, 0x00))
#define WHITE	(RGB8(0xff, 0xff, 0xff))
#define BLACK	(RGB8(0x00, 0x00, 0x00)) 
#define ORANGE	(RGB8(0xff, 0x99, 0x00))


u16 *vb = (u16*)VRAM;

void inline DrawHLine(u8 x, u8 y, u8 length, u8 clrid)
{
	u16 start_pixcel = x*240 + y;	
	u16 end_pixcel = start_pixcel + length; 
	u16 index;

	while (start_pixcel < end_pixcel)	
	{
		index = start_pixcel/2;

		if(start_pixcel&1) {
			vb[index] = (vb[index] & 0x00ff) |  (clrid<<8) ;
		} else {
			vb[index] = (vb[index] & 0xff00) |  (clrid) ;
		}

		start_pixcel++;
	}
}

void inline DrawVLine(u8 x, u8 y, u8 height, u8 clrid)
{
	u16 start_pixcel = x*240 + y;	
	u16 end_pixcel = (x + height)*240 + y; 
	u16 index;

	while (start_pixcel < end_pixcel)	
	{
		index = start_pixcel/2;

		if(start_pixcel&1) {
			vb[index] = (vb[index] & 0x00ff) |  (clrid<<8) ;
		} else {
			vb[index] = (vb[index] & 0xff00) |  (clrid) ;
		}

		start_pixcel += 240;
	}
}


void inline DrawFrame(u8 x, u8 y, u8 length , u8 height, u8 clrid)
{
	DrawHLine(x, y, length, clrid);	
	DrawVLine(x, y, height, clrid);

	DrawVLine(x, y + length - 1, height, clrid);	
	DrawHLine(x + height - 1, y, length, clrid); 
}

void inline DrawPoint(u8 x, u8 y, u8 clrid)
{
	u16 pixcel = x*240 + y;
	u16 index = pixcel/2;

	if(pixcel&1) {
		vb[index] = (vb[index] & 0x00ff) |  (clrid<<8) ;
	} else {
		vb[index] = (vb[index] & 0xff00) |  (clrid) ;
	}

}

void inline DrawRect(u8 x, u8 y, u8 length, u8 height, u8 clrid)
{
	u8 i, j;
	u16 pixcel;	
	u16 index;

	for (i = 0; i < length ; i++)	
	{
		for (j = 0; j < height ; j++) 
		{
			pixcel = (x + j)*240 + (y+i);		
			index = pixcel/2;

			if(pixcel&1) {
				vb[index] = (vb[index] & 0x00ff) |  (clrid<<8) ;
			} else {
				vb[index] = (vb[index] & 0xff00) |  (clrid) ;
			}
		}
	}

}

inline int DrawRound( u8 x,u8 y,u8 r,u8 clrid)
{
    u16 xx,rr;
    u16 xt,yt;
    u16 rs,row,col;
    yt = r;
    rr = ( unsigned int )r*r+1; // 补偿1 修正方形
    rs = ( yt+( yt>>1 ) )>>1; // (*0.75)分开1/8圆弧来画
    for ( xt=0; xt<=rs; xt++ )
    {
        xx = xt*xt;
        while ( ( yt*yt )>( rr-xx ) )
        {
            yt--;
        }
        row = x+xt; // 第一象限
        col = y-yt;
        DrawPoint( row,col,clrid);
        row = x-xt; // 第二象限
        DrawPoint( row,col,clrid);
        col = y+yt; // 第三象限
        DrawPoint( row,col,clrid);
        row = x+xt; // 第四象限
        DrawPoint( row,col,clrid);
	//***************45度镜象画另一半***************
        row = x+yt; // 第一象限
        col = y-xt;
        DrawPoint( row,col,clrid);
        row = x-yt; // 第二象限
        DrawPoint( row,col,clrid);
        col = y+xt; // 第三象限
        DrawPoint( row,col,clrid);
        row = x+yt; // 第四象限
        DrawPoint( row,col,clrid);

     }
}


void inline DrawCicle(u8 x, u8 y, u8 r, u8 clrid) 
{
    u16 xx,rr;
    u16 xt,yt;
    u16 rs,row,col;
    rr = r*r;

    for ( xt=0; xt <= r; xt++ )
    {
        xx = xt*xt;

	for (yt=0; yt <= r; yt++)		
	{
		if ( yt*yt <= rr - xx )
		{
			row = x+xt; // 第一象限
			col = y+yt;
			DrawPoint( row,col,clrid);
			row = x-xt; // 第二象限
			DrawPoint( row,col,clrid);
			col = y-yt; // 第三象限
			DrawPoint( row,col,clrid);
			row = x+xt; // 第四象限
			DrawPoint( row,col,clrid);
		}
	}
        
    }
}

void inline SetPalette(u8 clrid, u16 color)
{
	u16 *paletts = BG_COLORS;	

	*(paletts + clrid) = color;	
}

enum 
{
	KEY_A_COLOR = 10,
	KEY_B_COLOR,
	KEY_L_COLOR,
	KEY_R_COLOR,
	KEY_SELECT_COLOR,
	KEY_START_COLOR,
	KEY_RIGHT_COLOR,
	KEY_LEFT_COLOR,
	KEY_UP_COLOR,
	KEY_DOWN_COLOR,
	BLACK_COLOR,
	GBA_COLOR,
	SCREEN_COLOR,
	SCREEN_FRAME_COLOR
};

void main(void)
{
	irqInit();
	irqEnable(IRQ_VBLANK);		

	SetMode(MODE_4| BG2_ON);	

	DrawFrame(0,0,240,160, BLACK_COLOR); // 用于测试屏幕对准的边框
        DrawRect(38,47,146,83, GBA_COLOR);  // GBA的整体背景颜色

	DrawRect(59,89,62,42, SCREEN_FRAME_COLOR); // 屏幕边框颜色 
	DrawRect(60,90,60,40, SCREEN_COLOR); // 整块屏幕颜色 
	// LR键
	DrawRect(39,48,30,10, KEY_L_COLOR); // L button
	DrawRect(39,162,30,10, KEY_R_COLOR); // R button
	// AB键
	DrawCicle(70, 177, 5, KEY_A_COLOR); //A button
	DrawCicle(80, 162, 5, KEY_B_COLOR); //B button
	// 方向键
	DrawCicle(100, 78, 2, KEY_START_COLOR); //start button
	DrawCicle(110, 78, 2, KEY_SELECT_COLOR); //select button
	DrawRect(76, 54, 8, 8, KEY_LEFT_COLOR); // left button 
	DrawRect(68, 62, 8, 8, KEY_UP_COLOR); // up button 
	DrawRect(76, 70, 8, 8, KEY_RIGHT_COLOR); // right button 
	DrawRect(84, 62, 8, 8, KEY_DOWN_COLOR); // down button 

	SetPalette(0, WHITE);
	SetPalette(BLACK_COLOR, BLACK);
	SetPalette(GBA_COLOR, ORANGE);
	SetPalette(SCREEN_FRAME_COLOR, BLACK);
	SetPalette(SCREEN_COLOR, WHITE);
	SetPalette(KEY_A_COLOR, WHITE);
	SetPalette(KEY_B_COLOR, WHITE);
	SetPalette(KEY_L_COLOR, WHITE);
	SetPalette(KEY_R_COLOR, WHITE);
	SetPalette(KEY_START_COLOR, WHITE);
	SetPalette(KEY_SELECT_COLOR, WHITE);
	SetPalette(KEY_LEFT_COLOR, WHITE);
	SetPalette(KEY_RIGHT_COLOR, WHITE);
	SetPalette(KEY_UP_COLOR, WHITE);
	SetPalette(KEY_DOWN_COLOR, WHITE);

	while (1)
	{	
		VBlankIntrWait();
		scanKeys();

		int keys_pressed = keysHeld();

		if (keys_pressed & KEY_L)
		{
			SetPalette(KEY_L_COLOR, RED); 
		}else {
			SetPalette(KEY_L_COLOR, WHITE);
		}

		if (keys_pressed & KEY_R)
		{
			SetPalette(KEY_R_COLOR, RED);
		}else {
			SetPalette(KEY_R_COLOR, WHITE);
		}

		if (keys_pressed & KEY_A)
		{
			SetPalette(KEY_A_COLOR, RED);
		}else {
			SetPalette(KEY_A_COLOR, WHITE);
		}

		if (keys_pressed & KEY_B)
		{
			SetPalette(KEY_B_COLOR, RED);
		}else {
			SetPalette(KEY_B_COLOR, WHITE);
		}

		if (keys_pressed & KEY_START)
		{
			SetPalette(KEY_START_COLOR, RED);
		}else {
			SetPalette(KEY_START_COLOR, WHITE);
		}

		if (keys_pressed & KEY_SELECT)
		{
			SetPalette(KEY_SELECT_COLOR, RED);
		}else {
			SetPalette(KEY_SELECT_COLOR, WHITE);
		}

		if (keys_pressed & KEY_UP)
		{
			SetPalette(KEY_UP_COLOR, RED);
		}else {
			SetPalette(KEY_UP_COLOR, WHITE);
		}

		if (keys_pressed & KEY_DOWN)
		{
			SetPalette(KEY_DOWN_COLOR, RED);
		}else {
			SetPalette(KEY_DOWN_COLOR, WHITE);
		}
		if (keys_pressed & KEY_LEFT)
		{
			SetPalette(KEY_LEFT_COLOR, RED);
		}else {
			SetPalette(KEY_LEFT_COLOR, WHITE);
		}

		if (keys_pressed & KEY_RIGHT)
		{
			SetPalette(KEY_RIGHT_COLOR, RED);
		}else {
			SetPalette(KEY_RIGHT_COLOR, WHITE);
		}


	}
}
