

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
   initWiringPi();
   connectBme();
   float TE = getCurrentTemperature();
   printf("\t TE: %.2f⁰C\n", TE);
}