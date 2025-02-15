#include <stdio.h>
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"

#define LED_DEVICE_ID XPAR_AXI_GPIO_LED_DEVICE_ID
#define LED_CHANNEL 1

int main() 
{   
    XGpio led;
    int status;

    // Init GPIO
    status = XGpio_Initialize(&led, LED_DEVICE_ID);
    if(status != XST_SUCCESS)
    {
        printf("Error initializing GPIO\n");
        return XST_FAILURE;
    }

    // Set LED as output
    XGpio_SetDataDirection(&led, LED_CHANNEL, 0x00);

    while(1)
    {
        // 0x0F -> 0000 1111 -> LED 0, 1, 2, 3, ON
        XGpio_DiscreteWrite(&led, LED_CHANNEL, 0x0F);
        printf("LED ON\n");
        sleep(1);

        // 0x00 -> 0000 0000 -> LED 0, 1, 2, 3, OFF
        XGpio_DiscreteWrite(&led, LED_CHANNEL, 0x00);
        printf("LED OFF\n");
        sleep(1);
    }

    return 0;
}
