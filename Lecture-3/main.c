#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

// Define Zynq GIC IRQ ID (PL-to-PS Interrupt ID) - Usually 61 for GPIO
#define GPIO_INTR_ID    XPAR_FABRIC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR

// GPIO Instance
XGpio GpioBtn;

// GIC (Zynq Interrupt Controller) Instance
XScuGic ScuGic;

// 🔹 Button Interrupt Handler
void ButtonInterruptHandler(void *CallbackRef) {
    xil_printf("🚨 Button Interrupt Handler Called!\n");

    // Read Button State
    int btnState = XGpio_DiscreteRead(&GpioBtn, 1);
    xil_printf("Button Pressed! State: %d\n", btnState);

    // Clear the GPIO interrupt
    XGpio_InterruptClear(&GpioBtn, 0xF);
}

// 🔹 Interrupt Setup Function
int InterruptSetup(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr) {
    int Status;
    XScuGic_Config *Config;

    xil_printf("🔹 Initializing Zynq GIC...\n");

    // Lookup GIC Configuration
    Config = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
    if (Config == NULL) return XST_FAILURE;

    // Initialize GIC
    Status = XScuGic_CfgInitialize(GicInstancePtr, Config, Config->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("❌ GIC Initialization Failed!\n");
        return XST_FAILURE;
    }

    // Connect GIC to GPIO Interrupt Handler
    Status = XScuGic_Connect(GicInstancePtr, GPIO_INTR_ID,
                             (Xil_ExceptionHandler)ButtonInterruptHandler,
                             (void *)GpioInstancePtr);
    if (Status != XST_SUCCESS) {
        xil_printf("❌ GIC Interrupt Connection Failed!\n");
        return XST_FAILURE;
    }

    // Enable GPIO interrupt in GIC
    XScuGic_Enable(GicInstancePtr, GPIO_INTR_ID);

    // Enable Global Exception Handling
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 GicInstancePtr);
    Xil_ExceptionEnable();

    xil_printf("✅ GIC Interrupt Setup Complete!\n");
    return XST_SUCCESS;
}

// 🔹 Main Function
int main() {
    int Status;

    xil_printf("\n🚀 Starting GPIO Interrupt Example (Without AXI INTC)\n");

    // Initialize GPIO for button
    Status = XGpio_Initialize(&GpioBtn, XPAR_GPIO_0_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        xil_printf("❌ GPIO Initialization Failed!\n");
        return XST_FAILURE;
    }

    // Set GPIO direction (input for buttons)
    XGpio_SetDataDirection(&GpioBtn, 1, 0xF);

    // Enable GPIO Interrupts
    XGpio_InterruptEnable(&GpioBtn, 0xF);
    XGpio_InterruptGlobalEnable(&GpioBtn);

    // Initialize and enable GIC interrupts
    Status = InterruptSetup(&ScuGic, &GpioBtn);
    if (Status != XST_SUCCESS) {
        xil_printf("❌ Interrupt Setup Failed!\n");
        return XST_FAILURE;
    }

    xil_printf("✅ System Initialized. Waiting for Button Press...\n");

    while (1);  // Infinite loop

    return XST_SUCCESS;
}
