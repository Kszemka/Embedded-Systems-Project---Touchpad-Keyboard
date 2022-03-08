#include "math.h"
#include "GPIO_LPC17xx.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "TP_Open1768.h"

#include "lcdDrawing.h"

//// global variables ////////////////////////////////////////////////////////////////////////////////////

#define SAMPLE_SIZE 10 // averaging touch coordinates

// flague for touch event and x,y touch coordinates
volatile bool touched = false; 
volatile int last_touch_x = 0;
volatile int last_touch_y = 0;

// linear calibration 
double delta_x = 1.0;
double delta_y = 1.0;
int b_x = 0;
int b_y = 0;

// wave sent to DAC
const uint8_t SoundWave[16] = {4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3};
//const uint8_t SoundWave[36] = {4,5,6,7,7,7,6,5,4,3,2,2,2,3,4,5,5,5,4,3,3,3,4,5,6,6,6,5,4,3,2,1,1,1,2,3};
int Sound_index = 0;
int wave_size = 16;

// default volume
int volume = 50;

// sound frequency: default freq is 262Hz - C4
int freq = 262;

// array with coordinates of last key played
int last_key[5];


//////////////////////////////////////////////////////////////////////////////////////////////////////////



//// functions //////////////////////////////////////////////////////////////////////////////////////////

// func sending string with UART
void send_string(char str[]){

	int i = 0;	
	while(str[i] != '\0'){
		if ((LPC_UART0->LSR & (1 << 5)) != 0){
			LPC_UART0->THR = str[i];
			i++;
		}
	}
}

// func awaiting for interruption from touchpad
void wait_for_touch(int *x, int *y){

	while(1){
		if (touched){
			*x = last_touch_x;
			*y = last_touch_y;
			touched = false;
			break;
		}
	}
}

// func gets coordinate of interruption
void read_touch(int *x, int *y){

	int sample_x, sample_y;
	*x = 0;
	*y = 0;
	for (int i = 0; i < SAMPLE_SIZE; i++){
		touchpanelGetXY(&sample_x, &sample_y);
		*x += sample_x;
		*y += sample_y;
	}
	*x /= SAMPLE_SIZE;
	*y /= SAMPLE_SIZE;
}

// remembering coordinates of last played key globally
void fill_key(int x1,int x2,int x3,int x4,int color){

	last_key[0] = x1;
	last_key[1] = x2;
	last_key[2] = x3;
	last_key[3] = x4;
	last_key[4] = color;
}

// func identyfing pushed key
void print_what_sound(){
	
	if(last_touch_x>294){

		if(last_touch_y<LCD_MAX_X/2 && volume<100)
			volume += 10;

		else if(last_touch_y>LCD_MAX_X/2 && volume>10)
			volume -= 10;
	}

	//onlywhite keys options
	if(last_touch_y<100){

		if(last_touch_x<42){
			//send_string("C"); 
			fill(15,70,6,30,LCDGreen);
			fill_key(15,70,6,30,LCDWhite);	
		}

		else if(last_touch_x<84 && last_touch_x>=42){
			//send_string("D");404373
			freq*=pow(pow(2,1.0/12),2);
			fill(15,70,48,30,LCDGreen);
			fill_key(15,70,48,30,LCDWhite);	
		}

		else if(last_touch_x<126 && last_touch_x>=84){
			//send_string("E");
			freq*=pow(pow(2,1.0/12),4);
			fill(15,70,90,30,LCDGreen);
			fill_key(15,70,90,30,LCDWhite);
		}

		else if(last_touch_x<168 && last_touch_x>=126){
			//send_string("F");
			freq*=pow(pow(2,1.0/12),5);
			fill(15,70,132,30,LCDGreen);
			fill_key(15,70,132,30,LCDWhite);
		}

		else if(last_touch_x<210 && last_touch_x>=168){
			//send_string("G");
			freq*=pow(pow(2,1.0/12),7);
			fill(15,70,174,30,LCDGreen);
			fill_key(15,70,174,30,LCDWhite);
		}

		else if(last_touch_x<252 && last_touch_x>=210){
			//send_string("A");
			freq*=pow(pow(2,1.0/12),9);
			fill(15,70,216,30,LCDGreen);
			fill_key(15,70,216,30,LCDWhite);
		}

		else if(last_touch_x<294 && last_touch_x>=252){
			//send_string("H");
			freq*=pow(pow(2,1.0/12),11);
			fill(15,70,258,30,LCDGreen);
			fill_key(15,70,258,30,LCDWhite);
		}
	}

	//white and black keys options
	if(last_touch_y>100){

		if(last_touch_x<28){
			//send_string("C");
			fill(15,70,6,30,LCDGreen);
			fill_key(15,70,6,30,LCDWhite);
		}

		else if(last_touch_x<56 && last_touch_x>=28){
			//send_string("CIS");
			freq*=pow(pow(2,1.0/12),1);
			fill(120,50,35,15,LCDGreen);
			fill_key(120,50,35,15,LCDBlack);
			
		}

		else if(last_touch_x<70 && last_touch_x>=56){
			//send_string("D");
			freq*=pow(pow(2,1.0/12),2);
			fill(15,70,48,30,LCDGreen);
			fill_key(15,70,48,30,LCDWhite);
		}

		else if(last_touch_x<98 && last_touch_x>=70){
			//send_string("DIS");
			freq*=pow(pow(2,1.0/12),3);
			fill(120,50,77,15,LCDGreen);
			fill_key(120,50,77,15,LCDBlack);
		}

		else if(last_touch_x<126 && last_touch_x>=98){
			//send_string("E");
			freq*=pow(pow(2,1.0/12),4);
			fill(15,70,90,30,LCDGreen);
			fill_key(15,70,90,30,LCDWhite);
		}

		else if(last_touch_x<154 && last_touch_x>=126){
			//send_string("F");
			freq*=pow(pow(2,1.0/12),5);
			fill(15,70,132,30,LCDGreen);
			fill_key(15,70,132,30,LCDWhite);
		}

		else if(last_touch_x<182 && last_touch_x>=154){
			//send_string("FIS");
			freq*=pow(pow(2,1.0/12),6);
			fill(120,50,161,15,LCDGreen);
			fill_key(120,50,161,15,LCDBlack);
		}

		else if(last_touch_x<196 && last_touch_x>=182){
			//send_string("G");
			freq*=pow(pow(2,1.0/12),7);
			fill(15,70,174,30,LCDGreen);
			fill_key(15,70,174,30,LCDWhite);
		}

		else if(last_touch_x<224 && last_touch_x>=196){
			//send_string("GIS");
			freq*=pow(pow(2,1.0/12),8);
			fill(120,50,203,15,LCDGreen);
			fill_key(120,50,203,15,LCDBlack);
		}

		else if(last_touch_x<238 && last_touch_x>=224){
			//send_string("A");
			freq*=pow(pow(2,1.0/12),9);
			fill(15,70,216,30,LCDGreen);
			fill_key(15,70,216,30,LCDWhite);
		}

		else if(last_touch_x<266 && last_touch_x>=238){
			//send_string("AIS");
			freq*=pow(pow(2,1.0/12),10);
			fill(120,50,245,15,LCDGreen);
			fill_key(120,50,245,15,LCDBlack);
				
		}

		else if(last_touch_x<294 && last_touch_x>=266){
			//send_string("H");
			freq*=pow(pow(2,1.0/12),11);
			fill(15,70,258,30,LCDGreen);
			fill_key(15,70,258,30,LCDWhite);
		}
	}	
}

