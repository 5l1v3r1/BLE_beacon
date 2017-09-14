#include "simpleBLEUart.h"
#include "npi.h"
#include "hal_uart.h"
#include "hal_led.h"

#define BLE_UART_BR HAL_UART_BR_115200

void ble_uart_init()
{
    NPI_InitTransport(NpiSerialCallback);
    NPI_WriteTransport("SimpleBLETest_Init\r\n", 20);
    NPI_PrintString("UART Init Done");
}

// ���ڻص������� ����Ѹûص�������ʵ�ֵĹ��ܽ���һ��
/*
1, ˼·:  �������յ����ݺ󣬾ͻ����ϵ������»ص���������ʵ�ʲ����з��֣��˻ص�
��������Ƶ���� ����㲻ִ��NPI_ReadTransport�������ж�ȡ�� ��ô����ص������ͻ�
Ƶ���ر�ִ�У����ǣ���ͨ�����ڷ���һ�����ݣ� �㱾�����봦����һ����һ�ε����ݣ����ԣ�
����������������ʱ��Ĵ������� Ҳ�����յ����ݹ�����߳�ʱ���Ͷ�ȡһ�����ݣ� 
Ȼ����ݵ�ǰ��״̬����ִ�У����û�������ϣ��Ͱ��������ݵ���AT����� �������
���ˣ��Ͱ������͵��Զˡ�  ---------------amomcu   2014.08.17
*/
static void NpiSerialCallback(uint8 port, uint8 events)
{
    (void)port;                                            //�Ӹ� (void)����δ�˱������澯����ȷ���߻�������������������
    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL)) //����������
    {
        uint8 numBytes = 0;

        numBytes = NPI_RxBufLen(); //�������ڻ������ж����ֽ�

        if (numBytes == 0)
        {
            return;
        }
        else
        {
            //���뻺����buffer
            uint8 *buffer = osal_mem_alloc(numBytes);
            if (buffer)
            {
                //��ȡ��ȡ���ڻ��������ݣ��ͷŴ�������
                NPI_ReadTransport(buffer, numBytes);

                //���յ������ݷ��͵�����-ʵ�ֻػ�
                NPI_WriteTransport(buffer, numBytes);
                
                // Process the command.
                ble_command_parse(buffer, numBytes);
                
                //�ͷ�����Ļ�����
                osal_mem_free(buffer);
            }
        }
    }
}

/*
  0xDE
  0xAD // Start flag.
  0x01 // TYPE: LED
  0x31 // LED1/LED2/LED3 HAL_LED_MODE_ON
  */  // Total 4 bytes. StartFlag, TYPE, VALUE
static uint8 command[2];
static uint8 command_len;
static void ble_command_parse(uint8 *buffer, uint8 numBytes)
{
    uint8 i;
    for (i = 0; i < numBytes; i++)
    {
        if (command_len < 2)
        {
            switch(buffer[i])
            {
                case 0xDE:
                if (command_len == 0)
                    command_len++;
                else
                    command_len = 0;
                break;
                case 0xAD:
                if (command_len == 1)
                    command_len++;
                else
                    command_len = 0;
                break;
                default:
                break;
            }
            continue;
        }
        else
        {
            command[command_len-2] = buffer[i];
            command_len++;
        }
    }
    if (command_len == 4) {
        ble_process_command(command);
        osal_memset(command, 0, 2);
    }
}

static void ble_process_command(uint8 *buffer)
{
    switch (buffer[0])
    {
        case 0x01:
        //LED CASE
        HalLedSet(buffer[1] >> 4, buffer[1]&0xF);
        break;
        default:
        break;
    }
}
