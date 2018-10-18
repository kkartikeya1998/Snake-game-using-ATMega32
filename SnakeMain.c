/*
 * SnakeMain.c
 *
 * Created: 2/2/2018 4:07:53 PM
 *  Author: Kumar Kartikeya
 */

#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include "LCD_4bit.h"
#include <string.h>`
#include <avr/interrupt.h>

#define rButton PIND3           //Right turn button at pin 3
#define lButton PIND2           //Left turn button at 2
int foodx	, foody;    		// Food coordinates
int headx, heady;	        	// Head coordinates
int length = 3;
unsigned int size;      		// Length of snake

uint8_t arr[8] = {		        //Snake state matrix
	0b11111000,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111

	};

 uint8_t foodArr[8] ={          //Food state matrix
	 0b11111111,
	 0b11111111,
	 0b11111111,
	 0b11111111,
	 0b11111111,
	 0b11111111,
	 0b11111111,
	 0b11111111
 } ;



typedef enum {		    	//Creating direction as an enum
	right,			    	//	0 for right
	down,			    	//	1 for down
	left,			    	//	2 for left
	up				    	//	3 for up
	}direction;


typedef enum{			    //Creating boolean datatype
	false,
	true
	}bool;

	bool buttonFlag =  true;


typedef struct{		    	//Creating a struct for a single segment of snake
	int x;
	int y;  				//Coordinates
	struct segment *c;  	//Pointer
	}segment;


direction facing = up;

segment snake[64] ={	    // Initializing array of snake segments with initially 3 segments
			0,0, &(snake[1]),
			0,1, &(snake[2]),
			0,2, NULL
			};
headx = 0;
heady = 2;

void gameOver(){            //Turn ON all LEDs when game over
	int j = 0;
	for(j = 0 ; j < 8; j++){
		arr[j] = 0b00000000;
		foodArr[j] = 0b00000000;
		LCD_Disp("Game Over.");

	}
}


void moveSnake(void){
	int newx = headx;
	int newy = heady;

	buttonFlag = 1;
	switch(facing){				    // setting coordinates of next head position
		case 0:
			newx += 1;
			break;

		case 1:
			newy -= 1;
			break;

		case 2:
			newx -= 1;
			break;

		case 3:
			newy += 1;
			break;
	}

	segment newHeadPosition = {newx, newy, NULL};					// making the new head
	arr[newx] &= ~(1<<newy);
	segment oldHead = {.x = headx, .y = heady, &(newHeadPosition)};	// Putting old head as new second segment
	arr[snake[0].x] |= (1 << snake[0].y);
	snake[length-1] = oldHead;

	for(int i = 0;i < length-1; i++){
		snake[i] = snake[i+1];
	}
	snake[length-1] = newHeadPosition;


	headx = newx;
	heady = newy;

	if(headx > 7 || headx < 0 || heady > 7 || heady < 0){
		gameOver();
	}
	int k = 0;
	for(k = 0; k < length-1; k++){
		if((headx == snake[k].x) && (heady == snake[k].y)){
			gameOver();
		}
	}
}


void displayFood(){

	PORTB |= (1 << foodx);
	PORTA = (foodArr[foodx]);

	_delay_us(60);

	PORTA = 0xff;
	PORTB &= ~(1 << foodx);


}


void generateFood(){
	foodArr[foodx] |= (1 << foody);
	int temp1, temp2;
	temp1 = (rand())%8;
	foodx = temp1;

	temp2 = (rand()%8);
	foody = temp2;

	foodArr[foodx] &= ~( 1<<foody );


}

void addSegment(){
	int newx = headx;
	int newy = heady;

	buttonFlag = 1;
	switch(facing){				// setting coordinates of next head position
		case 0:
			newx += 1;
			break;

		case 1:
			newy -= 1;
			break;

	case 2:
		newx -= 1;
		break;

	case 3:
		newy += 1;
		break;
}


	segment newHead = {newx, newy,  NULL};

	segment oldHead = {.x = headx, .y = heady, &(newHead)};
	snake[length-1] = oldHead;
	snake[length] = newHead;
	headx = newx;
	heady = newy;
	arr[newx] &= ~(1<<newy);
	arr[snake[0].x] |= (1 << snake[0].y);


	length++;

	LCD_setCursor(9,1);
	LCD_printScore((length-3));

}


LCD_setup(){
	LCD_Init();
	LCD_setCursor(0, 0);
	LCD_Disp("Snake hsss~");
	LCD_setCursor(1,1);
	LCD_Disp("Score: ");
	LCD_setCursor(11,1);
	LCD_Disp("     ");

}

 ISR(INT0_vect){
	 if(facing == up){
		 facing = right;
	}else{
		 facing++;			// Change direction clockwise
	}
	 _delay_ms(50);
 }

 ISR(INT1_vect){
	 if(facing == right){
		 facing = up;
	 }else{
		 facing--;			// Change direction anticlockwise
	 }
	 _delay_ms(50);
 }


int main(void)
{
	DDRA = 0b11111111;		//Ground pins set to OUTPUT
	DDRB = 0b11111111;		//Red pins set to OUTPUT
	DDRC = 0b11111111;		//Green pins set to OUTPUT
	DDRD = 0b11001111;		//Set first 6 pins for LCD and last two for buttons

	PORTD |= (1 << rButton);
	PORTD |= (1 << lButton);
	PORTA = 0b11111111;
	PORTB = 0b00000000;



	GICR |= 1<<INT0;
	GICR |= 1<<INT1;
	LCD_setup();
	LCD_setCursor(9,1);
	LCD_printScore((length-3));

	generateFood();

	int count = 0;

	sei();
	while(1)
	{


		if(count >= 600){
			moveSnake();
			if(foodx == headx && foody == heady){
				addSegment();
				generateFood();
			}
			count = 0;

		}
		count++;
		for(int i = 0; i < 8; i++){		//Printing Matrix on 8*8 LED Display

			PORTC |= (1 << i);
			PORTA = (arr[i]);

			_delay_us(60);

			PORTA = 0xff;
			PORTC &= ~(1 << i);

		}
		displayFood();
	}
}
