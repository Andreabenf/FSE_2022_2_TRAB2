

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "pid.h"
#include "bme280.h"
#include "sensortemp.h"
#include "gpio.h"

int uart_filestream, key_gpio = 1;

int main()
{
//    printf("sim");
//    connectBme();
//    float TE = getCurrentTemperature();
//    printf("\t TE: %.2f⁰C\n", TE);

    openUart("/dev/serial0");

     float TR = LeTempRef();
     float TI = leTempInterna();
     

   printf("\t Tr (fora): %f⁰C\n", (TR));
   printf("\t Ti (fora): %f⁰C\n", (TI));

   while (1)
   {
    int resp=  LeComandos();
    printf("Comando (fora) %d\n", resp);
   }
   


}