/*##############################################################################
lab 4 - Daiara e Poliana
##############################################################################*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h> // Inclui o tipo bool
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"

// defines GPIOS constantes
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

#define USER_LED1  GPIO_PIN_1 // PN_1
#define USER_LED2  GPIO_PIN_0 //PN_0
#define USER_LED3  GPIO_PIN_4 //PF_4
#define USER_LED4  GPIO_PIN_0 //PF_0


//variável que conta os ticks(1ms) - Volatile não permite o compilador otimizar o código 
volatile unsigned int SysTicks1ms;
//buffer de rx
unsigned char rxbuffer[4];

//status led
bool status_led[4];
//variável para receber o retorno do cfg do clk
uint32_t SysClock;
  
//Protótipos de funções criadas no programa, código depois do main
void SysTickIntHandler(void);
void UartSendString(const uint8_t *String, uint32_t tamanho);
void UART_Interruption_Handler(void);
void SystickSetup(void);
void UartSetup(void);
void GpioSetup(void);

int main(void)
{
  SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
  
  SystickSetup();  
  GpioSetup();
  UartSetup();  
  
  while (1)
  {
    if (rxbuffer[0]=='#' && rxbuffer[1]=='R')
    {
      if(rxbuffer[2]=='0')
      {
        if(rxbuffer[3]=='0')
        {
          GPIOPinWrite(GPIO_PORTN_BASE, USER_LED1, 0);
					status_led[0] = false;
          UartSendString("@R00", 5);
        }
        else if(rxbuffer[3]=='1')
        {
          GPIOPinWrite(GPIO_PORTN_BASE, USER_LED1, USER_LED1);
					status_led[0] = true;
          UartSendString("@R01", 5);
        }
      }
      else if(rxbuffer[2]=='1')
      {
        if(rxbuffer[3]=='0')
        {
          GPIOPinWrite(GPIO_PORTN_BASE, USER_LED2, 0);
					status_led[1] = false;
          UartSendString("@R10", 5);
        }
        else if(rxbuffer[3]=='1')
        {
          GPIOPinWrite(GPIO_PORTN_BASE, USER_LED2, USER_LED2);
					status_led[1] = true;
          UartSendString("@R11", 5);
        }
      }
      else if(rxbuffer[2]=='2')
      {
        if(rxbuffer[3]=='0')
        {
          GPIOPinWrite(GPIO_PORTF_BASE, USER_LED3, 0);
					status_led[2] = false;
          UartSendString("@R20", 5);
        }
        else if(rxbuffer[3]=='1')
        {
          GPIOPinWrite(GPIO_PORTF_BASE, USER_LED3, USER_LED3);
					status_led[2] = true;
          UartSendString("@R21", 5);
        }
      }
      else if(rxbuffer[2]=='3')
      {
        if(rxbuffer[3]=='0')
        {
          GPIOPinWrite(GPIO_PORTF_BASE, USER_LED4, 0);
					status_led[3] = false;
          UartSendString("@R30", 5);
        }
        else if(rxbuffer[3]=='1')
        {
          GPIOPinWrite(GPIO_PORTF_BASE, USER_LED4, USER_LED4);
					status_led[3] = true;
          UartSendString("@R31", 5);
        }
      }
       rxbuffer[0]=0;
    }
    
    if (rxbuffer[0]=='#' && rxbuffer[1]=='T' && rxbuffer[2]=='X')
    {
      if (rxbuffer[3]=='0')
      {
        GPIOPinWrite(GPIO_PORTN_BASE, USER_LED1 | USER_LED2, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, USER_LED3 | USER_LED4, 0);
				for(int i =0; i<4;i++){
					status_led[i] = false;
				}
        UartSendString("@TX0", 5);
      }
      else if(rxbuffer[3]=='1')
      {
        GPIOPinWrite(GPIO_PORTN_BASE, USER_LED1 | USER_LED2, USER_LED1 | USER_LED2);
        GPIOPinWrite(GPIO_PORTF_BASE, USER_LED3 | USER_LED4, USER_LED3 | USER_LED4);
				for(int i =0; i<4;i++){
					status_led[i] = true;
				}
        UartSendString("@TX1", 5);
      }
       rxbuffer[0]=0;
    } 
		
		    if (rxbuffer[0]=='#' && rxbuffer[1]=='T' && rxbuffer[2]=='S'&& rxbuffer[3]=='T')
			{
				int valor_decimal = 0;
				for (int i = 0; i < 4; i++) {
					valor_decimal = valor_decimal * 2 + status_led[i]; // Desloca e soma o valor booleano atual
				}
				valor_decimal += 64;
				char ascii_char = (char)valor_decimal;
			 // Criação da string resultante
        char result[6]; // '@TS' + ascii_char + '\0'
        result[0] = '@';
        result[1] = 'T';
        result[2] = 'S';
        result[3] = ascii_char;
        result[4] = '\0'; // Caractere nulo para terminar a string
				UartSendString(result, 5);
				rxbuffer[0]=0;
			}

  }
}
void GpioSetup(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {/*Espera habilitar o port*/}
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, USER_LED1 | USER_LED2);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {/*Espera habilitar o port*/}
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, USER_LED3 | USER_LED4);
}

//função de tratamento da interrupção do SysTick
void SysTickIntHandler(void)
{
  SysTicks1ms++;
}

//função para enviar string pela uart
void UartSendString(const uint8_t *String, uint32_t tamanho)
{
while (tamanho--) UARTCharPut(UART0_BASE, *String++);
}

//função de tratamento da interrupção do uart
void UART_Interruption_Handler(void) 
{
  uint8_t last;
  //limpar IRQ exec
  UARTIntClear(UART0_BASE,UARTIntStatus(UART0_BASE,true));
  // Ler o próximo caractere na uart.
  last = (uint8_t)UARTCharGetNonBlocking(UART0_BASE);
  //rotacionar buffer circular
  rxbuffer[0]=rxbuffer[1];
  rxbuffer[1]=rxbuffer[2];
  rxbuffer[2]=rxbuffer[3];
  rxbuffer[3]=last;
}

//função para configurar e inicializar o periférico Systick a 1ms
void SystickSetup(void)
{
  SysTicks1ms=0;
  //desliga o SysTick para poder configurar
  SysTickDisable();
  //clock 120MHz <=> SysTick deve contar 1ms=120k - 1 do Systick_Counter - 12 trocas de contexto PP->IRQ - (1T Mov, 1T Movt, 3T LDR, 1T INC ... STR e IRQ->PP já não contabilizam atrasos para a variável)  
  SysTickPeriodSet(120000-1-12-6);
  //registra a função de atendimento da interrupção
  SysTickIntRegister(SysTickIntHandler);
  //liga o atendimento via interrupção
  SysTickIntEnable();
  //liga novamente o SysTick
  SysTickEnable();
}

//função para configurar e inicializar o periférico Uart a 115.2k,8,n,1
void UartSetup(void)
{
  //Habilitar porta serial a 115200 com interrupção seguindo sequencia de inicializações abaixo:
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
  UARTConfigSetExpClk(UART0_BASE, SysClock, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART0_BASE);
  UARTIntEnable(UART0_BASE,UART_INT_RX);
  UARTIntRegister(UART0_BASE,UART_Interruption_Handler);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));
}