// TIM0 config
void config_Timer(){

	LPC_SC->PCONP |= 1 << 1;     // Power up Timer 0
	LPC_SC->PCLKSEL0 |= 1 << 2;

	LPC_TIM0->PR = 0;
	LPC_TIM0->MR0 = SystemCoreClock/4/(freq*wave_size);
	LPC_TIM0->MCR = 3;
	LPC_TIM0->TCR = 1;
	NVIC_DisableIRQ(TIMER0_IRQn);
}

// interruption TIM0 handler
void TIMER0_IRQHandler(void){
		
	LPC_DAC->DACR = SoundWave[Sound_index]*volume<<6;	
	Sound_index++;
	if(Sound_index>=wave_size)
			Sound_index = 0;	
	LPC_TIM0->IR = 1;
}

// interruption handler
void EINT3_IRQHandler(void){

	int x, y;
	read_touch(&x, &y);

		if (!(x > 4000 || y < 100)){
		
			last_touch_x = delta_x * x +b_x;
			last_touch_y = delta_y * y +b_y;
			
			touched = true;
			
			print_what_sound();

			LPC_TIM0->TCR =2;
			LPC_TIM0->TCR=1;
			LPC_TIM0->MR0 = SystemCoreClock/4/(freq*wave_size);
			NVIC_EnableIRQ(TIMER0_IRQn);	
		}

		else{

			NVIC_DisableIRQ(TIMER0_IRQn);
			LPC_TIM0->TCR=0;
			fill(last_key[0],last_key[1],last_key[2],last_key[3],last_key[4]);
		}

	LPC_GPIOINT->IO0IntClr |= (1 << 19);
}

// calibrating touchpad - lcd
void calibrate(){
	
	int x1, x2, y1, y2, x1_t, x2_t, y1_t, y2_t;
	
	whitening();
	draw_string(LCD_MAX_X/2, 80, "Konfiguruj: kliknij w X-y");
	
	y1 = LCD_MAX_X-40;
	x1 = LCD_MAX_Y-40;
	draw_letter(LCD_MAX_X-40, LCD_MAX_Y-40, 'X');
	wait_for_touch(&x1_t, &y1_t);
	
	x2 = 40;
	y2 = 40;
	draw_letter(40, 40, 'X');
	wait_for_touch(&x2_t, &y2_t);
	
	delta_x = ((double)(x1-x2))/(x1_t-x2_t);
	delta_y = ((double)(y1-y2))/(y1_t-y2_t);
	
	b_x = x1-delta_x*x1_t;
	b_y = y1-delta_y*y1_t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
	
	//LCD config
	lcdConfiguration();
	init_ILI9325();
	touchpanelInit();

	//UART0 CONFIG
	PIN_Configure(0,2,1,2,0);
	PIN_Configure(0,3,1,2,0);
	LPC_UART0->LCR=(1<<7)|(3<<0);
	LPC_UART0->DLL=13;
	LPC_UART0->DLM=0;
	LPC_UART0->FDR=1|(15<<4);
	LPC_UART0->LCR=3;

	//DAC
	PIN_Configure(0,26,2,2,0);
	LPC_SC->PCLKSEL0 |= 1 << 22;
	
	//EINT3
	LPC_GPIOINT->IO0IntEnR = (1 << 19);
	LPC_GPIOINT->IO0IntEnF = (1 << 19);
	NVIC_EnableIRQ(EINT3_IRQn);
	
	//TIMER
	config_Timer();


	calibrate();
	draw_piano();
	
	while(1){			
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////