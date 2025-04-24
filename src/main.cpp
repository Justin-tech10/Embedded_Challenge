#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

//Objectives
//create a PWM signal with timer1 16bits
//PWM frequency of 1khz 
//need ticks is 8000
#define SET_DUTY_CYCLE 50
void setup() {
  //Normal port operation
  //Clear then set at top
  //don't cares for the C's
  //Fast PWM  
 TCCR1A=0b00100011;
 //don't cares for 000
 //turn on waveform generation mode 1 
 //clock prescaler for 256
 TCCR1B=0b00011100;
 //Top value
 OCR1A= 8000;
 //bottom value
 OCR1B=OCR1A*(SET_DUTY_CYCLE/100);//50% duty cycle 
 //setting port 10 on the board
 DDRB |=(1<<6);
 
}
uint8_t count=1; // if 1 count up if 0 count down

void loop() {

    if(count==1){OCR1B++;}
    if(count==0){OCR1B--;}
    if(OCR1B==OCR1A){count=0;}
    if(OCR1B==0){count=1;}
    delay(1000);
}
