#include <stdio.h>
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"

#define BTN_DEVICE_ID XPAR_AXI_GPIO_BTN_DEVICE_ID
#define BTN_CHANNEL 1

int main()
{
    XGpio button;
    int status;
    u32 btn_val;

    status = XGpio_Initialize(&button, BTN_DEVICE_ID);
    if (status != XST_SUCCESS) {
        printf("Button init failed!\n");
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(&button, BTN_CHANNEL, 0xFF); // Set as input

    while (1) {
        btn_val = XGpio_DiscreteRead(&button, BTN_CHANNEL);
        printf("Button value: 0x%X\n", btn_val);
        usleep(250000);
    }

    return 0;
}
