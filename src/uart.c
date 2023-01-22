
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
float restemp = 0;

void gettemp()
{
   printf("%f", restemp);
   return;
}

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

ComunicaUartReq(char prefix, char dataType)
{
   char *package = (char *)malloc(pkgLength * sizeof(char));

   montaPack(package, prefix, dataType);

   setCrc(package, pkgLength);

   writeUart(package, pkgLength);

   free(package);

   getResponse();
}


void ComunicaUartSendInt(char prefix, char dataType, int payload, int size)
{
   int newPkg= pkgLength+size;
   char *package = (char *)malloc(newPkg * sizeof(char));

   montaPack(package, prefix, dataType);
   memcpy(&package[7], &payload, 4);
   setCrc(package, newPkg);

   writeUart(package, newPkg);

   free(package);

   getResponse();
}

void ComunicaUartSendFloat(char prefix, char dataType, float payload, int size)
{
   int newPkg= pkgLength+size;
   char *package = (char *)malloc(newPkg * sizeof(char));

   montaPack(package, prefix, dataType);
   memcpy(&package[7], &payload, 4);
   setCrc(package, newPkg);

   writeUart(package, newPkg);

   free(package);

   getResponse();
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

void getResponse()
{
   memset(responsePackage, 0x00, RX_BUFFER);

   sleep(1);

   globalResPkgLen = read(uart0_filestream, (void *)responsePackage, RX_BUFFER);

   if (globalResPkgLen < 0)
   {
      //printf("getResponse: read error\n");
   }
   else if (globalResPkgLen == 0)
   {
      //printf("getResponse: got no response\n");
      return;
   }
   else
   {
      responsePackage[globalResPkgLen] = '\0';

      char crcVerified = VerifCrc(responsePackage, globalResPkgLen);

      if (!crcVerified)
      {
         printf("getResponse: Erro no match CRC\n");
         return;
      }
      //printf("getResponse: received %d bytes\n", globalResPkgLen);
   }

   return;
}

float leTempInterna()
{
   ComunicaUartReq(COD_REQ, REQ_TI);

   float internalTemperature;
   memcpy(&internalTemperature, &responsePackage[3], 4);
   memcpy(&restemp, &responsePackage[3], 4);

   printf("leTempInterna: int. temp. is %f\n", internalTemperature);
   return internalTemperature;
}

float LeTempRef()
{
   ComunicaUartReq(COD_REQ, REQ_TR);

   float referenceTemperature;
   memcpy(&referenceTemperature, &responsePackage[3], 4);
   printf("LeTempRef: ref. temp. is %f\n", referenceTemperature);

   return referenceTemperature;
}

int LeComandos()
{

   ComunicaUartReq(COD_REQ, REQ_DASH_COMAND);

   int comandorec;
   memcpy(&comandorec, &responsePackage[3], 4);
   // printf("LeComandos: %d\n", comandorec);

   return comandorec;
}
