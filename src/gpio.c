#include "gpio.h"

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>


void initWiringPi(){
   wiringPiSetup();
   pinMode(VENTO_PORT, OUTPUT);
   pinMode(RESIST_PORT, OUTPUT);
   softPwmCreate(VENTO_PORT,1,100);
   softPwmCreate(RESIST_PORT,1,100);

   return;
}


void gelaForno(int valor){
   softPwmWrite (VENTO_PORT, valor);
   return;
}

void esquentaForno(int valor){
   softPwmWrite (RESIST_PORT, valor);
   return;
}