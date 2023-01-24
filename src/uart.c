
#include "uart.h"

#include "crc.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>

static int uart0_filestream = -1;
int pkgLength = 7 + 2;
static struct termios options;

static char responsePackage[RX_BUFFER + 1];
static int globalResPkgLen = 0;
void openUart(char *path)
{
   uart0_filestream = open(path, O_RDWR | O_NOCTTY | O_NDELAY);

   if (uart0_filestream == -1)
   {
      printf("Error openning UART");
      exit(1);
   }
   tcgetattr(uart0_filestream, &options);

   options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
   options.c_iflag = IGNPAR;
   options.c_oflag = 0;
   options.c_lflag = 0;

   tcflush(uart0_filestream, TCIFLUSH);
   tcsetattr(uart0_filestream, TCSANOW, &options);

   return;
}

void closeUart()
{
   close(uart0_filestream);
}

void writeUart(char *package, int pkgLength)
{
   int numOfBytesWritten = 0;
   numOfBytesWritten = write(uart0_filestream, &package[0], pkgLength);

   return;
}

Number_type readFromUart(unsigned char code)
{
   unsigned char buffer[10];
   unsigned char reference;
   Number_type number = {-1, -1.0};

   int content = read(uart0_filestream, (void*)buffer, 9);
   printf("%d bytes: \n", content);
   if (!content)
   {
      printf("Nenhum dado foi recebido\n");
   }
   else if (content < 0)
   {
      printf("Erro ao ler dados\n");
   }
   else
   {
      buffer[content] = '\0';


      if (code == REQ_DASH_COMAND )
      {
         memcpy(&number.int_value, &buffer[3], 4);
      }
      else
      {
         printf("%c tezste\n", buffer[3]);
         memcpy(&number.float_value, &buffer[3], 4);
      }
      return number;
   }
   delay(300);
   return number;
}

ComunicaUartReq(char prefix, char dataType)
{
   char *package = (char *)malloc(pkgLength * sizeof(char));

   montaPack(package, prefix, dataType);

   setCrc(package, pkgLength);

   writeUart(package, pkgLength);

   free(package);
   sleep(1);
   // if (dataType != REQ_DASH_COMAND)
   // {

   //    getResponse();
   // }
}

void ComunicaUartSendInt(char prefix, char dataType, int payload, int size)
{
   int newPkg = pkgLength + size;
   char *package = (char *)malloc(newPkg * sizeof(char));

   montaPack(package, prefix, dataType);
   memcpy(&package[7], &payload, 4);
   setCrc(package, newPkg);

   writeUart(package, newPkg);

   free(package);
   sleep(1);
}
void getResponse()
{
   memset(responsePackage, 0x00, RX_BUFFER);

   globalResPkgLen = read(uart0_filestream, (void *)responsePackage, RX_BUFFER);

   if (globalResPkgLen < 0)
   {
      // printf("getResponse: read error\n");
   }
   else if (globalResPkgLen == 0)
   {
      // printf("getResponse: got no response\n");
      return;
   }
   else
   {
      responsePackage[globalResPkgLen] = '\0';
      printf("%f estesetseste\n\n", responsePackage[3]);
      char crcVerified = VerifCrc(responsePackage, globalResPkgLen);

      if (!crcVerified)
      {
         printf("getResponse: Erro no match CRC\n");
         return;
      }
      // printf("getResponse: received %d bytes\n", globalResPkgLen);
   }
   sleep(1.2);
   return;
}

void ComunicaUartSendFloat(char prefix, char dataType, float payload, int size)
{
   int newPkg = pkgLength + size;
   char *package = (char *)malloc(newPkg * sizeof(char));

   montaPack(package, prefix, dataType);
   memcpy(&package[7], &payload, 4);
   setCrc(package, newPkg);

   writeUart(package, newPkg);

   free(package);
    sleep(1);
}
void setCrc(char *package, int pkgLength)
{
   short crc16 = calcula_CRC(package, pkgLength - 2);
   memcpy(&package[pkgLength - 2], &crc16, 2);

   return;
}

void montaPack(char *package, char prefix, char dataType)
{
   package[0] = 0x01;     // ESP
   package[1] = prefix;   // Pedido ou COmando
   package[2] = dataType; // Dado em si
   package[3] = 6;        // matricula
   package[4] = 1;        // matricula
   package[5] = 5;        // matricula
   package[6] = 5;        // matricula
   return;
}

char VerifCrc(char *package, int pkgLength)
{
   short crc16 = calcula_CRC(package, pkgLength - 2);
   short providedCrc = 0;
   memcpy(&providedCrc, &package[pkgLength - 2], 2);

   if (crc16 != providedCrc)
   {
      printf("VerifCrc: Erro CRC\n");
      return 0;
   }

   return 1;
}

float leTempInterna()
{
   ComunicaUartReq(COD_REQ, REQ_TR);
   float internalTemperature;
   internalTemperature= readFromUart(REQ_TR).float_value;
   // memcpy(&internalTemperature, &responsePackage[3], 4);

   // printf("leTempInterna: int. temp. is %f\n", internalTemperature);
   return internalTemperature;
}

float LeTempRef()
{
   ComunicaUartReq(COD_REQ, REQ_TI);
   float referenceTemperature;
   referenceTemperature= readFromUart(REQ_TI).float_value;
   // memcpy(&referenceTemperature, &responsePackage[3], 4);
   // printf("LeTempRef: ref. temp. is %f\n", referenceTemperature);

   return referenceTemperature;
}

int LeComandos()
{
   ComunicaUartReq(COD_REQ, REQ_DASH_COMAND);
   Number_type comandorec = readFromUart(REQ_DASH_COMAND);
   if (comandorec.int_value > 0 && comandorec.int_value < 170)
   {
      printf("LeComandos: %d\n\n\n\n\n\n\n\n", comandorec.int_value);
   }
   return comandorec.int_value;
}
