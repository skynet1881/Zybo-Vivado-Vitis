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
    int count = 0;

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
        printf("Enter led value: ");
        scanf("%d", &count);
        
        // 1 -> 0001 -> LED1
        // 3 -> 0011 -> LED1, LED2
        XGpio_DiscreteWrite(&led, LED_CHANNEL, count);
        sleep(1);
    }

    return 0;
}
