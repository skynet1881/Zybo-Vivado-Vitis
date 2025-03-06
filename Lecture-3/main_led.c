#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#define GPIO_DEVICE_ID XPAR_GPIO_0_DEVICE_ID
#define GPIO_LED_DEVICE_ID XPAR_AXI_GPIO_LED_DEVICE_ID
#define GPIO_INTR_ID XPAR_FABRIC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR

// GPIO Instance
XGpio Gpio;
XGpio GpioLed;

// Interrupt Controller Instance
XScuGic ScuGic;

void ButtonInterruptHandler(void *CallbackRef)
{
    xil_printf("Button Handler Called\r\n");

    int btnState = XGpio_DiscreteRead(&Gpio, 1);
    xil_printf("Button State: %x\r\n", btnState);

    // Write Button State to LED
    XGpio_DiscreteWrite(&GpioLed, 1, btnState);

    // Clear Interupt
    XGpio_InterruptClear(&Gpio, 0xF);
}

int InterruptSetup(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr)
{
    int status;
    XScuGic_Config *config;

    xil_printf("Init Interrupt Controller\r\n");

    // Lookup GIC config
    config = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
    if(config == NULL)
    {
        return XST_FAILURE;
    }

    // Init GIC
    status = XScuGic_CfgInitialize(GicInstancePtr, config, config->CpuBaseAddress);
    if(status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }

    // Connect GIC to GPIO Interurpt Handler
    status = XScuGic_Connect(GicInstancePtr, GPIO_INTR_ID, (Xil_ExceptionHandler)ButtonInterruptHandler, (void *)GpioInstancePtr);
    if(status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }

    // Enable GIC
    XScuGic_Enable(GicInstancePtr, GPIO_INTR_ID);

    // Exception Enable
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, GicInstancePtr);

    Xil_ExceptionEnable();

    xil_printf("GIC interrupt setup complete\r\n");

    return XST_SUCCESS;
}

int main()
{
    int status;

    xil_printf("GPIO Interrupt Example\r\n");

    // Init GPIO
    status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
    if(status != XST_SUCCESS)
    {
        xil_printf("GPIO Initialization Failed\r\n");
        return XST_FAILURE;
    }

    // Set GPIO Direction
    XGpio_SetDataDirection(&Gpio, 1, 0xF); // input 1111

    // Enable GPIO Interrupt
    XGpio_InterruptEnable(&Gpio, 0xF); // 1111
    XGpio_InterruptGlobalEnable(&Gpio);

    // Init LED
    status = XGpio_Initialize(&GpioLed, GPIO_LED_DEVICE_ID);
    if(status != XST_SUCCESS)
    {
        xil_printf("GPIO LED Initialization Failed\r\n");
        return XST_FAILURE;
    }

    // Set LED Direction
    XGpio_SetDataDirection(&GpioLed, 1, 0x0); // output 0000

    // Init and enable GIC
    status = InterruptSetup(&ScuGic, &Gpio);
    if(status != XST_SUCCESS)
    {
        xil_printf("GIC Setup Failed\r\n");
        return XST_FAILURE;
    }

    while(1); // Infite loop

    return 0;
}
