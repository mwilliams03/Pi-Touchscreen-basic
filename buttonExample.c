/*
	A simple program that demonstrates how to program for a
	touch screen. Specifically on the Raspberry Pi.
	This program can be used for most Linux based systems.
	For more details: www.marks-space.com

    Copyright (C) 2013  Mark Williams

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA
*/

#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "touch.h"
#include "touch.c"
#include "framebuffer.c"
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include "wiringPi.h"

#define BUTTON_ON 1
#define BUTTON_OFF 0


#define LED_RED 1
#define LED_GREEN 6
#define LED_BLUE 4



#define X 0
#define Y 1
#define W 2
#define H 3

void createButton(int x, int y, int w, int h, char *text, int backgroundColor, int foregroundColor)

{
	int size = sizeof(text);
	char *p = text;
	int length = 0;

	//get length of the string *text
	while(*(p+length))
      		length++;

	//
	if((length*8)> (w-2)){
		printf("####error,button too small for text####\n");
		exit(1);
	}

	//Draw button outline
	drawSquare(x-2,y-2,w+4,h+4,backgroundColor);

	//Draw button foreground 
        drawSquare(x,y,w,h,foregroundColor);
	//Place text on the button.  Try and center it in a primitive way
	put_string(x+((w-(length*8))/2), y+((h-8)/2),text,4);
}

int  SetPinsOut()
{
        if (wiringPiSetup () == -1)
                exit (1) ;

        pinMode (LED_RED, OUTPUT) ;
        pinMode (LED_GREEN, OUTPUT) ;
        pinMode (LED_BLUE, OUTPUT) ;

	digitalWrite(LED_RED,LOW);
	digitalWrite(LED_GREEN,LOW);
	digitalWrite(LED_BLUE,LOW);

}
int mymillis()
{
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}


void  INThandler(int sig)
{
	digitalWrite(LED_RED,LOW);
	digitalWrite(LED_GREEN,LOW);
	digitalWrite(LED_BLUE,LOW);

        signal(sig, SIG_IGN);
	closeFramebuffer();
        exit(0);
}

int main()
{
	signal(SIGINT, INThandler);

	int  xres,yres,x;


	int screenXmax, screenXmin;
	int screenYmax, screenYmin;

	float scaleXvalue, scaleYvalue;

	int rawX, rawY, rawPressure, scaledX, scaledY;



	//Used to monitor the timers for each button

	int buttonTimer2 = mymillis();
	int buttonTimer3 = mymillis();
	int buttonTimer4 = mymillis();

	//To keep track of the button states
	int		button2= BUTTON_OFF;
	int		button3= BUTTON_OFF;
	int		button4= BUTTON_OFF;

	//Our buttons.  X, Y, Width, Height
	int buttonCords2[4] = {45,75,60,80};
	int buttonCords3[4] = {135,75,60,80};
	int buttonCords4[4] = {225,75,60,80};



	if (openTouchScreen() == 1)
		perror("error opening touch screen");


	getTouchScreenDetails(&screenXmin,&screenXmax,&screenYmin,&screenYmax);

	framebufferInitialize(&xres,&yres);

	scaleXvalue = ((float)screenXmax-screenXmin) / xres;
	printf ("X Scale Factor = %f\n", scaleXvalue);
	scaleYvalue = ((float)screenYmax-screenYmin) / yres;
	printf ("Y Scale Factor = %f\n", scaleYvalue);



	createButton(buttonCords2[X],buttonCords2[Y],buttonCords2[W],buttonCords2[H],"RED",GREY,RED);
	createButton(buttonCords3[X],buttonCords3[Y],buttonCords3[W],buttonCords3[H],"GREEN",GREY,GREEN);
	createButton(buttonCords4[X],buttonCords4[Y],buttonCords4[W],buttonCords4[H],"BLUE",GREY,BLUE);

	//Setup pins
	SetPinsOut();



	while(1){
                getTouchSample(&rawX, &rawY, &rawPressure);

                scaledX = rawX/scaleXvalue;
                scaledY = rawY/scaleYvalue;




		//See if the results retuned by the touch event fall within the coordinates of the button
                if((scaledX  > buttonCords2[X] && scaledX < (buttonCords2[X]+buttonCords2[W])) && (scaledY > buttonCords2[Y] && scaledY < (buttonCords2[Y]+buttonCords2[H])))
			//Has 500ms passed since the last time this button was pressed?
			if (mymillis() - buttonTimer2 > 500)
				//Is the button currently on? If so, we need to turn it off.
                                if(button2){
					//Change color to a darker red
                                        createButton(buttonCords2[X],buttonCords2[Y],buttonCords2[W],buttonCords2[H],"RED",GREY,RED);
					//Change state to off
                                        button2= BUTTON_OFF;
					//Start the timer
                                        buttonTimer2 = mymillis();
                                        //Turn the LED off
					digitalWrite(LED_RED,LOW);

                                }
                                else{
					//The button must be off.. so we need to turn it on.
                                        createButton(buttonCords2[X],buttonCords2[Y],buttonCords2[W],buttonCords2[H],"RED",WHITE,LIGHT_RED);
					//Change state to on
                                        button2= BUTTON_ON;
					//Start the timer
                                        buttonTimer2 = mymillis();
					//Turn the LED on
                                        digitalWrite(LED_RED,HIGH);
                                }

                if((scaledX  > buttonCords3[X] && scaledX < (buttonCords3[X]+buttonCords3[W])) && (scaledY > buttonCords3[Y] && scaledY < (buttonCords3[Y]+buttonCords3[H])))
                      if (mymillis() - buttonTimer3 > 500)
                                if(button3){
                                        createButton(buttonCords3[X],buttonCords3[Y],buttonCords3[W],buttonCords3[H],"GREEN",GREY,GREEN);
                                        button3= BUTTON_OFF;
                                        buttonTimer3 = mymillis();
                                        digitalWrite(LED_GREEN,LOW);
                                }
                                else{
                                        createButton(buttonCords3[X],buttonCords3[Y],buttonCords3[W],buttonCords3[H],"GREEN",WHITE,LIGHT_GREEN);

                                        button3= BUTTON_ON;
                                        buttonTimer3 = mymillis();
                                        digitalWrite(LED_GREEN,HIGH);
                                }

                if((scaledX  > buttonCords4[X] && scaledX < (buttonCords4[X]+buttonCords4[W])) && (scaledY > buttonCords4[Y] && scaledY < (buttonCords4[Y]+buttonCords4[H])))
                      if (mymillis() - buttonTimer4 > 500)
                                if(button4){
                                        createButton(buttonCords4[X],buttonCords4[Y],buttonCords4[W],buttonCords4[H],"BLUE",GREY,BLUE);
                                        button4= BUTTON_OFF;
                                        buttonTimer4 = mymillis();
                                        digitalWrite(LED_BLUE,LOW);
                                }
                                else{
                                        createButton(buttonCords4[X],buttonCords4[Y],buttonCords4[W],buttonCords4[H],"BLUE",WHITE,LIGHT_BLUE);
                                        button4= BUTTON_ON;
                                        buttonTimer4 = mymillis();
                                        digitalWrite(LED_BLUE,HIGH);
                                }

	}
}





