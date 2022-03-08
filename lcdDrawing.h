#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include "asciiLib.h"

/////////////////////////////   DRAWING SECTION    /////////////////////////////////////////////////////////////////////


// function - make lcd white
void whitening(){
	lcdSetCursor(0, 0);
	lcdWriteIndex(DATA_RAM);
	for (int i = 0; i < LCD_MAX_X * LCD_MAX_Y; i++)
			lcdWriteData(LCDWhite);
}

// function - fill area with color
void fill(int pos_x, int size_x, int pos_y, int size_y, int color){
	lcdWriteReg(HADRPOS_RAM_START, pos_x);
	lcdWriteReg(HADRPOS_RAM_END, pos_x + size_x);
	
	lcdWriteReg(VADRPOS_RAM_START, pos_y);
	lcdWriteReg(VADRPOS_RAM_END, pos_y + size_y);
	
	lcdSetCursor(pos_x, pos_y);
	lcdWriteIndex(DATA_RAM);
	for (int i = 0; i < (size_x+1) * size_y; i++){
			lcdWriteData(color);
	}
}

// function drawing line - Bresenham's line algorithm
void draw_line(const int x1, const int y1, const int x2, const int y2) {
     int d, dx, dy, ai, bi, xi, yi;
     int x = x1, y = y1;
     
     if (x1 < x2){
         xi = 1;
         dx = x2 - x1;
     }
     else{
         xi = -1;
         dx = x1 - x2;
     }
     
     if (y1 < y2){
         yi = 1;
         dy = y2 - y1;
     }
     else{
         yi = -1;
         dy = y1 - y2;
     }

     lcdSetCursor(x, y);
	 lcdWriteReg(DATA_RAM,LCDBlack);
     
     if (dx > dy){
         ai = (dy - dx) * 2;
         bi = dy * 2;
         d = bi - dx;
         
         while (x != x2){
             
             if (d >= 0){
                 x += xi;
                 y += yi;
                 d += ai;
             }
             else{
                 d += bi;
                 x += xi;
             }
            lcdSetCursor(x, y);
			lcdWriteReg(DATA_RAM,LCDBlack);
         }
     }

     else {
         ai = ( dx - dy ) * 2;
         bi = dx * 2;
         d = bi - dy;

         while (y != y2){

             if (d >= 0){
                 x += xi;
                 y += yi;
                 d += ai;
             }
             else{
                 d += bi;
                 y += yi;
             }
            lcdSetCursor(x, y);
			lcdWriteReg(DATA_RAM,LCDBlack);
         }
     }
 }

// function drawing rectangle
void draw_rectangle(const int x1,const int y1,const int h,const int w){
	 draw_line(x1,y1,x1+h,y1);
	 draw_line(x1+h,y1,x1+h,y1+w);
	 draw_line(x1+h,y1+w,x1,y1+w);
	 draw_line(x1,y1+w,x1,y1);
}

// draw letter on lcd
void draw_letter(int x,int y,char c){
	 unsigned char tab[16];
	 GetASCIICode(1,tab,c);
	 for(int i=0;i<16;i++){
		 for(int j=0;j<8;j++){
			 lcdSetCursor(x-i,y-j);
			 if((tab[i]>>j&1)==1){
				lcdWriteReg(DATA_RAM,LCDBlack);
			 }	 
		 }
	 }
}

// draw string on lcd 
void draw_string(int x,int y,char* s){
	 int i=0;
	 while(s[i]!='\0') {
		 draw_letter(x,y,s[i++]);
		 y+=8;
	 }
 }

// func drawing piano keyboard and volume buttons
void draw_piano(){
	
	whitening();

	for(int i=0; i<LCD_MAX_Y; i+=42)
		draw_rectangle(0, i, LCD_MAX_X, 42);
	for(int i=28; i<LCD_MAX_Y; i+=42){
		if(i!=112 && i!=280){
			draw_rectangle(LCD_MAX_X/2-20, i, LCD_MAX_X/2+20, 28);
			fill(LCD_MAX_X/2-20,LCD_MAX_X/2+20, i ,28,LCDBlack );
		}
	}
	fill(0,LCD_MAX_X/2,294,42,LCDMagenta);
	fill(LCD_MAX_X/2,LCD_MAX_X/2,294,42,LCDYellow);
	draw_string(LCD_MAX_X/4,310,"+");
	draw_string(LCD_MAX_X/4*3,310,"-");
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

