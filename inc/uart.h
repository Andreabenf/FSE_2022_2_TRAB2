
#ifndef UART_H
#define UART_H

#define COD_REQ 0x23
#define COD_SEND 0x16

#define RX_BUFFER 30

#define REQ_TI 0xC1
#define REQ_TR 0xC2
#define REQ_DASH_COMAND 0xC3

#define SEND_CONTROL_SIGNAL      0xD1  
#define SEND_REFERENCE_SIGNAL    0xD2  
#define SEND_SYSTEM_STATE        0xD3  
#define SET_CONTROL_MODE         0xD4  
#define SEND_FUNCTIONING_STATE   0xD5  
#define SEND_TEMP_AMB         0xD6 

#define LIGA_FORNO         0xA1
#define DESLIGA_FORNO        0xA2
#define INICIA_AQUEC   0xA3
#define CANCELA 0xA4
#define MENU    0xA5

#define QUATRO_BYTES 4
#define UM_BYTE 1


static int uart0_filestream;
typedef struct Number_type
{
   int int_value;
   float float_value;
} Number_type;
void openUart(char *path);
void closeUart();
void writeUart(char *package, int pkgLength);
Number_type readFromUart(unsigned char code);
void setCrc(char *package, int pkgLength);
char VerifCrc(char *package, int pkgLength);
ComunicaUartReq(char prefix, char dataType);
void getResponse();
void montaPack(char *package, char prefix, char dataType);
int LeComandos();
float LeTempRef();
float leTempInterna();
void ComunicaUartSendInt(char prefix, char dataType, int payload, int size);
void ComunicaUartSendFloat(char prefix, char dataType, float payload, int size);
static char responsePackage[RX_BUFFER + 1];
static int globalResPkgLen;

#endif