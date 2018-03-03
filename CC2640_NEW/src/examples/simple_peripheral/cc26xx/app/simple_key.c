#include "simple_key.h"
#include "board_key.h"
#include "simple_uart.h"
#include "simple_led.h"

static void processKeyEvent(uint8_t keyPressed)
{
    if (keyPressed & KEY_SELECT)
    {
        ledToggle(LED_INDEX_0);
    }
    DEBUG_STRING("Hello\r\n");
}

void keyInit()
{
    Board_initKeys(processKeyEvent);
}