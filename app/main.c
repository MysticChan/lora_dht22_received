#include "uart.h"
#include "led.h"
#include "timer.h"
#include "SX1278.h"
const uint8_t respond[2]={0x55,0xaa};
void CLK_Init(void);
uint8_t buf[5];
void main(void)
{
  uint8_t len,res,status;
  uint16_t n = 0;
  uint16_t hum,tem;
  uint8_t rssi;
  status = SET;
  CLK_Init();
  uart_init(115200);
  enableInterrupts();
  LED_Init();
  TimerDelay_Init();
  TimerBlink_Init();
  SX1278_Init();
  while(1)
  {
    res = SX1278_LoRaRxPacket(buf,&len,1000);
    if(res == 0)
    {
      printf("Received package. \n");
      if(len == 5)
      {
        res = buf[0]+buf[1]+buf[2]+buf[3];
        if(res == buf[4])
        {
          printf("SUM Chack pass. Responding ...\n");
          if( SX1278_LoRaTxPacket(respond, 2) )
          {
            printf("TX fail. \n");
          }
          
          n++;
          hum = 0;
          hum |= buf[0];
          hum <<= 8;
          hum |= buf[1];
          tem = 0;
          tem |= buf[2];
          tem <<= 8;
          tem |= buf[3];
          rssi = SX1278_ReadRSSI();
          printf("Package NO.%d .\n", n);
          printf("DHT22: Humidity = %d (0.1%%).\n",hum);
          printf("DHT22: Temperature = %d (0.1'C).\n",tem);
          printf("sx1278:RSSI = %d \n", rssi);
          if(status == RESET)
          {
            BlinkNormal();
            status = SET;
          }
          
        }
      }
    }
    else if(res == 1)
    {
        printf("Time out!\r\n");
        if(status == SET)
        {
          BlinkFast();
          status = RESET;
        }
    }
    else if(res == 2)
    {
        printf("CRC eeror!\r\n");
        if(status == SET)
        {
          BlinkFast();
          status = RESET;
        }
    }
    
  }
}

void CLK_Init(void)
{
    CLK_DeInit();
    CLK_HSECmd(ENABLE);
    while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_ClockSwitchCmd(ENABLE); 
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO , CLK_SOURCE_HSE , DISABLE ,CLK_CURRENTCLOCKSTATE_DISABLE);
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
