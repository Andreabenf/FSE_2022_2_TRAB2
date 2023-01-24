

#include "pid.h"
#include "bme280.h"
#include "sensortemp.h"
#include "gpio.h"
#include "uart.h"
#include "pid.h"
#include "stdio.h"
#include <pthread.h>
#include <signal.h>

int ligado = 0;
int funcionando = 0;
double referencia_ = 0.0;
double Kp_ = 30.0;  // Ganho Proporcional
double Ki_ = 0.2;   // Ganho Integral
double Kd_ = 400.0; // Ganho Derivativo
int modoTR = 0;
pthread_t controla;
pthread_t recebe;
pthread_t menu;
int ret1;
FILE *fpt;

float ti_tmp = 1;
float tr_tmp = 1;

void init()
{
  connectBme();
  openUart("/dev/serial0");
  fpt = fopen("Logs.csv", "w+");
  fprintf(fpt, "data_e_hora, temperatura_interna, temperatura_externa, temperatura_user, atuadores(%%)\n");
  ComunicaUartSendInt(COD_SEND, SEND_SYSTEM_STATE, ligado, UM_BYTE);
  ComunicaUartSendInt(COD_SEND, SEND_FUNCTIONING_STATE, funcionando, UM_BYTE);
  pid_configura_constantes(Kp_,Ki_,Kd_);
  initWiringPi();
}

void logCsv(float temperatura_interna, float temperatura_externa, float temperatura_user, float atuadores)
{
  time_t t = time(NULL);         // Get current time
  struct tm *tm = localtime(&t); // Convert to local time
  char s[25];

  // Format the timestamp as a string (e.g., "2018-12-21 12:34:56")
  printf("escrevente...\n");
  strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S'", tm);
  fprintf(fpt, "%s, %f, %f, %f\n", s, temperatura_interna, temperatura_externa, temperatura_user, atuadores);
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
  int modo = i;
  printf("enviando...\n");
  ComunicaUartSendInt(COD_SEND, SET_CONTROL_MODE, modo, UM_BYTE);
  modoTR = 0;
  if (i == 1)
  {
    printf("Escreva o valor de: \nTR: ");
    scanf("%lf", &referencia_);
    modoTR = 1;
    ComunicaUartSendFloat(COD_SEND, SEND_REFERENCE_SIGNAL, referencia_, QUATRO_BYTES);
  }

  __fpurge(stdin);
}

void *controlaTemp()
{
  while (funcionando)
  {
    float TAMB = getCurrentTemperature();
    // float TAMB = 69.9;

    printf("TAMB: %.2f⁰C\n", TAMB);

    float TI = leTempInterna(modoTR);
     if (TI <= 0.5)
    {
      TI = ti_tmp;
    }
    else
    {
      ti_tmp = TI;
    }
    printf("Ti: %f⁰C\n", (TI));
    int control_signal = (int)pid_controle(TI);
    printf("pid: %d\n", control_signal);
    ComunicaUartSendInt(COD_SEND, SEND_CONTROL_SIGNAL, control_signal, QUATRO_BYTES);

    if (control_signal < 0.0)
    {
      printf("[ESFRIANDO] Ligando ventoinha...\n");
      control_signal = control_signal > -40 ? 40 : control_signal * (-1);
      gelaForno(control_signal);
      esquentaForno(0);
    }
    else if (control_signal > 0.0)
    {
      printf("[ESQUENTANDO] Ligando resistencia...\n");
      esquentaForno(control_signal);
      gelaForno(0);
    }
    if (modoTR)
    {
      printf("setando...\n");
      ComunicaUartSendFloat(COD_SEND, SEND_REFERENCE_SIGNAL, referencia_, QUATRO_BYTES);
    }
    else
    {
      
      referencia_ = LeTempRef();
      if(referencia_<=0.5){
        referencia_=tr_tmp;
      }else{
        tr_tmp=referencia_;
      }
      printf("TR lida: %f\n", referencia_);
    }
      pid_atualiza_referencia(referencia_);

    ComunicaUartSendFloat(COD_SEND, SEND_TEMP_AMB, TAMB, QUATRO_BYTES);
    logCsv(TI, TAMB, referencia_, control_signal);
    delay(1000);
  }
}

void recebeComandos()
{
  do
  {
    int resp = LeComandos();
    switch (resp)
    {
    case LIGA_FORNO:
      printf("LIGA_FORNO!!!\n");
      ligado = 1;
      ComunicaUartSendInt(COD_SEND, SEND_SYSTEM_STATE, ligado, UM_BYTE);

      break;
    case DESLIGA_FORNO:
      printf("DESLIGA_FORNO!!!\n");
      ligado = 0;
      ComunicaUartSendInt(COD_SEND, SEND_SYSTEM_STATE, ligado, UM_BYTE);

      break;
    case INICIA_AQUEC:
      printf("INICIA_AQUEC!!!\n");
      funcionando = 1;
      ComunicaUartSendInt(COD_SEND, SEND_FUNCTIONING_STATE, funcionando, UM_BYTE);
      pthread_create(&controla, NULL, controlaTemp, NULL);

      break;
    case CANCELA:
      printf("CANCELA!!!\n");
      funcionando = 0;
      ComunicaUartSendInt(COD_SEND, SEND_FUNCTIONING_STATE, funcionando, UM_BYTE);
      system("clear");
      system("clear");
      printf("\nMenu liberado!\nAperte a tecla 2! \n");
      break;
    case MENU:
      printf("MENU!!!\n");
      break;
    }
    delay(500);
  } while (1);
}

void *menuFunc()
{
  int menuon = 1;
  int i;
  do
  {
    system("clear");
    printf("Modo: %s\n", modoTR ? "Terminal" : "DashBoard");
    printf("Kp: %lf\n", Kp_);
    printf("Ki: %lf\n", Ki_);
    printf("Kd: %lf\n", Kd_);
    printf("TR: %lf\n\n", referencia_);
    printf("Digite 1 para Decidir MODO TR \n");
    printf("Digite 2 para definir os valores das constantes Kp, Ki e Kd\n");
    printf("Ou espere o forno ser acionado...\n");

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
      printf("Ki: ");

      scanf("%lf", &Ki_);
      printf("Kd: ");

      scanf("%lf", &Kd_);
       pid_configura_constantes(Kp_,Ki_,Kd_);
    }

    __fpurge(stdin);
  } while (!funcionando);
}
void trata_SIGINT()
{
  printf("\nDesligando...\n");
  closeUart();
  gelaForno(0);
  esquentaForno(0);
  fclose(fpt);
  exit(1);
}
int main()
{
  init();
  signal(SIGINT, trata_SIGINT);
  pthread_create(&menu, NULL, menuFunc, NULL);

  recebeComandos();
}