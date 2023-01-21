

#include "pid.h"
#include "bme280.h"
#include "sensortemp.h"
#include "gpio.h"
#include "uart.h"
#include "pid.h"
#include "stdio.h"
#include <pthread.h>
#include <signal.h>

double referencia_ = 0.0;
double Kp_ = 30.0;  // Ganho Proporcional
double Ki_ = 0.2;   // Ganho Integral
double Kd_ = 400.0; // Ganho Derivativo
int modoTR = 0;
pthread_t menu;
pthread_t recebe;
pthread_t controla;

void init()
{
  connectBme();
  openUart("/dev/serial0");
  initWiringPi();
}
void menuTR()
{

  // system("clear");
  int i = -1;
  do
  {
    printf("Digite 0 para utilizar valor definido no dashboard via UART\n");
    printf("Digite 1 para definir a TR via teclado (DEBUG)\n");
    scanf("%d", &i);

  } while (i != 0 && i != 1);
  int man = i;
  ComunicaUartSendInt(COD_SEND, SET_CONTROL_MODE, man, 1);

  if (i == 1)
  {
    printf("Escreva o valor de: \nTR: ");
    scanf("%lf", &referencia_);
  }

  __fpurge(stdin);
}
void *menuFunc()
{
  int menuon = 1;
  int i;
  do
  {
    // system("clear");
    printf("Digite 1 para Decidir MODO TR \n");
    printf("Digite 2 para definir os valores das constantes Kp, Ki e Kd\n");

    scanf("%d", &i);
    if (i == 1)
    {
      menuTR();
    }
    if (i == 2)
    {
      printf("Escreva o valor de: \n");
      __fpurge(stdin);
      printf("Kp: ");
      scanf("%lf", &Kp_);
      printf("Ki_: ");

      scanf("%lf", &Ki_);
      printf("Kd_: ");

      scanf("%lf", &Kd_);
    }
    i = 0;

    __fpurge(stdin);
  } while (menuon);
}
void *recebeComandos()
{
  while (1)
  {
    int resp = LeComandos();

    switch (resp)
    {
    case LIGA_FORNO:
      printf("LIGA_FORNO!!!\n");
      break;
    case DESLIGA_FORNO:
      printf("DESLIGA_FORNO!!!\n");
      break;
    case INICIA_AQUEC:
      printf("INICIA_AQUEC!!!\n");
      break;
    case CANCELA:
      printf("CANCELA!!!\n");
      break;
    case MENU:
      printf("MENU!!!\n");
      break;
      
    }
    sleep(0.5);
  }
}
void *controlaTemp()
{
  while (1)
  {
    int ligado = 1;
    int funcionando = 1;

    float TE = getCurrentTemperature();
   // printf("\t TE: %.2f⁰C\n", TE);
    float TI = leTempInterna();
   // printf(" Ti (fora): %f⁰C\n\n", (TI));
    int control_signal = (int)pid_controle(TI);
   // printf("pid: %d", control_signal);
    ComunicaUartSendInt(COD_SEND, SEND_CONTROL_SIGNAL, control_signal, 4);

    if (control_signal < 0.0)
    {
      printf("LIGA VENTO\n");
      control_signal = control_signal > -40 ? 40 : control_signal * (-1);
      gelaForno(control_signal);
      esquentaForno(0);
    }

    // pid: se positivo -> resistencia
    else if (control_signal > 0.0)
    {
      printf("LIGA RESISTENCIA");
      esquentaForno(control_signal);
      gelaForno(0);
    }

    ComunicaUartSendInt(COD_SEND, SEND_SYSTEM_STATE, ligado, 1);
    ComunicaUartSendInt(COD_SEND, SEND_FUNCTIONING_STATE, funcionando, 1);

    if (modoTR)
    {
      ComunicaUartSendFloat(COD_SEND, SEND_REFERENCE_SIGNAL, referencia_, 4);
    }
    else
    {
      referencia_ = LeTempRef();
      pid_atualiza_referencia(referencia_);
    }

    ComunicaUartSendFloat(COD_SEND, SEND_TEMP_AMB, TE, 4);
    sleep(1);
  }
}
void trata_SIGINT()
{
  exit(1);
}
int main()
{

  init();

  signal(SIGINT, trata_SIGINT);

  pthread_create(&menu, NULL, menuFunc, NULL);
  pthread_create(&recebe, NULL, recebeComandos, NULL);
  pthread_create(&controla, NULL, controlaTemp, NULL);

  pthread_join(menu, NULL);
  pthread_join(recebe, NULL);
  pthread_join(controla, NULL);
}