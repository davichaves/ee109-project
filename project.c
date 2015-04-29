/********************************************
*
*  Name: Davi Rodrigues Chaves
*  Section: W 3:30-5:00 PM
*  Assignment: Final Project
*
********************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

void init_ports(void);
void write_lcd(void);
void check_buttons(void);
void check_temp(void);
volatile char hasChanged;

int Low = 50;
int High = 70;
int temp = 60;

char upText[20];
char downText[20];
char button = 2; // 1 = Hot | 2 = Cold
char charButton = 'B';

int aState, bState, prevAState, prevBState;

ISR(PCINT1_vect) {
    prevAState = aState;
    prevBState = bState;
    if ((PINC & (1<<PC2)) == 0) { //A2 == 0
        aState = 0;
    } else {
        aState = 1;
    }
    if ((PINC & (1<<PC3)) == 0) { //A3 == 0
        bState = 0;
    } else {
        bState = 1;
    }
    if (aState != prevAState || bState != prevBState) { // if it changes do something
        hasChanged = 1;
        if (aState == 0 && bState == 0) {
            if (prevAState == 1) {
                if (button == 1) {
                    Low+=1;
                } else {
                    High+=1;
                }
            }
            if (prevBState == 1) {
                if (button == 1) {
                    Low-=1;
                } else {
                    High-=1;
                }
            }
        }
        if (aState == 0 && bState == 1) {
            if (prevAState == 1) {
                if (button == 1) {
                    Low-=1;
                } else {
                    High-=1;
                }
            }
            if (prevBState == 0) {
                if (button == 1) {
                    Low+=1;
                } else {
                    High+=1;
                }
            }
        }
        if (aState == 1 && bState == 1) {
            if (prevAState == 0) {
                if (button == 1) {
                    Low+=1;
                } else {
                    High+=1;
                }
            }
            if (prevBState == 0) {
                if (button == 1) {
                    Low-=1;
                } else {
                    High-=1;
                }
            }
        }
        if (aState == 1 && bState == 0) {
            if (prevAState == 0) {
                if (button == 1) {
                    Low-=1;
                } else {
                    High-=1;
                }
            }
            if (prevBState == 1) {
                if (button == 1) {
                    Low+=1;
                } else {
                    High+=1;
                }
            }
        }
    }
}

int main(void) {
    init_ports();
    init_lcd();
    sei(); // Enable interrupts
    writecommand(1); //clear LCD
    stringout("Eu sou um");
    moveto(16);
    stringout("termostato! :)");
    moveto(32);
    //write_lcd();
    /* Main programs goes here */
    while (1) {
        // check_buttons();
        // check_temp();
        // if (hasChanged == 1) {
        //     write_lcd();
        // }
    } // Loop forever
    return 0;   /* never reached */
}

/*
  check_temp - check the cold and hot temps and turn on the respective LED
*/

void check_temp(){
    if (temp > High){//turn on AC
        PORTB |= 0b00001000; //turn on green LED
    } else {//turn off AC
        PORTB &= ~(0b00001000);
    }
    if (temp < Low){//turn on heater
        PORTB |= 0b00010000; //turn on red LED
    } else {//turn off heater
        PORTB &= ~(0b00010000);
    }
}

/*
  check_buttons - check the cold and hot buttons
*/

void check_buttons(){
    if((PIND & 0b00000100) == 0){ //D2 = H
        button = 2;
        write_lcd();
    }
    if((PIND & 0b00001000) == 0){ //D3 = C
        button = 1;
        write_lcd();
    }
}

/*
  write_lcd - Write stuff in the LCD
*/

void write_lcd() {
    moveto(0);
    if (button == 1){
        charButton = 'C';
    } else {
        charButton = 'H';
    }
    snprintf(upText, 20, "Lo:%d | Hi:%d", Low,High);
    stringout(upText);
    moveto(16);
    snprintf(downText, 20, "Temp:%d | Sel:%c", temp, charButton);
    stringout(downText);
    hasChanged = 0;
    moveto(32);
}

/*
  init_ports - Do various things to initialize the ports
*/

void init_ports() {
    PCICR |= (1 << PCIE1);
    PCMSK1 |= ((1 << PCINT10)| (1 << PCINT11));
    DDRD |= 0xF0; //setting D4-D7 as outputs
    DDRB |= 0x1B; //seeting B1-B2 as outputs
    DDRD &= 0b11110011; //made an input //D2 and D3 -> buttons!
    PORTD |= 0b00001100; //set to zero  //D2 and D3 -> buttons!
    PORTC |= ((1 << PC2) | (1 << PC3)); //A2 and A3 -> Rotary!
}