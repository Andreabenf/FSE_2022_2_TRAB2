
#ifndef  UART_H
#define  UART_H


#define COD_REQ            0x23  
#define COD_SEND            0x16  

#define RX_BUFFER        256   


#define REQ_TI   0xC1 
#define REQ_TR  0xC2 
#define REQ_DASH_COMAND      0xC3 

void openUart(char *path);
void closeUart();
void writeUart(char *package, int pkgLength);

void setCrc(char *package, int pkgLength);
char VerifCrc(char *package, int pkgLength);

void getResponse();

float leTempInterna();
float LeTempRef();
int LeComandos();

#endif