



#include "pid.h"
#include "bme280.h"
#include "sensortemp.h"
#include "gpio.h"
#include "uart.h"

int uart_filestream, key_gpio = 1;

int main()
{
  printf("sim");
  connectBme();
  float TE = getCurrentTemperature();
  printf("\t TE: %.2f⁰C\n", TE);

  openUart("/dev/serial0");

  float TR = 0;
  TR = LeTempRef();
  float TI = leTempInterna();

  printf("\n Tr (fora): %f⁰C\n", (TR));
  printf(" Ti (fora): %f⁰C\n\n", (TI));

  while (1)
  {
    int resp = LeComandos();
    printf("Comando (fora) %d\n", resp);
  }
}