#include "sys.h"
#include "delay.h"
#include "usart.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	����ԭ�� Pandora STM32L475 IoT������	ʵ��0-1
 *	Template����ģ��-�½������½�ʹ��
 *	����֧�֣�www.openedv.com
 *	�Ա����̣�http://openedv.taobao.com
 *	��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 *	������������ӿƼ����޹�˾
 *	���ߣ�����ԭ�� @ALIENTEK
 *	******************************************************************************/


void Delay(__IO uint32_t nCount);
void Delay(__IO uint32_t nCount)
{
    while(nCount--) {};
}

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    HAL_Init();                         //��ʼ��HAL��
    SystemClock_Config();	            //��ʼ��ϵͳʱ��Ϊ80M
	
    __HAL_RCC_GPIOE_CLK_ENABLE();       //����GPIOEʱ��

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 	//�������
    GPIO_InitStruct.Pull = GPIO_PULLUP;				//����
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	//����
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    while(1)
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET); 	//PE7 ��1
        Delay(0x7FFFFF);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);	//PE7 ��0
        Delay(0x7FFFFF);
    }
}


