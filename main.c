/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tw88.h"
#include "ar1100.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define RX_BUFFER_LENGTH	(40)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


RCC_ClocksTypeDef RCC_Clocks;

uint8_t rx_buffer[RX_BUFFER_LENGTH];
uint8_t rx_counter = 0;


void SerialPort_Init(uint32_t speed, uint8_t enable_recvirq);
uint8_t USART2_ReadChar(void);
volatile TouchCoords touch_coords;

void onTouchScreenEvent(TouchCoords coords);











void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        rx_buffer[rx_counter] = (USART_ReceiveData(USART2) & 0x7F);

        if(rx_counter + 1 == RX_BUFFER_LENGTH ||
                rx_buffer[rx_counter] == '\n' || rx_buffer[rx_counter] == '\r')
        {
            printf("%s\n\r", rx_buffer);
            memset(rx_buffer, 0, RX_BUFFER_LENGTH);
            rx_counter = 0;
        }
        else
        {
            rx_counter++;
        }
    }
}


uint8_t USART2_ReadChar()
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART2);
}



// PD5 -> TX and PD6 -> RX for USART2

void SerialPort_Init(uint32_t speed, uint8_t enable_recvirq)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

    /* Enable USART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_7);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_7);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* USART configuration */
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    if(enable_recvirq)
    {
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

        /* Enable the USART2 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);	
	
	
}


void onTouchScreenEvent(TouchCoords coords)
{
	  //here we will receive complete touch screen events 
		
	printf("Received Tocuh event: X = %d Y=%d Pen state: %d \r\n",coords.x,coords.y,coords.p);
	STM_EVAL_LEDToggle(LED4);
	
}










int main(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;								//structure for GPIO setup
	//TouchInitStr touch_init;
	
	I2C_InitTypeDef					I2C_InitStructure; 		 	
	

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	
	
 
	touch_coords.processed=1;
	
	

  /* Initialize LEDs, Key Button, LCD and COM port(USART) available on
     STM32303C-EVAL board *****************************************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
	
	STM_EVAL_LEDInit(LED9);
	

	//init now the TW88 chip on I2C 1
	
	//I2C init ...on PB6 And PB7
	
	/* Configure the I2C clock source. The clock is derived from the SYSCLK */
  RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);													

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	

	
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_4);  // SCL
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_4);	// SDA


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;																
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;


  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_6);	
  GPIO_Init(GPIOB, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7);
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//init our serial port on UART2
	SerialPort_Init(115200,1);
	
	
	printf("\n\r ***** COEN490/ELEC490 Capstone Project Team 14 Welcome! ***** \n\r");
	
	
	I2C_DeInit(I2C1);
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	//I2C_InitStructure.I2C_Timing = 0x50330309; //0xB0420F13;    ///0x00902025; // 0xc062121f
	I2C_InitStructure.I2C_Timing = 0x00310309; 
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);



	
	Delay(1);	
	
  //init TW88
	TW88_Init();
	
	
	//add the osd
	TW88_AddOSD_Win(0,0,0x3F,1);
	
	
	//touch_init.onTouchEvent = onTouchScreenEvent;
	AR1100Init();
	
	
	
  /* Infinite loop */
  while (1)
  {
	
		if (rx_counter>0) {
			printf("Recv: %d",rx_buffer[0]);
			memset(rx_buffer, 0, RX_BUFFER_LENGTH);
			rx_counter=0;
		}
		
		
		if (touch_coords.processed==0) {
			 touch_coords.processed=1;
			 onTouchScreenEvent(touch_coords);
		}
		
		
    /* Toggle LD1 */
    STM_EVAL_LEDToggle(LED1);

    /* Insert 50 ms delay */
    Delay(50);

    /* Toggle LD2 */
    STM_EVAL_LEDToggle(LED2);

    /* Insert 50 ms delay */
    Delay(50);

    /* Toggle LD3 */
    STM_EVAL_LEDToggle(LED3);

    /* Insert 50 ms delay */
    Delay(50);

    /* Toggle LD4 */
    //STM_EVAL_LEDToggle(LED4);

    /* Insert 50 ms delay */
    //Delay(50);
		

		
		
  }
}
















