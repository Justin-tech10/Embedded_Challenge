#include <Arduino.h>
//Frist Question
/*void adc_init(void){
  ADMUX = (1<<REFS0); //sets the voltage reference to Vcc
  ADSRA = (1<<ADEN) | 7;//Enables ADC, set prescaler to 128
}
int readAdc(char chan){
  ADMUX = (1<<REFS0) | (chan & 0x0f);//Selecting ADC channel with the Vcc as reference
  ADSRA |= (1<<ADSC);//Starts the ADC conversion 
  while (ADSRA & (1<<ADSC));//Waits until the ADC conversion is complete
  return ADCW;//Returns the result of the ADC conversion.
}
//Questionn #2
*/
//Rewrite the above code using pointers, bit assignments instead of macros
/* volatile unit8_t* ADMUX_REG =(volatile unit8_t*)0x7C;
 volatile unit8_t* ADCSRA_REG =(volatile unit8_t*)0x7A;
  volatile unit16_t* DATA_ADC =(volatile unit16_t*)0x78;
void adc_init(void){
  ADMUX_REG = (1<<6); //sets the voltage reference to Vcc
  ADCSRA_REG = (1<<7) | 7;//Enables ADC, set prescaler to 128
}
int readAdc(char chan){
  ADMUX_REG = (1<<6) | (chan & 0x0f);//Selecting ADC channel with the Vcc as reference
  ADSRA |= (1<<6);//Starts the ADC conversion 
  while (*ADCSRA_REG & (1<<6));//Waits until the ADC conversion is complete
  return DATA_ADC;//Returns the result of the ADC conversion.
}
*/
//Question #3

void setup() {
  // put your setup code here, to run once:
  ADMUX = 0b01000111;
  //        01 = Vcc is the top measurement
  //          0 = Right Adjust for now
  //           00000 - ADC 7 , GND as ref
  ADCSRA = 0b10100110;
  //         101 = Enable, Dont start yet, Auto trigger
  //            00 = Interrupt stuff
  //              011 = 64 Clock Prescale
  ADCSRB = 0b00000000;
  //         00000000 = No high Speed, Free running
  ADCSRA |= (1<<ADSC);
  Serial.begin(9600);
PORTC = (1<<7);
}
          
void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
  Serial.print(">temp:");
  Serial.println(ADCW>>6);
  int adc_val= ADCW; 
    float voltage=(adc_val *3.3)/1023;
    if(voltage>1.01){
      PORTC |=(1<<7);
    }else if(voltage<.99){
      PORTC &= ~(1<<7);
    }
    Serial.println(voltage);
    delay(500);

}

